/***************************************************************************
*
* Copyright (C) 2015, Jean-Yves VET, <contact@jean-yves.vet>
*
* This software is licensed as described in the file LICENCE, which
* you should have received as part of this distribution.
*
* You may opt to use, copy, modify, merge, publish, distribute and/or sell
* copies of the Software, and permit persons to whom the Software is
* furnished to do so, under the terms of the LICENCE file.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express or implied.
***************************************************************************/

#ifndef HEADER_MAPREDUCE_WORDSTREAMER_H
    #define HEADER_MAPREDUCE_WORDSTREAMER_H

    #include "common.h"

    /**
     * @struct wordstreamer_s
     * @brief  Structure containing information to manage word streams from a
     *         file. Multiple Wordstreamers may be created to manage streams in
     *         parallel.
     */
    typedef struct wordstreamer_s {
        int          (*get)();
        void         (*delete)();
        char*        shared_map;   /**<  Memory area where the file is mapped */
        char*        file_path;    /**<  String for the path to the file      */
        long int     file_size;    /**<  File size in Bytes                   */
        long int     start_offset; /**<  Start offset for the Wordstreamer    */
        long int     stop_offset;  /**<  End offset for the Wordstreamer      */
        long int     chunk_size;   /**<  Chunk size for the Wordstreamer      */
        long int     offset;       /**<  Offset index for the next character  */
        int          fd;           /**<  File descriptor                      */
        unsigned int type;         /**<  Type of Wordstreamer (see common.h)  */
        unsigned int streamer_id;  /**<  Id of the current Wordstreamer       */
        unsigned int nb_streamers; /**<  Total number of streamers            */
        Timer        timer_get;    /**<  Timer for get func. [Profiling mode] */
        bool         end;          /**<  End of chunk reached                 */
        bool         profiling;    /**<  Profiling mode                       */
    } Wordstreamer;


    /* ============================== Prototypes ============================ */

    Wordstreamer*  _mr_wordstreamer_common_create(const char*,
                                             char*, const int, const int, bool);
    Wordstreamer*  mr_wordstreamer_create_first(const char*, const int,
                                                               const int, bool);
    Wordstreamer*  mr_wordstreamer_create_another(Wordstreamer*, const int);

    void           _mr_wordstreamer_common_delete(Wordstreamer*);
    void           mr_wordstreamer_delete(Wordstreamer**);
    int            mr_wordstreamer_get(Wordstreamer*, char*);
#endif
