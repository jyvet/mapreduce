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
        int          (*get)();              /**<  Pointer to impl. of get     */
        void         (*delete)();           /**<  Pointer to impl. of delete  */
        Wordstreamer* (*create_another)();  /**<  Pointer to impl.            */
        Filereader*  filereader;   /**<  Pointer to a filereader              */
        fr_type      reader_type;  /**<  Type of filereader (see common.h)    */
        unsigned int streamer_id;  /**<  Id of the current Wordstreamer       */
        unsigned int nb_streamers; /**<  Total number of streamers            */
        Timer        timer_get;    /**<  Timer for get func. [Profiling mode] */
        bool         end;          /**<  End of all chunks reached            */
        bool         profiling;    /**<  Profiling mode                       */
    };


    /* =========================== Static Elements ========================== */

    /**
     * Common constructor for implementations of Wordstreamer. Static inline
     * definition to avoid to compile wordstreamer.c when using an implemention.
     *

     * @param   file_path[in]     String containing the path to the file to read
     * @param   reader_type[in]      Type of filereader to use (see common.h)
     * @param   first_reader[in]     Pointer to first filereader structure
     * @param   read_buffer_size[in] Size in bytes of the read buffer
     * @param   streamer_id[in]      Id of the current streamer
     * @param   nb_streamers[in]     Total number of streamers
     * @param   profiling[in]        Activate the profiling mode
     * @return  Pointer to the new Wordstreamer structure
     */
    static inline Wordstreamer* _mr_wordstreamer_common_create(
                const char *file_path, fr_type reader_type,
                Filereader *first_reader, const unsigned int read_buffer_size,
                const int streamer_id, const int nb_streamers, bool profiling) {

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
        ws->reader_type = reader_type;
        ws->streamer_id = streamer_id;
        ws->nb_streamers = nb_streamers;
        ws->end = false;

        /* If streamer_id = 0, create first filereader */
        if (streamer_id == 0) {
            ws->filereader = mr_filereader_create_first(file_path, reader_type,
                                                              read_buffer_size);
        } else {
            assert(first_reader != NULL);
            ws->filereader = mr_filereader_create_another(first_reader);
        }

        return ws;
    }


    /**
     * Common destructor for implementations of Wordstreamer. Static inline
     * definition to avoid to compile wordstreamer.c when using an implemention.
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
     * Static inline definition to improve calling performance.
     *
     * @param   ws[in]               Pointer to the Wordstreamer structure
     * @param   buffer[inout]        Buffer to hold the retrieved word
     * @return  0 if a word was copied into the buffer or 1 if the end of the
     *          stream was reached
     */
    static inline int mr_wordstreamer_get(Wordstreamer *ws, char *buffer) {
        _timer_start(&ws->timer_get);
        int ret = ws->get(ws, buffer);
        _timer_stop(&ws->timer_get);

        return ret;
    }


    /* ============================== Prototypes ============================ */

    Wordstreamer*  mr_wordstreamer_create_first(const char*, const int,
                        const ws_type, const fr_type, const unsigned int, bool);
    Wordstreamer*  mr_wordstreamer_create_another(const Wordstreamer*,
                                                                     const int);

    void           mr_wordstreamer_delete(Wordstreamer**);
    int            mr_wordstreamer_get(Wordstreamer*, char*);
#endif
