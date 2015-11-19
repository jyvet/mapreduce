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

#include "dictionary.h"
#include <check.h>

#define NB_TESTS 10000
#define MAX_CHAR 15


START_TEST (test_create)
{
    Dictionary *dico = mr_dictionary_create(0);
    ck_assert_int_eq(dico->hash_size, 256*256);

    mr_dictionary_delete(&dico);
}
END_TEST


START_TEST (test_delete)
{
    Dictionary *dico = mr_dictionary_create(0);

    mr_dictionary_delete(&dico);
    ck_assert(dico == NULL);
}
END_TEST


START_TEST (test_put)
{
    Dictionary *dico = mr_dictionary_create(0);

    mr_dictionary_put_word(dico, "max");
    mr_dictionary_put_word(dico, "sam");
    mr_dictionary_put_word(dico, "lechuck");
    mr_dictionary_put_word(dico, "guybrush");
    mr_dictionary_put_word(dico, "fred");
    mr_dictionary_put_word(dico, "sam");
    mr_dictionary_put_word(dico, "samandmax");
    mr_dictionary_put_word(dico, "bernard");
    mr_dictionary_put_word(dico, "sam");
    mr_dictionary_put_word(dico, "samm");

    ck_assert_int_eq(mr_dictionary_count_word(dico, "max"), 1);
    ck_assert_int_eq(mr_dictionary_count_word(dico, "sam"), 3);

    mr_dictionary_delete(&dico);
}
END_TEST


START_TEST (test_massive_put)
{
    int i;
    char buffer[MAX_CHAR+1];
    srand(1);

    Dictionary *dico = mr_dictionary_create(0);

    for (i=0; i<NB_TESTS; i++) {
        int nb_char = rand()%MAX_CHAR + 1;
        int j;

        /* Generate a random word */
        for (j=0; j<nb_char; j++) {
            unsigned char character = rand()%70+50;
            buffer[j] = (char)character;
        }
        buffer[j] = '\0';

        /* Put word in dictionnary  */
        mr_dictionary_put_word(dico, buffer);

        int count = mr_dictionary_count_word(dico, buffer);
        ck_assert(count >= 1);
    }

    mr_dictionary_delete(&dico);
}
END_TEST


START_TEST (test_merge)
{
    Dictionary *dico1 = mr_dictionary_create(0);
    Dictionary *dico2 = mr_dictionary_create(0);

    mr_dictionary_put_word(dico1, "Lorem");
    mr_dictionary_put_word(dico1, "Lorem");
    mr_dictionary_put_word(dico2, "Lorem");
    mr_dictionary_put_word(dico2, "Ipsum");

    mr_dictionary_merge(dico1, dico2);
    ck_assert_int_eq(mr_dictionary_count_word(dico1, "Lorem"), 3);
    ck_assert_int_eq(mr_dictionary_count_word(dico2, "Ipsum"), 1);

    mr_dictionary_delete(&dico1);
    mr_dictionary_delete(&dico2);
}
END_TEST


Suite *dictionary_suite(void) {
    Suite *suite = suite_create("Dictionary");
    TCase *tcase1 = tcase_create("Case Create");
    TCase *tcase2 = tcase_create("Case Delete");
    TCase *tcase3 = tcase_create("Case Put");
    TCase *tcase4 = tcase_create("Case Massive Put");
    TCase *tcase5 = tcase_create("Case Merge");

    tcase_add_test(tcase1, test_create);
    tcase_add_test(tcase2, test_delete);
    tcase_add_test(tcase3, test_put);
    tcase_add_test(tcase4, test_massive_put);
    tcase_add_test(tcase5, test_merge);

    suite_add_tcase(suite, tcase1);
    suite_add_tcase(suite, tcase2);
    suite_add_tcase(suite, tcase3);
    suite_add_tcase(suite, tcase4);
    suite_add_tcase(suite, tcase5);

    return suite;
}


int main(void) {
    int number_failed;
    Suite *suite = dictionary_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
