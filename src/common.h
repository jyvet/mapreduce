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
        WS_SCHUNKS,         /* Wordstreamer type: scattered chunks   */
        WS_ICHUNKS,         /* Wordstreamer type: interleaved chunks */
        WS_IWORDS,          /* Wordstreamer type: interleaved words  */
        WS_NB               /* Number of Wordstreamers               */
    } ws_type;


    #define TYPE_PARALLEL                    0
    #define TYPE_SEQUENTIAL                  1

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
