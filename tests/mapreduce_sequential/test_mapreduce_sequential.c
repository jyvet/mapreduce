/*******************************************************************************
* Copyright (C) 2015, Jean-Yves VET, contact [at] jean-yves [dot] vet          *
*                                                                              *
* This software is licensed as described in the file LICENCE, which you should *
* have received as part of this distribution. You may opt to use, copy,        *
* modify, merge, publish, distribute and/or sell copies of the Software, and   *
* permit persons to whom the Software is furnished to do so, under the terms   *
* of the LICENCE file.                                                         *
*                                                                              *
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY    *
* KIND, either express or implied.                                             *
*******************************************************************************/

#include <check.h>
#include "mapreduce_sequential.h"


void create_file(const char *filename, const char *content) {
    FILE *fp;
    fp = fopen (filename,"w");
    if (fp!=NULL) {
        fprintf(fp, "%s", content);
        fclose (fp);
    }
}


START_TEST (test_create_delete)
{
    /* Create test file */
    char *filename = "ws_test.txt";
    char *content = "content tests";
    create_file(filename, content);

    Mapreduce *mr = mr_sequential_create(filename, TYPE_WORDSTREAMER_SCATTER,
                                                                   true, false);

    ck_assert(mr != NULL);

    mr_sequential_delete(mr);

    /* Delete testfile */
    remove(filename);
}
END_TEST


START_TEST (test_mapreduce)
{
    /* Create test file */
    char *filename = "ws_test.txt";
    char *content = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
                    "Donec a diam lectus. Sed sit amet ipsum mauris. Maecenas "
                    "congue ligula ac quam viverra nec consectetur ante "
                    "hendrerit. Donec et mollis dolor. Praesent et diam eget "
                    "libero egestas mattis sit amet vitae augue. Nam tincidunt "
                    "congue enim, ut porta lorem lacinia consectetur. Donec ut "
                    "libero sed arcu vehicula ultricies a non tortor. Lorem "
                    "ipsum dolor sit amet, consectetur adipiscing elit. Aenean "
                    "ut gravida lorem. Ut turpis felis, pulvinar a semper sed, "
                    "adipiscing id dolor. Pellentesque auctor nisi id magna "
                    "consequat sagittis. Curabitur dapibus enim sit amet elit "
                    "pharetra tincidunt feugiat nisl imperdiet. Ut convallis "
                    "libero in urna ultrices accumsan. Donec sed odio eros. "
                    "Donec viverra mi quis quam pulvinar at malesuada arcu "
                    "rhoncus. Cum sociis natoque penatibus et magnis dis "
                    "parturient montes, nascetur ridiculus mus. In rutrum "
                    "accumsan ultricies. Mauris vitae nisi at sem facilisis "
                    "semper ac in est.";

    create_file(filename, content);

    Mapreduce *mr = mr_sequential_create(filename, TYPE_WORDSTREAMER_SCATTER,
                                                                   true, false);
    ck_assert(mr != NULL);

    ck_assert(mr->ext != NULL);
    Mapreduce_sequential_ext *ext = (Mapreduce_sequential_ext *) mr->ext;

    ck_assert(ext->dictionary != NULL);
    Dictionary *dico = ext->dictionary;

    /* Perform map and reduce */
    mr_sequential_map(mr);
    mr_sequential_reduce(mr);

    /* Check some occurences */
    ck_assert_int_eq(mr_dictionary_count_word(dico, "adipiscing"), 3);
    ck_assert_int_eq(mr_dictionary_count_word(dico, "consectetur"), 4);
    ck_assert_int_eq(mr_dictionary_count_word(dico, "amet"), 5);
    ck_assert_int_eq(mr_dictionary_count_word(dico, "pharetra"), 1);
    ck_assert_int_eq(mr_dictionary_count_word(dico, "sit"), 5);
    ck_assert_int_eq(mr_dictionary_count_word(dico, "viverra"), 2);

    mr_sequential_delete(mr);
    remove(filename);
}
END_TEST


Suite *mapreduce_suite(void) {
    Suite *suite = suite_create("Mapreduce sequential");
    TCase *tcase1 = tcase_create("Case Create Delete");
    TCase *tcase2 = tcase_create("Case MapReduce");

    tcase_add_test(tcase1, test_create_delete);
    tcase_add_test(tcase2, test_mapreduce);

    suite_add_tcase(suite, tcase1);
    suite_add_tcase(suite, tcase2);

    return suite;
}


int main(void) {
    int number_failed;
    Suite *suite = mapreduce_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
