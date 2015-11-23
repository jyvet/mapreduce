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

#ifndef HEADER_MAPREDUCE_WORD_H
    #define HEADER_MAPREDUCE_WORD_H

    #include "common.h"
    #include "buffalloc.h"

    /**
     * @struct struct word_s
     * @brief  Structure containing information about word such as the spelling,
     *         its number of characters and its number of occurrences in a text.
     *         Words may be chained together by using a singly linked list.
     */
    typedef struct word_s {
        char*          name;       /**<  Pointer to the spelling of the word  */
        struct word_s* next;       /**<  Pointer a next word                  */
        unsigned int   count;      /**<  Word occurrences in a text           */
        unsigned int   length;     /**<  Number of characters in the word     */
        /* /!\ Keep the last element at the end of the structure it is used
               to avoid handling muitple malloc for the structure and the string
               containing the spelling of the word. */
    } Word;


    /* ============================== Prototypes ============================ */

    Word*   mr_word_create(const char*);
    Word*   mr_word_create_buff(const char *, Buffalloc*);
    void    mr_word_delete(Word**);
#endif
