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

#ifndef HEADER_MAPREDUCE_FILEREADER_MMAP_H
    #define HEADER_MAPREDUCE_FILEREADER_MMAP_H

    #include "filereader.h"

    /**
     * @struct filereader_mmap_s
     * @brief  Structure containing extra data for filereader_mmap
     */
    typedef struct filereader_mmap_s {
        char*       shared_map;    /**<  Memory area where the file is mapped */
    } Filereader_mmap;

    /* ============================== Prototypes ============================ */

    Filereader*  mr_filereader_mmap_create_first(const char*);
    Filereader*  mr_filereader_mmap_create_another(const Filereader*);
    void         mr_filereader_mmap_delete(Filereader*);

    int          mr_filereader_mmap_get_byte(Filereader*, char*);
    void         mr_filereader_mmap_set_offsets(Filereader*, long long,
                                                                     long long);

#endif
