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

/**
 * @file tools.c
 * @brief Tools.
 * @author Jean-Yves VET
 */

#include "tools.h"
#include <sys/stat.h>

/* ============================= Public functions =========================== */

/**
 * Get size of a file in Bytes.
 *
 * @param   file_path[in]   String containing the path to the file
 * @return  Size in Bytes
 */
long int mr_tools_fsize(const char *file_path) {
    struct stat st;

    if (stat(file_path, &st) == 0)
        return st.st_size;

    return -1;
}


/**
 * Count number of words in a file.
 *
 * @param   file_path[in]   String containing the path to the file
 * @return  Number of words
 */
long int mr_tools_wc(const char *file_path) {
    long int nb_words = 0;
    FILE *fp = fopen(file_path, "r");
    assert(fp != NULL);
    char character = getc(fp);

    while (character != EOF) {
        /* Remove extra spaces and punctuation char */
        while (ispunct(character) || isspace(character)) {
            character = getc(fp);
        }

        /* Retrieve a complete word */
        while(character != EOF && !ispunct(character) && !isspace(character)) {
            character = getc(fp);
        }

        nb_words++;
    }

    fclose(fp);

    return nb_words;
}
