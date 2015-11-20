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
 * @file wordstreamer_interleave.c
 * @brief Streamer to retrieve words from a file. With multiple streams, file is
          accessed in words interleaved fashion.
 * @author Jean-Yves VET
 */

#include "wordstreamer_iwords.h"

Wordstreamer* _mr_wordstreamer_iwords_create(const char*, char *, const int,
                                                         const int, const bool);

/* ========================= Constructor / Destructor ======================= */

/**
 * Constructor for the first streamer.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @param   nb_streamers[in]  Total number of streamers
 * @param   profiling[in]     Activate the profiling mode
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* mr_wordstreamer_iwords_create_first(const char* file_path,
                                       const int nb_streamers, bool profiling) {

    return _mr_wordstreamer_iwords_create(file_path, NULL, 0,
                                                       nb_streamers, profiling);
}


/**
 * Constructor for each other streamer.
 *
 * @param   first[in]        String containing the path to the file to read
 * @param   streamer_id[in]  Id of the current wordstreamer
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* mr_wordstreamer_iwords_create_another(const Wordstreamer* first,
                                                        const int streamer_id) {

    return _mr_wordstreamer_iwords_create(first->file_path, first->shared_map,
                            streamer_id, first->nb_streamers, first->profiling);
}


/**
 * Delete a Wordstreamer structure.
 *
 * @param   ws_ptr[in]   Pointer to the Wordstreamer structure
 */
void  mr_wordstreamer_iwords_delete(Wordstreamer* ws) {
    _mr_wordstreamer_common_delete(ws);
}


/* ============================= Private functions ========================== */

/**
 * Internal constructor for Wordstreamer_interleave.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @param   shared_map[in]    Pointer to area where the file was mmaped
 * @param   streamer_id[in]   Id of the current wordstreamer
 * @param   nb_streamers[in]  Total number of streamers
 * @param   profiling[in]     Activate the profiling mode
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* _mr_wordstreamer_iwords_create(
          const char* file_path, char *shared_map,
          const int streamer_id, const int nb_streamers, const bool profiling) {

    Wordstreamer *ws = _mr_wordstreamer_common_create(file_path, shared_map,
                                          streamer_id, nb_streamers, profiling);

    /* Set function pointers */
    ws->get = mr_wordstreamer_iwords_get;
    ws->delete = mr_wordstreamer_iwords_delete;
    ws->create_another = mr_wordstreamer_iwords_create_another;

    /* Set offsets and chunk elements */
    ws->start_offset = 0;
    ws->offset = 0;
    ws->chunk_size = ws->file_size;
    ws->stop_offset = ws->file_size - 1;

    return ws;
}


/**
 * Remove non-word characters to stream.
 *
 * @param   ws[inout]          Pointer to the Wordstreamer structure
 * @param   file_size[in]      Size in Bytes of the input file
 * @param   start_offset[in]   Start offset of the current chunk in Bytes
 * @param   streamer_id[in]    Id of the current wordstreamer
 * @param   shared_map[in]     Pointer to area where the file was mmaped
 */
static inline void _mr_wordstreamer_iwords_remove_nonwords(Wordstreamer *ws,
                                                             char *shared_map) {
    long int offset = ws->offset;
    char character = shared_map[offset];

    /* Remove extra spaces and punctuation char */
    while (ispunct(character) || isspace(character)) {
        character = shared_map[++offset];
    }

    ws->offset = offset;
}


/**
 * Retrieve a word.
 *
 * @param   ws[inout]          Pointer to the Wordstreamer structure
 * @param   file_size[in]      Size in Bytes of the input file
 * @param   buffer[out]        Buffer to hold the retrieved word
 * @param   streamer_id[in]    Id of the current wordstreamer
 * @param   shared_map[in]     Pointer to area where the file was mmaped
 */
static inline void _mr_wordstreamer_iwords_retrieve_word(Wordstreamer *ws,
                           long int file_size, char *buffer, char *shared_map) {
    long int offset = ws->offset;
    char character = shared_map[offset];
    int i=0;

    /* Retrieve all characters */
    while(!ispunct(character) && !isspace(character) && offset < file_size) {
        buffer[i++] = character;
        character = shared_map[++offset];
    }

    /* Terminate string */
    buffer[i] = '\0';

    /* First char to lower case */
    buffer[0] = tolower(buffer[0]);

    ws->offset = offset;
}


/* ============================= Public functions =========================== */

/**
 * Get next word from a wordstreamer. Return 1 if end of stream reached.
 *
 * @param   ws[in]               Pointer to the Wordstreamer structure
 * @param   shared_map[out]      Buffer to hold the retrieved word
 * @return  0 if a word was copied into the buffer or 1 if the end of the stream
 *          was reached
 */
int mr_wordstreamer_iwords_get(Wordstreamer *ws, char *buffer) {
    char word[MAPREDUCE_MAX_WORD_SIZE];
    int nb_streamers = ws->nb_streamers, streamer_id = ws->streamer_id;
    char* map = ws->shared_map;
    long int offset, file_size = ws->file_size, stop_offset = ws->stop_offset;
    int word_count = 0;

    /* Return because end of stream already reached */
    if(ws->end) return 1;

    /* Find the next word associated with the streamer id */
    while (word_count < nb_streamers && ws->offset <= stop_offset) {
        /* Remove incomplete words, spaces and punctuation */
        _mr_wordstreamer_iwords_remove_nonwords(ws, map);
        offset = ws->offset;

        if (offset < stop_offset) {
            /* Retrieve a complete word */
            _mr_wordstreamer_iwords_retrieve_word(ws, file_size, word, map);

            if (word_count == streamer_id) {
                strcpy(buffer, word);
            }

            word_count++;
        } else {
            ws->end = true;
            break;
        }
    }

    return (word_count <= streamer_id);
}
