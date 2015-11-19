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

#include "word.h"
#include <check.h>
#include <time.h>
#include <stdlib.h>

#define NB_TESTS 10000
#define MAX_CHAR 15

START_TEST (test_create)
{
    Word *word = mr_word_create("test");

    ck_assert(word != NULL);
    ck_assert_str_eq(word->name, "test");
    ck_assert_int_eq(word->length, 4);
    ck_assert_int_eq(word->count, 1);
    ck_assert(word->next == NULL);

    mr_word_delete(&word);
}
END_TEST


START_TEST (test_delete)
{
    Word *word = mr_word_create("test");
    mr_word_delete(&word);
    ck_assert(word == NULL);
}
END_TEST


START_TEST (test_massiv_create)
{
    srand(1);
    char buffer[MAX_CHAR+1];

    for (int i=0; i<NB_TESTS; i++) {
        int nb_char = rand()%MAX_CHAR + 1;
        int j;

        /* Generate a random word */
        for (j=0; j<nb_char; j++) {
            unsigned char character = rand()%70+50;
            buffer[j] = (char)character;
        }
        buffer[j] = '\0';

        /* Add word */
        Word *word = mr_word_create(buffer);

        ck_assert(word == NULL);
        ck_assert_int_eq(word->length, nb_char);
        ck_assert_str_eq(word->name, buffer);

        mr_word_delete(&word);
    }
}
END_TEST


Suite *word_suite(void) {
    Suite *suite = suite_create("Word");
    TCase *tcase1 = tcase_create("Case Create");
    TCase *tcase2 = tcase_create("Case Delete");
    TCase *tcase3 = tcase_create("Case Massiv Create");

    tcase_add_test(tcase1, test_create);
    tcase_add_test(tcase2, test_delete);
    tcase_add_test(tcase3, test_massiv_create);

    suite_add_tcase(suite, tcase1);
    suite_add_tcase(suite, tcase2);
    suite_add_tcase(suite, tcase3);

    return suite;
}


int main(void) {
    int number_failed;
    Suite *suite = word_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
