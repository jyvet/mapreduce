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

#ifndef HEADER_MAPREDUCE_WORDSTREAMER_H
    #define HEADER_MAPREDUCE_WORDSTREAMER_H

    #include "common.h"
    #include "tools.h"
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/mman.h>

    typedef struct wordstreamer_s Wordstreamer;

    /**
     * @struct wordstreamer_s
     * @brief  Structure containing information to manage word streams from a
     *         file. Multiple Wordstreamers may be created to manage streams in
     *         parallel.
     */
    struct wordstreamer_s {
        int          (*get)();
        void         (*delete)();
        Wordstreamer* (*create_another)();
        char*        shared_map;   /**<  Memory area where the file is mapped */
        char*        file_path;    /**<  String for the path to the file      */
        long int     file_size;    /**<  File size in Bytes                   */
        long int     start_offset; /**<  Start offset for the Wordstreamer    */
        long int     stop_offset;  /**<  End offset for the Wordstreamer      */
        long int     chunk_size;   /**<  Chunk size for the Wordstreamer      */
        long int     offset;       /**<  Offset index for the next character  */
        int          fd;           /**<  File descriptor                      */
        unsigned int streamer_id;  /**<  Id of the current Wordstreamer       */
        unsigned int nb_streamers; /**<  Total number of streamers            */
        Timer        timer_get;    /**<  Timer for get func. [Profiling mode] */
        bool         end;          /**<  End of all chunks reached            */
        bool         profiling;    /**<  Profiling mode                       */
    };


    /* =========================== Static Elements ========================== */

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
    static inline Wordstreamer* _mr_wordstreamer_common_create(
                 const char *file_path, char *shared_map, const int streamer_id,
                                       const int nb_streamers, bool profiling) {
        assert(nb_streamers != 0
               && streamer_id < nb_streamers
               && file_path != NULL);

        /* Allocate Wordstreamer structure */
        Wordstreamer *ws = malloc(sizeof(Wordstreamer));
        assert(ws != NULL);

        /* Get file size and copy string in structure */
        ws->file_size = mr_tools_fsize(file_path);
        ws->file_path = malloc(ws->file_size+1);
        strcpy(ws->file_path, file_path);

        /* Initilize profiling variable and timer counter */
        ws->profiling = profiling;
        _timer_init(&ws->timer_get, ws->profiling);


        ws->streamer_id = streamer_id;
        ws->nb_streamers = nb_streamers;

        /* Initialize other variables */
        ws->end = false;

        /* If streamer_id = 0, open file and map it to memory */
        if (!streamer_id) {
            ws->fd = open(file_path, O_RDONLY | O_NONBLOCK);
            assert(ws->fd >= 0);

            /* Map file to memory to make accesses from multiple threads
               easier */
            ws->shared_map = mmap(NULL, ws->file_size, PROT_READ, MAP_PRIVATE |
                                                      MAP_POPULATE,  ws->fd, 0);
            assert(ws->shared_map != MAP_FAILED);

            /* Advise the kernel we need to read completely the mapped file in
               sequential order */
            madvise(ws->shared_map, ws->file_size, MADV_SEQUENTIAL |
                                                                 MADV_WILLNEED);

        /* If streamer_id > 0, use mmap ptr obtained by streamer_id 0 */
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
    static inline void _mr_wordstreamer_common_delete(Wordstreamer *ws) {
        assert(ws != NULL);

        /* Display profile with stream id if requiered */
        char str[32];
        char str_buf[32];
        sprintf(str_buf, "%d", ws->streamer_id);
        strcpy(str, "[WordStreamer ");
        strcat(str, str_buf);
        strcat(str, "] get");
        _timer_print(&ws->timer_get, str);

        /* Close file and unmap area */
        if (ws->streamer_id == 0) {
            close(ws->fd);

            assert(ws->shared_map != NULL);
            munmap(ws->shared_map, ws->chunk_size);
        }

        /* Free string */
        free(ws->file_path);

        /* Free root structure */
        free(ws);
    }


    /* ============================== Prototypes ============================ */

    Wordstreamer*  mr_wordstreamer_create_first(const char*, const int,
                                                           const ws_type, bool);
    Wordstreamer*  mr_wordstreamer_create_another(const Wordstreamer*,
                                                                     const int);

    void           mr_wordstreamer_delete(Wordstreamer**);
    int            mr_wordstreamer_get(Wordstreamer*, char*);
#endif
