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
    #include "filereader.h"
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
        Filereader*  filereader;
        //char*        shared_map;   /**<  Memory area where the file is mapped */
        //char*        file_path;    /**<  String for the path to the file      */
        //long long    file_size;    /**<  File size in Bytes                   */
        //long long    start_offset; /**<  Start offset for the Wordstreamer    */
        //long long    stop_offset;  /**<  End offset for the Wordstreamer      */
        long long    chunk_size;   /**<  Chunk size for the Wordstreamer      */
        //long long    offset;       /**<  Offset index for the next character  */
        //int          fd;           /**<  File descriptor                      */
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
          const char *file_path, Filereader *first_reader, const int streamer_id,
                                       const int nb_streamers, bool profiling) {
        assert(nb_streamers != 0
               && streamer_id < nb_streamers
               && file_path != NULL);

        /* Allocate Wordstreamer structure */
        Wordstreamer *ws = malloc(sizeof(Wordstreamer));
        assert(ws != NULL);

        /* Initilize profiling variable and timer counter */
        ws->profiling = profiling;
        _timer_init(&ws->timer_get, ws->profiling);

        /* Initialize other variables */
        ws->streamer_id = streamer_id;
        ws->nb_streamers = nb_streamers;
        ws->end = false;

        /* If streamer_id = 0, create first filereader */
        if (streamer_id == 0) {
            ws->filereader = mr_filereader_create_first(file_path, FR_MMAP,
                                                                     profiling);
        } else {
            assert(first_reader != NULL);
            ws->filereader = mr_filereader_create_another(first_reader);
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

        /* Delete filereader */
        mr_filereader_delete(&ws->filereader);

        /* Free root structure */
        free(ws);
    }


    /**
     * Get next word from a wordstreamer. Return 1 if end of stream reached.
     *
     * @param   ws[in]               Pointer to the Wordstreamer structure
     * @param   buffer[inout]        Buffer to hold the retrieved word
     * @return  0 if a word was copied into the buffer or 1 if the end of the stream
     *          was reached
     */
    static inline int mr_wordstreamer_get(Wordstreamer *ws, char *buffer) {
        _timer_start(&ws->timer_get);
        int ret = ws->get(ws, buffer);
        _timer_stop(&ws->timer_get);

        return ret;
    }


    /* ============================== Prototypes ============================ */

    Wordstreamer*  mr_wordstreamer_create_first(const char*, const int,
                                                           const ws_type, bool);
    Wordstreamer*  mr_wordstreamer_create_another(const Wordstreamer*,
                                                                     const int);

    void           mr_wordstreamer_delete(Wordstreamer**);
    int            mr_wordstreamer_get(Wordstreamer*, char*);
#endif
