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
 * @file mapreduce_sequential.c
 * @brief Mapreduce sequential implementation.
 * @author Jean-Yves VET
 */

#include "mapreduce.h"
#include "mapreduce_sequential.h"

/* ========================= Constructor / Destructor ======================= */

/**
 * Constructor for Mapreduce sequential.
 *
 * @param  file_path[in]    String containg the path to the file we want to read
 * @param  quiet[in]        Activate the quiet mode (no output)
 * @param  profiling[in]    Activate the profiling mode
 * @return  A Mapreduce structure
 */
Mapreduce* mr_sequential_create(const char *file_path, const int ws_type,
                                       const bool quiet, const bool profiling) {
    Mapreduce *mr = mr_common_create(file_path, 1, TYPE_SEQUENTIAL,
                                                              quiet, profiling);

    mr->map = mr_sequential_map;
    mr->reduce = mr_sequential_reduce;
    mr->delete = mr_sequential_delete;

    Mapreduce_sequential_ext *ext = malloc(sizeof(Mapreduce_sequential_ext));
    ext->wordstreamer = mr_wordstreamer_create_first(file_path, 1,
                                                            ws_type, profiling);
    ext->dictionary = mr_dictionary_create(profiling);

    mr->ext = ext;

    return mr;
}


/**
 * Delete Mapreduce and associated structures.
 *
 * @param   mr[in]     Pointer to a Mapreduce structure
 */
void mr_sequential_delete(Mapreduce *mr) {
    if (mr != NULL) {
        Mapreduce_sequential_ext *ext = (Mapreduce_sequential_ext *) mr->ext;

        mr_wordstreamer_delete(&ext->wordstreamer);
        mr_dictionary_delete(&ext->dictionary);

        free(ext);
    }
}


/* ============================= Public functions =========================== */

/**
 * Map operation.
 *
 * @param   mr[inout]     Pointer to a Mapreduce structure
 */
void mr_sequential_map(Mapreduce *mr) {
    assert(mr != NULL);

    Mapreduce_sequential_ext *ext = (Mapreduce_sequential_ext *) mr->ext;
    Dictionary *dico = ext->dictionary;
    Wordstreamer *ws = ext->wordstreamer;

    char word[MAPREDUCE_MAX_WORD_SIZE];

    while (!mr_wordstreamer_get(ws, word)) {
        mr_dictionary_put_word(dico, word);
    }
}


/**
 * Map operation : Aggregate results, and display them, sorted by word.
 *
 * @param   mr[inout]     Pointer to a Mapreduce structure
 */
void mr_sequential_reduce(Mapreduce *mr) {
    Mapreduce_sequential_ext *ext = (Mapreduce_sequential_ext *) mr->ext;
    Dictionary *dico = ext->dictionary;

    if (!mr->quiet) mr_dictionary_display(dico);
}
