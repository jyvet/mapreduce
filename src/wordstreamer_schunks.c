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
 * @file wordstreamer_schunks.c
 * @brief Streamer to retrieve words from a file. With multiple streams, file is
          accessed via multiple scattered chunks where data do not overlap.
 * @author Jean-Yves VET
 */

#include "wordstreamer_schunks.h"

Wordstreamer* _mr_wordstreamer_schunks_create(const char*, Filereader*,
                                              const int, const int, const bool);

/* ========================= Constructor / Destructor ======================= */

/**
 * Constructor for the first streamer.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @param   nb_streamers[in]  Total number of streamers
 * @param   profiling[in]     Activate the profiling mode
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* mr_wordstreamer_schunks_create_first(const char* file_path,
                                       const int nb_streamers, bool profiling) {

    return _mr_wordstreamer_schunks_create(file_path, NULL, 0,
                                                       nb_streamers, profiling);
}


/**
 * Constructor for each other streamer.
 *
 * @param   first[in]        String containing the path to the file to read
 * @param   streamer_id[in]  Id of the current wordstreamer
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* mr_wordstreamer_schunks_create_another(const Wordstreamer* first,
                                                        const int streamer_id) {

    return _mr_wordstreamer_schunks_create("", first->filereader, streamer_id,
                                         first->nb_streamers, first->profiling);
}


/**
 * Delete a Wordstreamer structure.
 *
 * @param   ws_ptr[in]   Pointer to the Wordstreamer structure
 */
void  mr_wordstreamer_schunks_delete(Wordstreamer* ws) {
    _mr_wordstreamer_common_delete(ws);
}


/* ============================= Private functions ========================== */

/**
 * Internal constructor for Wordstreamer_schunks.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @param   shared_map[in]    Pointer to area where the file was mmaped
 * @param   streamer_id[in]   Id of the current wordstreamer
 * @param   nb_streamers[in]  Total number of streamers
 * @param   profiling[in]     Activate the profiling mode
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* _mr_wordstreamer_schunks_create(const char* file_path,
                                Filereader *first_reader, const int streamer_id,
                                 const int nb_streamers, const bool profiling) {

    Wordstreamer *ws = _mr_wordstreamer_common_create(file_path, first_reader,
                                          streamer_id, nb_streamers, profiling);

    /* Set function pointers */
    ws->get = mr_wordstreamer_schunks_get;
    ws->delete = mr_wordstreamer_schunks_delete;
    ws->create_another = mr_wordstreamer_schunks_create_another;

    /* Compute offsets */
    Filereader *fr = ws->filereader;
    long long chunk_size = fr->file_size / nb_streamers;
    long long start_offset = chunk_size * streamer_id;
    long long stop_offset = start_offset + chunk_size -1;

    /* Add rest for last streamer */
    if (streamer_id == nb_streamers - 1) {
        stop_offset += fr->file_size % nb_streamers;
    }

    /* Initialize offsets for filereader */
    mr_filereader_set_offsets(fr, start_offset, stop_offset);

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
static inline void _mr_wordstreamer_schunks_remove_nonwords(Filereader *fr,
                           char *character_ptr, int *ret_ptr, int streamer_id) {

    int ret = *ret_ptr;
    char character = *character_ptr;

    /* Remove incomplete words */
    if (streamer_id && fr->offset - 1 == fr->start_offset) {
        while (!ispunct(character)
               && !isspace(character)
               && !ret) {
            ret = mr_filereader_get_byte(fr, character_ptr);
            character = *character_ptr;
        }
    }

    /* Remove extra spaces and punctuation char */
    while ((ispunct(character) || isspace(character)) && !ret) {
        ret = mr_filereader_get_byte(fr, character_ptr);
        character = *character_ptr;
    }

    /* Save return value */
    *ret_ptr = ret;
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
static inline void _mr_wordstreamer_schunks_retrieve_word(Filereader *fr,
             char *character_ptr, int *ret_ptr, int streamer_id, char *buffer) {

    int i=0, ret = *ret_ptr;
    char character = *character_ptr;

    /* Retrieve all characters */
    while(!ispunct(character) && !isspace(character) && ret >= 0) {
        buffer[i++] = character;
        ret = mr_filereader_get_byte(fr, character_ptr);
        character = *character_ptr;
    }

    /* Terminate string */
    buffer[i] = '\0';

    /* First char to lower case */
    buffer[0] = tolower(buffer[0]);

    /* Save return value */
    *ret_ptr = ret;
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
int mr_wordstreamer_schunks_get(Wordstreamer *ws, char *buffer) {
    int ret, streamer_id = ws->streamer_id;
    Filereader *fr = ws->filereader;
    char character;

    /* Return because end of stream already reached */
    if(ws->end) return 1;

    /* Get next byte */
    ret = mr_filereader_get_byte(fr, &character);

    /* Remove incomplete words, spaces and punctuation */
    _mr_wordstreamer_schunks_remove_nonwords(fr, &character, &ret, streamer_id);

    if (!ret) {
        /* Retrieve a complete word */
        _mr_wordstreamer_schunks_retrieve_word(fr, &character, &ret,
                                                           streamer_id, buffer);

        if (ret) ws->end = true;

        return 0;
    } else {
        /* Retrieve one more word (last word between this chunk and the next
           one) */
        if(ret == 1
           && !ispunct(character)
           && !isspace(character)) {

            _mr_wordstreamer_schunks_retrieve_word(fr, &character, &ret,
                                                           streamer_id, buffer);
            ws->end = true;

            return 0;
        } else {
            /* Return because end of stream reached */
            ws->end = true;

            return 1;
        }
    }
}
