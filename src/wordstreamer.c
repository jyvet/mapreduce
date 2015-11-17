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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "tools.h"
#include "wordstreamer.h"
#include "wordstreamer_scatter.h"
#include "wordstreamer_interleave.h"

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
Wordstreamer* mr_wordstreamer_create_first (const char* file_path,
                       const int nb_streamers, const int type, bool profiling) {
    Wordstreamer *ws;

    switch(type) {
        default:
        case TYPE_WORDSTREAMER_INTERLEAVE :
            ws = mr_wordstreamer_interleave_create_first(file_path,
                                                       nb_streamers, profiling);
            break;
        case TYPE_WORDSTREAMER_SCATTER :
            ws = mr_wordstreamer_scatter_create_first(file_path, nb_streamers,
                                                                     profiling);
            break;
    }

    return ws;
}


/**
 * Constructor for each other streamer.
 *
 * @param   first[in]        String containing the path to the file to read
 * @param   streamer_id[in]  Total number of streamers
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* mr_wordstreamer_create_another(Wordstreamer* first,
                                                        const int streamer_id) {
    Wordstreamer *ws;

    unsigned int type = first->type;

    switch(type) {
        default:
        case TYPE_WORDSTREAMER_INTERLEAVE :
            ws = mr_wordstreamer_interleave_create_another(first, streamer_id);
            break;
        case TYPE_WORDSTREAMER_SCATTER :
            ws = mr_wordstreamer_scatter_create_another(first, streamer_id);
            break;
    }

    return ws;
}


/**
 * Delete a Wordstreamer structure.
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
 * Common constructor for implementations of Wordstreamer.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @param   shared_map[in]    Pointer to area where the file was mmaped
 * @param   streamer_id[in]   Id of the current wordstreamer
 * @param   nb_streamers[in]  Total number of streamers
 * @param   profiling[in]     Activate the profiling mode
 * @return  Pointer to the new Wordstreamer structure
 */
Wordstreamer* _mr_wordstreamer_common_create(
                 const char *file_path, char *shared_map, const int streamer_id,
                                       const int nb_streamers, bool profiling) {
    assert(nb_streamers != 0
           && streamer_id < nb_streamers);

    Wordstreamer *ws = malloc(sizeof(Wordstreamer));
    assert(ws != NULL);

    ws->profiling = profiling;
    _timer_init(&ws->timer_get, ws->profiling);

    ws->streamer_id = streamer_id;
    ws->end = false;
    ws->nb_streamers = nb_streamers;

    /* If streamer_id = 0, open file and map it to memory */
    if (!streamer_id) {
        ws->file_size = mr_tools_fsize(file_path);
        assert(file_path != NULL);
        ws->fd = open(file_path, O_RDONLY | O_NONBLOCK);
        assert(ws->fd >= 0);

        ws->shared_map = mmap(NULL, ws->file_size, PROT_READ, MAP_PRIVATE,
                                                                     ws->fd, 0);
        assert(ws->shared_map != MAP_FAILED);
    } else {
        assert(shared_map != NULL);
        ws->shared_map = shared_map;
    }

    return ws;
}


/**
 * Common destructor for implementations of Wordstreamer.
 *
 * @param   ws[inout]     Pointer to the Wordstreamer structure
 */
void _mr_wordstreamer_common_delete(Wordstreamer *ws) {
    assert(ws != NULL);

    /* Display profile with stream id if requiered */
    char str[32];
    char str_buf[32];
    sprintf(str_buf, "%d", ws->streamer_id);
    strcpy(str, "[WordStreamer ");
    strcat(str, str_buf);
    strcat(str, "] get");
    _timer_print(&ws->timer_get, str);

    /* Close file */
    if (ws->streamer_id == 0) {
        assert(ws->shared_map != NULL);
        munmap(ws->shared_map, ws->chunk_size);
    }

    /* Free root structure */
    free(ws);
}


/**
 * Get next word from a wordstreamer. Return 1 if end of stream reached.
 *
 * @param   ws[in]               Pointer to the Wordstreamer structure
 * @param   shared_map[inout]    Buffer to hold the retrieved word
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
