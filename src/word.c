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
 * @file word.c
 * @brief Manage Word structure.
 * @author Jean-Yves VET
 */

#include "word.h"

void _mr_word_init(Word*, const char *, const int);

/* ========================= Constructor / Destructor ======================= */

/**
 * Create a Word structure based on the provided string.
 *
 * @param   src_word[in]   String containing the spelling of the word
 * @return  Pointer to the new Word structure
 */
Word* mr_word_create(const char *src_word) {
    assert(src_word != NULL);
    int length = strlen(src_word);

    /* Allocate at one the Word structure and the string size */
    Word *word = malloc(sizeof(Word)+length+1);

    _mr_word_init(word, src_word, length);

    return word;
}


/**
 * Create a Word structure based on the provided string. It uses a buffer to
 * allow multiple words to be stored in consecutive areas in memory.
 *
 * @param   src_word[in]   String containing the spelling of the word
 * @param   ba[inout]      Pointer to a buffer structure to manage allocation
 * @return  Pointer to the new Word structure
 */
Word* mr_word_create_buff(const char *src_word, Buffalloc *ba) {
    assert(src_word != NULL);
    int length = strlen(src_word);

    /* Allocate at one the Word structure and the string size */
    Word *word = mr_buffalloc_malloc(ba, sizeof(Word)+length+1);

    _mr_word_init(word, src_word, length);

    return word;
}



/**
 * Delete Word structure and set pointer to NULL.
 *
 * @param   word_ptr[inout]     Pointer to pointer of Word structure
 */
void mr_word_delete(Word **word_ptr) {
    assert(word_ptr != NULL);
    Word *word = *word_ptr;

    if(word != NULL) free(word);

    *word_ptr = NULL;
}


/* ============================= Private functions ========================== */

/**
 * Initialize a Word structure.
 *
 * @param   word[inout]     Pointer to the Word structure to initialize
 * @param   src_word[in]    String containing the spelling of the word
 * @param   length[in]      Number of characters in the word
 */
void _mr_word_init(Word* word, const char *src_word, const int length) {
    /* Map the pointer of the string to the last element of the structure */
    word->name = (char*)&word->_name;

    /* Copy the word spelling in the structure */
    strcpy(word->name, src_word);

    /* Initialize other elements */
    word->count = 1;
    word->next = NULL;
    word->length = length;
}
