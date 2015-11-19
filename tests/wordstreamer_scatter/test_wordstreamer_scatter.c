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

#include "wordstreamer_scatter.h"
#include <check.h>

#define MAX_STREAMERS 11

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

    Wordstreamer *ws = mr_wordstreamer_scatter_create_first(filename, 1, false);
    ck_assert(ws != NULL);

    ck_assert_int_eq(ws->nb_streamers, 1);
    ck_assert_int_eq(ws->streamer_id, 0);
    ck_assert_int_eq(ws->start_offset, 0);
    ck_assert_int_eq(ws->profiling, 0);

    mr_wordstreamer_scatter_delete(ws);

    /* Delete testfile */
    remove(filename);
}
END_TEST


START_TEST (test_singlestreamer_get)
{
    /* Create test file */
    char *filename = "ws_test.txt";
    char *content = ".Donec!, ut  libero sed. ";
    create_file(filename, content);

    Wordstreamer *ws = mr_wordstreamer_scatter_create_first(filename, 1, false);

    char buffer[32];
    int ret;

    ret = mr_wordstreamer_scatter_get(ws, buffer);
    ck_assert_int_eq(ret, 0);
    ck_assert_str_eq(buffer, "donec");

    ret = mr_wordstreamer_scatter_get(ws, buffer);
    ck_assert_int_eq(ret, 0);
    ck_assert_str_eq(buffer, "ut");

    ret = mr_wordstreamer_scatter_get(ws, buffer);
    ck_assert_int_eq(ret, 0);
    ck_assert_str_eq(buffer, "libero");

    ret = mr_wordstreamer_scatter_get(ws, buffer);
    ck_assert_int_eq(ret, 0);
    ck_assert_str_eq(buffer, "sed");

    ret = mr_wordstreamer_scatter_get(ws, buffer);
    ck_assert_int_eq(ret, 1);

    mr_wordstreamer_scatter_delete(ws);

    /* Delete testfile */
    remove(filename);
}
END_TEST


START_TEST (test_multiplestreamer_get)
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

    /* Sequential streamer as a reference */
    Wordstreamer *ws = mr_wordstreamer_scatter_create_first(filename, 1, false);
    char ref[4096];
    char word[128];

    if (!mr_wordstreamer_scatter_get(ws, word)) strcpy(ref, word);

    while (!mr_wordstreamer_scatter_get(ws, word)) {
        strcat(ref, " ");
        strcat(ref, word);
    }

    mr_wordstreamer_scatter_delete(ws);


    /* Check several streamers combination */
    for (int i=2; i<=MAX_STREAMERS; i++) {
        char comp[4096];
        Wordstreamer *first_ws = mr_wordstreamer_scatter_create_first(filename,
                                                                      i, false);

        if (!mr_wordstreamer_scatter_get(first_ws, word)) strcpy(comp, word);

        while (!mr_wordstreamer_scatter_get(first_ws, word)) {
            strcat(comp, " ");
            strcat(comp, word);
        }

        for(int s=1; s<i; s++) {
            Wordstreamer *another_ws =
                            mr_wordstreamer_scatter_create_another(first_ws, s);

            while (!mr_wordstreamer_scatter_get(another_ws, word)) {
                strcat(comp, " ");
                strcat(comp, word);
            }

            mr_wordstreamer_scatter_delete(another_ws);
        }

        mr_wordstreamer_scatter_delete(first_ws);

        ck_assert_str_eq(comp, ref);
    }

    remove(filename);
}
END_TEST


Suite *wordstreamer_scatter_suite(void) {
    Suite *suite = suite_create("Wordstreamer Scatter");
    TCase *tcase1 = tcase_create("Case Create Delete");
    TCase *tcase2 = tcase_create("Case Single streamer Get");
    TCase *tcase3 = tcase_create("Case mutiple streamers Get");

    tcase_add_test(tcase1, test_create_delete);
    tcase_add_test(tcase2, test_singlestreamer_get);
    tcase_add_test(tcase3, test_multiplestreamer_get);

    suite_add_tcase(suite, tcase1);
    suite_add_tcase(suite, tcase2);
    suite_add_tcase(suite, tcase3);

    return suite;
}


int main(void) {
    int number_failed;
    Suite *suite = wordstreamer_scatter_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
