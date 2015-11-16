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
        fprintf(fp, content);
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
                                                                  false, false);

    ck_assert_ptr_ne(mr, NULL);

    mr_sequential_delete(mr);

    /* Delete testfile */
    remove(filename);
}
END_TEST


/*
START_TEST (test_put)
{
    Dictionary *dico = dictionary_create(256);

    dictionary_put_word(dico, "max");
    dictionary_put_word(dico, "sam");
    dictionary_put_word(dico, "lechuck");
    dictionary_put_word(dico, "guybrush");
    dictionary_put_word(dico, "fred");
    dictionary_put_word(dico, "sam");
    dictionary_put_word(dico, "samandmax");
    dictionary_put_word(dico, "bernard");
    dictionary_put_word(dico, "sam");
    dictionary_put_word(dico, "samm");

    ck_assert_int_eq(dictionary_count_word(dico, "max"), 1);
    ck_assert_int_eq(dictionary_count_word(dico, "sam"), 3);

    dictionary_delete(&dico);
}
END_TEST
*/

Suite *mapreduce_suite(void) {
    Suite *suite = suite_create("Mapreduce sequential");
    TCase *tcase1 = tcase_create("Case Create Delete");
    //TCase *tcase2 = tcase_create("Case Delete");
    //TCase *tcase3 = tcase_create("Case Put");

    tcase_add_test(tcase1, test_create_delete);
    //tcase_add_test(tcase2, test_delete);
    //tcase_add_test(tcase3, test_put);

    suite_add_tcase(suite, tcase1);
    //suite_add_tcase(suite, tcase2);
    //suite_add_tcase(suite, tcase3);

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
