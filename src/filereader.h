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

#ifndef HEADER_MAPREDUCE_FILEREADER_H
    #define HEADER_MAPREDUCE_FILEREADER_H

    #include "common.h"
    #include "tools.h"

    typedef struct filereader_s Filereader;

    /**
     * @struct filereader_s
     * @brief  Structure containing information to manage filereader operations.
     */
    struct filereader_s {
        int         (*get_byte)();       /**<  Pointer to impl. of get_byte   */
        void        (*set_offsets)();    /**<  Pointer to impl. of set_offsets*/
        void        (*delete)();         /**<  Pointer to impl. of delete     */
        Filereader* (*create_another)(); /**<  Pointer to impl. create_another*/
        char*       file_path;     /**<  String for the path to the file      */
        long long   file_size;     /**<  File size in Bytes                   */
        long long   start_offset;  /**<  Start offset for the Wordstreamer    */
        long long   stop_offset;   /**<  End offset for the Wordstreamer      */
        long long   offset;        /**<  Offset index for the next character  */
        int         reader_id;     /**<  Filereader id                        */
        int         fd;            /**<  File descriptor                      */
        fr_type     type;          /**<  Filereader type                      */
        void*       ext;           /**<  Pointer to additional data           */
    };


    /* =========================== Static Elements ========================== */

    /**
     * Common constructor for implementations of Filereader. Static inline
     * definition to avoid to compile filreader.c when using an implemention.
     *
     * @param   file_path[in]     String containing the path to the file to read
     * @param   reader_id[in]     Id of the current filereader
     * @return  Pointer to the new Filereader structure
     */
    static inline Filereader* _mr_filereader_common_create(
                                   const char *file_path, const int reader_id) {
        assert(file_path != NULL);

        /* Allocate Filereader structure */
        Filereader *fr = malloc(sizeof(Filereader));
        assert(fr != NULL);

        /* Get file size and copy string in structure */
        fr->file_size = mr_tools_fsize(file_path);
        fr->file_path = malloc(fr->file_size+1);
        strcpy(fr->file_path, file_path);

        /* Initialize other variables */
        fr->reader_id = reader_id;

        return fr;
    }


    /**
     * Common destructor for implementations of Filereader. Static inline
     * definition to avoid to compile filreader.c when using an implemention.
     *
     * @param   fr[inout]     Pointer to the Filereader structure
     */
    static inline void _mr_filereader_common_delete(Filereader *fr) {
        assert(fr != NULL);

        /* Free string */
        free(fr->file_path);

        /* Free root structure */
        free(fr);
    }


    /**
     * Get next byte from a filreader. Static inline definition to improve
     * calling performance.
     *
     * @param   fr[in]          Pointer to the Filereader structure
     * @param   buffer[inout]   Buffer to hold the retrieved byte
     * @return  0 if a byte was copied into the buffer, -1 if end of file was
     *          reached and >0 if offset > stop-offset.
     */
    static inline int mr_filereader_get_byte(Filereader *fr, char *buffer) {
        return fr->get_byte(fr, buffer);
    }


    /* ============================== Prototypes ============================ */

    Filereader*  mr_filereader_create_first(const char*, const fr_type,
                                                            const unsigned int);
    Filereader*  mr_filereader_create_another(const Filereader*);
    void         mr_filereader_delete(Filereader**);

    int          mr_filereader_get_byte(Filereader*, char*);
    void         mr_filereader_set_offsets(Filereader*, long long, long long);
#endif
