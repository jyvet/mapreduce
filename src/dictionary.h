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

#ifndef HEADER_MAPREDUCE_DICTIONARY_H
    #define HEADER_MAPREDUCE_DICTIONARY_H

    #include "common.h"
    #include "word.h"
    #include "buffalloc.h"

    #define HASH_CHAR_SIZE 256
    #define HASH_CHARS_USED 2
    #define HASH_SIZE HASH_CHAR_SIZE*HASH_CHAR_SIZE

    /**
     * @struct bucket_s
     * @brief  Structure containing buckets for the hastable.
     */
    typedef struct bucket_s {
        Word *word;                  /**<  String containing the word         */
        #if MAPREDUCE_USE_BUFFALLOC
            Buffalloc *buffalloc;    /**<  Pointer to a buffer to alloc words */
        #endif
    } Bucket;


    /**
     * @struct dictionary_s
     * @brief  Structure containing information to manage dictionary.
     */
    typedef struct dictionary_s {
        unsigned int  hash_size;  /**<  Hash size (max value for index)       */
        Bucket*       hash_tab;   /**<  Array of buckets                      */
        bool          profiling;  /**<  Profiling mode                        */
        Timer         timer_put;  /**<  Timer for put func. [Profiling mode]  */
        Timer         timer_hash; /**<  Timer for hash func. [Profiling mode] */
    } Dictionary;


    /* ============================== Prototypes ============================ */

    Dictionary*    mr_dictionary_create(bool);
    void           mr_dictionary_delete(Dictionary**);

    void           mr_dictionary_put_word(Dictionary*, const char*);
    void           mr_dictionary_merge(Dictionary*, Dictionary*);
    unsigned int   mr_dictionary_count_word(Dictionary*, const char*);
    void           mr_dictionary_display(Dictionary*);
#endif
