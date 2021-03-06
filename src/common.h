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

#ifndef HEADER_MAPREDUCE_COMMON_H
    #define HEADER_MAPREDUCE_COMMON_H

    typedef enum {
        FR_MMAP,             /* Filereader type: mmap                */
        FR_READ,             /* Filereader type: read with buffer    */
        FR_NB                /* Number of Filereader types           */
    } fr_type;

    typedef enum {
        WS_SCHUNKS,         /* Wordstreamer type: scattered chunks   */
        WS_IWORDS,          /* Wordstreamer type: interleaved words  */
        WS_NB               /* Number of Wordstreamer types          */
    } ws_type;

    typedef enum {
        MR_PARALLEL,         /* Mapreduce type: parallel (pthreads)   */
        MR_SEQUENTIAL,       /* Mapreduce type: sequential            */
        MR_NB                /* Number of Mapreduce types             */
    } mr_type;

    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <assert.h>
    #include <unistd.h>
    #include <ctype.h>
    #include <stdbool.h>

    #include "config.h"
    #include "error.h"
    #include "timer.h"
#endif
