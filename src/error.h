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

#ifndef HEADER_MAPREDUCE_ERROR_H
    #define HEADER_MAPREDUCE_ERROR_H

    #include "common.h"

    typedef enum {
        ERR_SUCCESS,            /* No error.                     */
        ERR_MINTHREADS,         /* Not enough threads            */
        ERR_MAXTHREADS,         /* Too many threads              */
        ERR_FILEACCESS,         /* File cannot be accessed       */
        ERR_MEMALLOC,           /* Allocation error.             */
        ERR_LAST                /* Number of errors.             */
    } err_code;


    static const char *t[] = {
        "Success",
        "You should start the program with more threads",
        "You should start the program with fewer threads (maximum reached)",
        "File does not exist or cannot be accessed in read mode",
        "Allocation error",
    };


    /**
     * Transform an error code into a string and exit with error code.
     *
     * @param   error[in]      Error code
     */
    static inline void *mr_error(err_code error) {
        #if MAPREDUCE_DEFAULT_USECOLORS
            fprintf(stderr, "\e[1;91mError: \e[2;91m%s\e[0m\n",
                    error<ERR_LAST ? (char *)t[error] : NULL);
        #else
            fprintf(stderr, "Error: %s\n",
                    error<ERR_LAST ? (char *)t[error] : NULL);
        #endif
        exit(error);
    }

#endif
