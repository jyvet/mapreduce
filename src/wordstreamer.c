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
 * @file wordstreamer.c
 * @brief Wordstreamer interface to test several streamers implementations.
 * @author Jean-Yves VET
 */

#include "wordstreamer.h"
#include "wordstreamer_schunks.h"
#include "wordstreamer_iwords.h"

/* ========================= Constructor / Destructor ======================= */

/**
 * Constructor for the first streamer.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @param   nb_streamers[in]  Total number of streamers
 * @param   type[in]          Type of Wordstreamer (see common.h)
 * @param   profiling[in]     Activate the profiling mode
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* mr_wordstreamer_create_first(const char* file_path,
                   const int nb_streamers, const ws_type type, bool profiling) {
    Wordstreamer *ws;

    switch(type) {
        default:
        case WS_IWORDS :
            ws = mr_wordstreamer_iwords_create_first(file_path,
                                                       nb_streamers, profiling);
            break;
        case WS_SCHUNKS :
            ws = mr_wordstreamer_schunks_create_first(file_path, nb_streamers,
                                                                     profiling);
            break;
    }

    return ws;
}


/**
 * Constructor for each other streamer.
 *
 * @param   first[in]        String containing the path to the file to read
 * @param   streamer_id[in]  Id of the current wordstreamer
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* mr_wordstreamer_create_another(const Wordstreamer* first,
                                                        const int streamer_id) {
    return first->create_another(first, streamer_id);
}


/**
 * Delete a Wordstreamer structure and set pointer to NULL.
 *
 * @param   ws_ptr[in]   Pointer to pointer of a Wordstreamer structure
 */
void mr_wordstreamer_delete(Wordstreamer **ws_ptr) {
    assert(ws_ptr != NULL);
    Wordstreamer *ws = *ws_ptr;

    ws->delete(ws);

    /* Set pointer to NULL */
    *ws_ptr = NULL;
}


/* ============================= Public functions =========================== */

/**
 * Get next word from a wordstreamer. Return 1 if end of stream reached.
 *
 * @param   ws[in]               Pointer to the Wordstreamer structure
 * @param   buffer[inout]        Buffer to hold the retrieved word
 * @return  0 if a word was copied into the buffer or 1 if the end of the stream
 *          was reached
 */
int mr_wordstreamer_get(Wordstreamer *ws, char *buffer) {
    assert(ws != NULL);

    _timer_start(&ws->timer_get);
    int ret = ws->get(ws, buffer);
    _timer_stop(&ws->timer_get);

    return ret;
}
