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
 * @file wordstreamer_scatter.c
 * @brief Streamer to retrieve words from a file. With multiple streams, file is
          accessed via multiple chunks where data do not overlap.
 * @author Jean-Yves VET
 */

#include "wordstreamer_scatter.h"

Wordstreamer* _mr_wordstreamer_scatter_create(const char*, char *, const int,
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
Wordstreamer* mr_wordstreamer_scatter_create_first(const char* file_path,
                                       const int nb_streamers, bool profiling) {
    Wordstreamer* ws = _mr_wordstreamer_scatter_create(file_path, NULL, 0,
                                                       nb_streamers, profiling);

    ws->chunk_size = ws->file_size/nb_streamers;
    ws->start_offset = 0;
    ws->offset = ws->start_offset;
    ws->stop_offset = ws->start_offset+ws->chunk_size;

    return ws;
}


/**
 * Constructor for each other streamer.
 *
 * @param   first[in]        String containing the path to the file to read
 * @param   streamer_id[in]  Total number of streamers
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* mr_wordstreamer_scatter_create_another(Wordstreamer* first,
                                                        const int streamer_id) {

    Wordstreamer* ws = _mr_wordstreamer_scatter_create("", first->shared_map,
                            streamer_id, first->nb_streamers, first->profiling);

    unsigned int file_size = first->file_size;
    unsigned int nb_streamers = first->nb_streamers;

    ws->file_size = file_size;

    ws->chunk_size = file_size/nb_streamers;
    ws->start_offset = ws->chunk_size*streamer_id;
    ws->offset = ws->start_offset;
    if (streamer_id == nb_streamers -1) {
        ws->chunk_size += ws->file_size%nb_streamers;
    }
    ws->stop_offset = ws->start_offset+ws->chunk_size;

    return ws;
}


/**
 * Delete a Wordstreamer structure.
 *
 * @param   ws_ptr[in]   Pointer to the Wordstreamer structure
 */
void  mr_wordstreamer_scatter_delete(Wordstreamer* ws) {
    _mr_wordstreamer_common_delete(ws);
}


/* ============================= Private functions ========================== */

/**
 * Internal constructor for Wordstreamer_scatter.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @param   shared_map[in]    Pointer to area where the file was mmaped
 * @param   streamer_id[in]   Id of the current wordstreamer
 * @param   nb_streamers[in]  Total number of streamers
 * @param   profiling[in]     Activate the profiling mode
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* _mr_wordstreamer_scatter_create(
          const char* file_path, char *shared_map,
          const int streamer_id, const int nb_streamers, const bool profiling) {

    Wordstreamer *ws =  _mr_wordstreamer_common_create(file_path, shared_map,
                                          streamer_id, nb_streamers, profiling);

    ws->get = mr_wordstreamer_scatter_get;
    ws->delete = mr_wordstreamer_scatter_delete;

    ws->type = TYPE_WORDSTREAMER_SCATTER;

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
static inline void _mr_wordstreamer_remove_nonwords(Wordstreamer *ws,
                                    long int file_size, long int start_offset,
                                            int streamer_id, char *shared_map) {
    long int offset = ws->offset;
    char character = shared_map[offset];

    /* Remove incomplete words */
    if (streamer_id && offset == start_offset) {
        while (!ispunct(character)
               && !isspace(character) && offset < file_size) {
            character = shared_map[++offset];
        }
    }

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
static inline void _mr_wordstreamer_retrieve_word(Wordstreamer *ws,
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
int mr_wordstreamer_scatter_get(Wordstreamer *ws, char *buffer) {
    _timer_start(&ws->timer_get);
    int streamer_id = ws->streamer_id;
    char* map = ws->shared_map;
    long int offset, file_size = ws->file_size, start_offset = ws->start_offset;
    long int stop_offset = ws->stop_offset;

    /* Return because end of stream already reached */
    if(ws->end) return 1;

    /* Remove incomplete words, spaces and punctuation */
    _mr_wordstreamer_remove_nonwords(ws, file_size, start_offset,
                                                             streamer_id, map);

    offset = ws->offset;

    if (offset < stop_offset) {
        /* Retrieve a complete word */
        _mr_wordstreamer_retrieve_word(ws, file_size, buffer, map);

        if (ws->offset >= stop_offset) ws->end = true;
        _timer_stop(&ws->timer_get);

        return 0;
    } else {
        char character = map[offset];

        /* Retrieve one more word (last word on two chunks) */
        if(!ispunct(character) && !isspace(character) && offset < file_size) {

            _mr_wordstreamer_retrieve_word(ws, file_size, buffer, map);
            ws->end = true;
            _timer_stop(&ws->timer_get);

            return 0;
        } else {
            /* Return because end of stream reached */
            ws->end = true;
            _timer_stop(&ws->timer_get);

            return 1;
        }
    }
}
