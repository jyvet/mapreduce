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
 * @file dictionary.c
 * @brief A dictionary (hash table containing words where they are
 *        alphabetically sorted).
 * @author Jean-Yves VET
 */

#include "dictionary.h"

#define MAPREDUCE_MAX_WORD_SIZE 128

/* ========================= Constructor / Destructor ======================= */

/**
 * Create a Dictionary based on a Hashtable.
 *
 * @param   profiling[in]  Activate the profiling mode
 * @return  Pointer to the new Dictionary structure
 */
Dictionary *mr_dictionary_create(bool profiling) {
    Dictionary *dico = malloc(sizeof(Dictionary));
    assert(dico != NULL);

    /* Initilaize variables for profiling */
    dico->profiling = profiling;
    _timer_init(&dico->timer_put, dico->profiling);
    _timer_init(&dico->timer_hash, dico->profiling);

    dico->hash_size = HASH_SIZE;

    dico->hash_tab = malloc(dico->hash_size*sizeof(Bucket));
    assert(dico->hash_tab != NULL);

    /* Set to null first Word pointers in all buckets */
    for (int i=0; i<dico->hash_size; i++){
        Bucket *b = &dico->hash_tab[i];
        b->word = NULL;
    }

    return dico;
}


/**
 * Delete Dictionary with all associated words and set structure pointer
 * to NULL.
 *
 * @param   ptr_dico[int]   Pointer to pointer of Dictionary structure
 */
void mr_dictionary_delete(Dictionary **ptr_dico) {
    assert(ptr_dico != NULL);
    Dictionary *dico = *ptr_dico;

    /* Display time info if requiered [Profiling mode] */
    _timer_print(&dico->timer_hash, "[Dictionary] hash function");
    _timer_print(&dico->timer_put, "[Dictionary] put");

    if (dico != NULL) {
        unsigned int hash_size = dico->hash_size;

        /* Delete all words */
        for (int i=0; i<hash_size; i++){
            Bucket *bucket = &dico->hash_tab[i];

            Word *word = bucket->word;
            while(word != NULL) {
                Word *word_ptr = word;
                word = word->next;
                mr_word_delete(&word_ptr);
            }
        }

        /* Delete dictionary structure */
        free(dico->hash_tab);
        free(dico);
    }

    *ptr_dico = NULL;
}


/* ============================= Private functions ========================== */

/**
 * Compute hash for a given word (hash based on the first 2 characters).
 *
 * @param   dico[in]   Pointer to a Dictionary structure
 * @param   word[in]   String containing the word
 * @return  hash code
 */
unsigned int _mr_dictionary_hash(Dictionary *dico, const char *word) {
    _timer_start(&dico->timer_hash);
    unsigned int word_length = strlen(word);
    unsigned int hash;

    if (word_length > 1) {
        hash = (unsigned char)word[0]*HASH_CHAR_SIZE
                                 + (unsigned char)word[1];
    } else {
        hash = (unsigned char)word[0]*HASH_CHAR_SIZE;
    }

    _timer_stop(&dico->timer_hash);

    return hash;
}


/**
 * Look for a given word in the dictionary and return a pointer to the Word
 * structure if found.
 *
 * @param   dico[in]   Pointer to a Dictionary structure
 * @param   word[in]   String containing the word
 * @return  Pointer to the Word structure or NULL if not found
 */
Word *_mr_dictionary_lookup_word(Dictionary *dico, const char *word) {
    unsigned int hash = _mr_dictionary_hash(dico, word);

    Bucket *bucket = &dico->hash_tab[hash];
    Word *dico_word = bucket->word;

    while(dico_word != NULL && strcmp(word, dico_word->name)) {
        dico_word = dico_word->next;
    }

    return dico_word;
}


/**
 * Convert a character in a word to a number for comparison.
 *
 * @param   word[in]     String containing the word
 * @param   length[in]   Characters in the word
 * @param   pos[in]      Position of the character to convert to integer
 * @return  Integer (positive value of the char or -1 if out of range)
 */
