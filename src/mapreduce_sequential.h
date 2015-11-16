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

#ifndef HEADER_MAPREDUCE_SEQUENTIAL_H
    #define HEADER_MAPREDUCE_SEQUENTIAL_H

    #include "common.h"
    #include "mapreduce.h"
    #include "dictionary.h"
    #include "wordstreamer.h"

    /**
     * @struct mapreduce_sequential_ext_s
     * @brief  Structure containing extra data for mapreduce_sequential
     */
    typedef struct mapreduce_sequential_ext_s {
        Dictionary*     dictionary;    /**<  Pointer to a sorted hashtab      */
        Wordstreamer*   wordstreamer;  /**<  Pointer to a streamer of words   */
    } Mapreduce_sequential_ext;


    /* ============================== Prototypes ============================ */

    Mapreduce*  mr_sequential_create(const char*, const int, const bool,
                                                                    const bool);
    void        mr_sequential_delete(Mapreduce*);

    void        mr_sequential_map(Mapreduce*);
    void        mr_sequential_reduce(Mapreduce*);
#endif
