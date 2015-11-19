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

#include "buffalloc.h"
#include <check.h>

#define NB_TESTS 1000
#define MAX_MALLOC_SIZE MAPREDUCE_BUFFALLOC_CHUNK_SIZE

START_TEST (test_create)
{
    Buffalloc *ba = mr_buffalloc_create();

    ck_assert(ba != NULL);
    ck_assert(ba->chunk_size > 0);
    ck_assert_int_eq(ba->alloc_size, 0);
    ck_assert(ba->last_chunk == NULL);

    mr_buffalloc_delete(&ba);
}
END_TEST


START_TEST (test_delete)
{
    Buffalloc *ba = mr_buffalloc_create();
    ck_assert(ba != NULL);

    mr_buffalloc_malloc(ba, 64);

    mr_buffalloc_delete(&ba);
    ck_assert(ba == NULL);
}
END_TEST


START_TEST (test_massiv_malloc)
{
    int i;
    Buffalloc *ba = mr_buffalloc_create();
    long int alloc_size = 0;
    srand(1);

    for (i=0; i<NB_TESTS; i++) {
        int j, size = rand()%MAX_MALLOC_SIZE;

        char *alloc = mr_buffalloc_malloc(ba, size);
        ck_assert(alloc != NULL);

        for (j=0; j<size; j++) {
            alloc[j] = 1;
        }

        alloc_size += size;
        ck_assert(ba->alloc_size >= alloc_size);
    }
    mr_buffalloc_delete(&ba);
}
END_TEST


Suite *word_suite(void) {
    Suite *suite = suite_create("Buffalloc");
    TCase *tcase1 = tcase_create("Case Create");
    TCase *tcase2 = tcase_create("Case Delete");
    TCase *tcase3 = tcase_create("Case MAssiv Malloc");

    tcase_add_test(tcase1, test_create);
    tcase_add_test(tcase2, test_delete);
    tcase_add_test(tcase3, test_massiv_malloc);

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
