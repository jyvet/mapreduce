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
#include "filereader_read.h"

#define MAX_READERS 11

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

    Filereader *fr = mr_filereader_read_create_first(filename, 4096);
    ck_assert(fr != NULL);

    mr_filereader_read_delete(fr);

    /* Delete testfile */
    remove(filename);
}
END_TEST


START_TEST (test_multiple_readers)
{
    int i, j;
    char buffer_byte;
    char buffer[4096];

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

    /* Check several combinations */
    for (i=1; i<=MAX_READERS; i++) {
        int buffer_index = 0;
        Filereader *fr[MAX_READERS];

        /* Create first reader */
        fr[0] = mr_filereader_read_create_first(filename, 4096);
        ck_assert(fr[0] != NULL);

        /* Create other readers */
        for (j=1; j<i; j++) {
            fr[j] = mr_filereader_read_create_another(fr[0]);
            ck_assert(fr[j] != NULL);
        }

        size_t file_size = fr[0]->file_size;
        long long chunk_size = file_size/i;
        long long chunk_rest = file_size%i;

        /* Initialize offsets */
        for (j=0; j<i; j++) {
            long long start_offset = j*chunk_size;
            long long end_offset = (j+1)*chunk_size-1;

            if (j==i-1) {
                end_offset += chunk_rest;
            }

            mr_filereader_read_set_offsets(fr[j], start_offset, end_offset);
        }

        /* Retrieve bytes */
        for (j=0; j<i; j++) {
            while(!mr_filereader_read_get_byte(fr[j], &buffer_byte)) {
                buffer[buffer_index++] = buffer_byte;
            }
        }
        buffer[buffer_index] = '\0';

        /* Check some occurences */
        ck_assert_str_eq(buffer, content);


        /* Delete all readers */
        for (j=0; j<i; j++) {
            mr_filereader_read_delete(fr[j]);
        }
    }

    remove(filename);
}
END_TEST


Suite *filereader_suite(void) {
    Suite *suite = suite_create("Filereader Read");
    TCase *tcase1 = tcase_create("Case Create Delete");
    TCase *tcase2 = tcase_create("Case Multiple Readers");

    tcase_add_test(tcase1, test_create_delete);
    tcase_add_test(tcase2, test_multiple_readers);

    suite_add_tcase(suite, tcase1);
    suite_add_tcase(suite, tcase2);

    return suite;
}


int main(void) {
    int number_failed;
    Suite *suite = filereader_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