static inline int _mr_dictionary_char_value(const char *word,
                                        unsigned int length, unsigned int pos) {
    if (pos < length) {
        return (int)((unsigned char) word[pos]);
    } else {
        return -1;
    }
}


/**
 * Add number of occurrences to a Word structure in a Dictionary. Add the word
 * if it does not exist.
 *
 * @param   dico[inout]   Pointer to a Dictionary structure
 * @param   word[in]      String containing the word
 * @param   count[in]     Occurrences to add
 */
void _mr_dictionary_add_word_count(Dictionary *dico, const char *word,
                                                                    int count) {
    unsigned int word_length = strlen(word), pos = HASH_CHARS_USED,
                 hash = _mr_dictionary_hash(dico, word);
    Bucket *bucket = &dico->hash_tab[hash];
    Word **ptr = &bucket->word;
    Word *dico_word = bucket->word;

    while (pos < MAPREDUCE_MAX_WORD_SIZE) {
        /* Empty bucket */
        if (dico_word == NULL) {
            *ptr = mr_word_create(word);
            (*ptr)->count = count;
            break;
        }

        char *dest_word = dico_word->name;
        int src = _mr_dictionary_char_value(word, word_length, pos);
        int dest = _mr_dictionary_char_value(dest_word, dico_word->length, pos);

        if (src < dest) {
            Word *new_word = mr_word_create(word);
            new_word->count = count;
            *ptr = new_word;
            new_word->next = dico_word;
            break;
        } else if (src > dest) {
            ptr = &dico_word->next;
            dico_word = dico_word->next;
            pos = HASH_CHARS_USED;
        } else {
            if (!strcmp(word, dest_word)) {
                dico_word->count += count;
                break;
            } else {
                pos++;
            }
        }
    }
}


/* ============================= Public functions =========================== */

/**
 * Merge two dictionaries.
 *
 * @param   first[inout]  Pointer to the first dictionary (outcome stored here)
 * @param   second[in]    Pointer to the second dictionary
 */
void mr_dictionary_merge(Dictionary* first, Dictionary* second) {
    assert(second->hash_size == first->hash_size);
    unsigned int hash_size = second->hash_size;

    for (int i=0; i<hash_size; i++){
        Bucket *bucket = &second->hash_tab[i];
        Word *word = bucket->word;
        while(word != NULL) {
            _mr_dictionary_add_word_count(first, word->name, word->count);
            word = word->next;
        }
    }
}


/**
 * Put new occurrence in the dictionary.
 *
 * @param   dico[inout]   Pointer to the dictionary
 * @param   word[in]      String containing the word
 */
void mr_dictionary_put_word(Dictionary *dico, const char *word) {
    _timer_start(&dico->timer_put);
    _mr_dictionary_add_word_count(dico, word, 1);
    _timer_stop(&dico->timer_put);
}


/**
 * Retrieve occurrences of a word.
 *
 * @param   dico[in]     Pointer to the dictionary
 * @param   str[in]      String containing the spelling of the word
 * @return  Count (0 if word not found)
 */
unsigned int mr_dictionary_count_word(Dictionary *dico, const char *str) {
    Word *word = _mr_dictionary_lookup_word(dico, str);
    unsigned int count = (word != NULL) ? word->count : 0;

    return count;
}


/**
 * Display all occurrences. Words are already alphabetically sorted.
 *
 * @param    dico[in]     Pointer to the dictionary
 */
void mr_dictionary_display(Dictionary *dico) {
    unsigned int hash_size = dico->hash_size;

    for (int i=0; i<hash_size; i++){
        Bucket *bucket = &dico->hash_tab[i];
        Word *word = bucket->word;
        
        while(word != NULL) {
            printf("%s=%d\n", word->name, word->count);
            word = word->next;
        }
    }
}
