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

#ifndef HEADER_MAPREDUCE_CONFIG_H
    #define HEADER_MAPREDUCE_CONFIG_H

    #include "common.h"

    #define MAPREDUCE_DEFAULT_TYPE            MR_PARALLEL
    #define MAPREDUCE_WS_DEFAULT_TYPE         WS_SCHUNKS
    #define MAPREDUCE_WS_DEFAULT_CHUNK_SIZE   16
    #define MAPREDUCE_DEFAULT_USECOLORS       1
    #define MAPREDUCE_DEFAULT_QUIET           0
    #define MAPREDUCE_DEFAULT_PROFILING       0
    #define MAPREDUCE_MIN_THREADS             1
    #define MAPREDUCE_MAX_THREADS             64
    #define MAPREDUCE_USE_BUFFALLOC           1
    #define MAPREDUCE_BUFFALLOC_CHUNK_SIZE    1024
    #define MAPREDUCE_MAX_WORD_SIZE           (MAPREDUCE_BUFFALLOC_CHUNK_SIZE-1)
    #define MAPREDUCE_VERSION                 "0.4"
    #define MAPREDUCE_CONTACT                 "contact [at] jean-yves [dot] vet"
#endif
