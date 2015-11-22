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
 * @file main.c
 * @brief Mapreduce.
 * @author Jean-Yves VET
 */

#include "args.h"
#include "mapreduce.h"

int main(int argc, char **argv) {
    /* Allocate Arguments and Mapreduce structures */
    Arguments* args = mr_args_retrieve(argc, argv);
    Mapreduce* mr = mr_create(args);

    /* Map and reduce operations */
    mr_map(mr);
    mr_reduce(mr);

    /* Clean */
    mr_args_delete(&args);
    mr_delete(&mr);

    return 0;
}
