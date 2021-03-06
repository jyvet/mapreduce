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
 * @file wordstreamer_iwords.c
 * @brief Streamer to retrieve words from a file. With multiple streams, file is
          accessed in words interleaved fashion.
 * @author Jean-Yves VET
 */

#include "wordstreamer_iwords.h"
#include "filereader_read.h"

Wordstreamer* _mr_wordstreamer_iwords_create(const char*, const fr_type,
             Filereader*, const unsigned int, const int, const int, const bool);

/* ========================= Constructor / Destructor ======================= */

/**
 * Constructor for the first streamer.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @param   nb_streamers[in]  Total number of streamers
 * @param   reader_type[in]   Type of filereader to use (see common.h)
 * @param   read_buffer_size[in] Size in bytes of the read buffer
 * @param   profiling[in]     Activate the profiling mode
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* mr_wordstreamer_iwords_create_first(const char* file_path,
                          const int nb_streamers, const fr_type reader_type,
                          const unsigned int read_buffer_size, bool profiling) {

    return _mr_wordstreamer_iwords_create(file_path, reader_type, NULL,
                                  read_buffer_size, 0, nb_streamers, profiling);
}


/**
 * Constructor for each other streamer.
 *
 * @param   first[in]        Pointer to the first Wordstreamer structure
 * @param   streamer_id[in]  Id of the current wordstreamer
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* mr_wordstreamer_iwords_create_another(const Wordstreamer* first,
                                                        const int streamer_id) {
    unsigned int read_buffer_size = 0;
    assert(first != NULL);
    Filereader *fr = first->filereader;

    if (fr->type == FR_READ) {
        Filereader_read *ext = fr->ext;
        assert(ext);

        read_buffer_size = ext->buffer_size;
    }

    return _mr_wordstreamer_iwords_create("", first->reader_type,
                            first->filereader, read_buffer_size,
                            streamer_id, first->nb_streamers, first->profiling);
}


/**
 * Delete a Wordstreamer structure.
 *
 * @param   ws[in]   Pointer to the Wordstreamer structure
 */
void  mr_wordstreamer_iwords_delete(Wordstreamer* ws) {
    _mr_wordstreamer_common_delete(ws);
}


/* ============================= Private functions ========================== */

/**
 * Internal constructor for Wordstreamer_iwords.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @param   reader_type[in]   Type of filereader to use (see common.h)
 * @param   first_reader[in]  Pointer to the first reader
 * @param   read_buffer_size[in] Size in bytes of the read buffer
 * @param   streamer_id[in]   Id of the current wordstreamer
 * @param   nb_streamers[in]  Total number of streamers
 * @param   profiling[in]     Activate the profiling mode
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* _mr_wordstreamer_iwords_create(const char* file_path,
                    const fr_type reader_type, Filereader* first_reader,
                    const unsigned int read_buffer_size, const int streamer_id,
                    const int nb_streamers, const bool profiling) {

    Wordstreamer *ws = _mr_wordstreamer_common_create(file_path, reader_type,
                                          first_reader, read_buffer_size,
                                          streamer_id, nb_streamers, profiling);

    /* Set function pointers */
    ws->get = mr_wordstreamer_iwords_get;
    ws->delete = mr_wordstreamer_iwords_delete;
    ws->create_another = mr_wordstreamer_iwords_create_another;

    /* Initialize offsets for filereader */
    Filereader *fr = ws->filereader;
    mr_filereader_set_offsets(fr, 0, fr->file_size - 1);

    return ws;
}


/**
 * Remove non-word characters to the stream.
 *
 * @param   fr[inout]            Pointer to the filereader
 * @param   character_ptr[in]    Pointer to the last character retrieved
 * @param   ret_ptr[in]          Pointer to the last returned value
 */
static inline void _mr_wordstreamer_iwords_remove_nonwords(Filereader *fr,
                                            char *character_ptr, int *ret_ptr) {
    int ret = *ret_ptr;
    char character = *character_ptr;

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
 * @param   fr[inout]            Pointer to the filereader
 * @param   character_ptr[in]    Pointer to the last character retrieved
 * @param   ret_ptr[in]          Pointer to the last returned value
 * @param   streamer_id[in]      Id of the current wordstreamer
 * @param   buffer[out]          Buffer to hold the retrieved word
 */
 static inline void _mr_wordstreamer_iwords_retrieve_word(Filereader *fr,
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
 * @param   ws[in]           Pointer to the Wordstreamer structure
 * @param   buffer[out]      Buffer to hold the retrieved word
 * @return  0 if a word was copied into the buffer or 1 if the end of the stream
 *          was reached
 */
int mr_wordstreamer_iwords_get(Wordstreamer *ws, char *buffer) {
    int ret, streamer_id = ws->streamer_id, nb_streamers = ws->nb_streamers;
    Filereader *fr = ws->filereader;
    char character, word[MAPREDUCE_MAX_WORD_SIZE];
    int word_count = 0;

    /* Return because end of stream already reached */
    if(ws->end) return 1;

    /* Get next byte */
    ret = mr_filereader_get_byte(fr, &character);

    /* Find the next word associated with the streamer id */
    while (word_count < nb_streamers && !ret) {
        /* Remove incomplete words, spaces and punctuation */
        _mr_wordstreamer_iwords_remove_nonwords(fr, &character, &ret);

        if (!ret) {
            /* Retrieve a complete word */
            _mr_wordstreamer_iwords_retrieve_word(fr, &character, &ret,
                                                             streamer_id, word);

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
