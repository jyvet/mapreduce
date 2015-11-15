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

#include "args.h"
#include <check.h>

START_TEST (test_create)
{
    char *argv[3] = {"", "file", "1"};
    Arguments *args = mr_args_create(3, argv);
    ck_assert_ptr_ne(args, NULL);

    mr_args_delete(&args);
}
END_TEST


START_TEST (test_delete)
{
    char *argv[3] = {"", "file", "1"};
    Arguments *args = mr_args_create(3, argv);

    mr_args_delete(&args);
    ck_assert_ptr_eq(args, NULL);
}
END_TEST


START_TEST (test_parse)
{
    char *argv[3] = {"", "file", "10"};
    Arguments *args = mr_args_create(3, argv);
    _parse_arguments(3, argv, args);

    ck_assert_int_eq(args->nb_threads, 10);
    ck_assert_str_eq(args->file_path, "file");

    mr_args_delete(&args);
}
END_TEST


Suite *args_suite(void) {
    Suite *suite = suite_create("Arguments");
    TCase *tcase1 = tcase_create("Create");
    TCase *tcase2 = tcase_create("Delete");
    TCase *tcase3 = tcase_create("Parse");

    tcase_add_test(tcase1, test_create);
    tcase_add_test(tcase2, test_delete);
    tcase_add_test(tcase3, test_parse);

    suite_add_tcase(suite, tcase1);
    suite_add_tcase(suite, tcase2);
    suite_add_tcase(suite, tcase3);

    return suite;
}


int main() {
    int number_failed;
    Suite *suite = args_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
