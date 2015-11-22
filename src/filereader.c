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
 * @file filereader.c
 * @brief Filereader interface to test several reader implementations.
 * @author Jean-Yves VET
 */

#include "filereader.h"
#include "filereader_mmap.h"
#include "filereader_read.h"

/* ========================= Constructor / Destructor ======================= */

/**
 * Constructor for the first reader.
 *
 * @param   file_path[in]         String containing the path to the file to read
 * @param   type[in]              Type of Filereader (see common.h)
 * @param   read_buffer_size[in]  Size in bytes for the buffer (read mod only)
 * @return  Pointer to the new Filereader structure
 */
Filereader* mr_filereader_create_first(const char* file_path,
                      const fr_type type, const unsigned int read_buffer_size) {
    Filereader *fr;

    switch(type) {
        default:
        case FR_MMAP :
            fr = mr_filereader_mmap_create_first(file_path);
            break;
        case FR_READ :
            fr = mr_filereader_read_create_first(file_path, read_buffer_size);
            break;
    }

    return fr;
}


/**
 * Constructor for each other readers.
 *
 * @param   first[in]        Pointer to the first Filereader structure
 * @return  Pointer to the new Filereader structure
 */
Filereader* mr_filereader_create_another(const Filereader* first) {
    return first->create_another(first);
}


/**
 * Delete a Filereader structure and set pointer to NULL.
 *
 * @param   fr_ptr[in]   Pointer to pointer of a Filereader structure
 */
void mr_filereader_delete(Filereader **fr_ptr) {
    assert(fr_ptr != NULL);
    Filereader *fr = *fr_ptr;

    fr->delete(fr);

    /* Set pointer to NULL */
    *fr_ptr = NULL;
}


/* ============================= Public functions =========================== */

/**
 * Set offsets for the provider filereader.
 *
 * @param   fr[inout]            Pointer to the Filereader structure
 * @param   start_offset[in]     Offset where the reader shall start
 * @param   stop_offset[in]      Offset where the reader shall stop
 */
void mr_filereader_set_offsets(Filereader *fr, long long start_offset,
                                                        long long stop_offset) {
    assert(fr != NULL);

    return fr->set_offsets(fr, start_offset, stop_offset);
}
