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
 * @file filereader_mmap.c
 * @brief Filereader mmap implementation.
 * @author Jean-Yves VET
 */

#include "filereader_mmap.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

Filereader* _mr_filereader_mmap_create(const char*, const int);

/* ========================= Constructor / Destructor ======================= */

/**
 * Constructor for the first filereader.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @return  Pointer to the new Filereader structure
 */
Filereader* mr_filereader_mmap_create_first(const char* file_path) {

    Filereader *fr = _mr_filereader_mmap_create(file_path, 0);

    fr->fd = open(file_path, O_RDONLY | O_NONBLOCK);
    assert(fr->fd >= 0);

    /* Map file to memory */
    Filereader_mmap *ext = fr->ext;
    ext->shared_map = mmap(NULL, fr->file_size, PROT_READ, MAP_PRIVATE |
                                                       MAP_POPULATE, fr->fd, 0);
    assert(ext->shared_map != MAP_FAILED);

    /* Advise the kernel we need to read completely the mapped file in
       sequential order */
    madvise(ext->shared_map, fr->file_size, MADV_SEQUENTIAL | MADV_WILLNEED);

    return fr;
}


/**
 * Constructor for each other filereaders.
 *
 * @param   first[in]        Pointer to the first Filereader structure
 * @return  Pointer to the new Filereader structure
 */
Filereader* mr_filereader_mmap_create_another(const Filereader* first) {
    assert(first != NULL);
    Filereader *fr = _mr_filereader_mmap_create(first->file_path, 1);

    Filereader_mmap *first_ext = first->ext;
    Filereader_mmap *ext = fr->ext;

    /* Use mmap ptr obtained by reader_id 0 */
    assert(first_ext->shared_map != NULL);
    ext->shared_map = first_ext->shared_map;

    return fr;
}


/**
 * Delete a Filereader structure.
 *
 * @param   fr_ptr[in]   Pointer to the Filereader structure
 */
void  mr_filereader_mmap_delete(Filereader* fr) {
    if (fr != NULL) {

        if (fr->reader_id == 0) {
            close(fr->fd);

            Filereader_mmap *ext = fr->ext;
            assert(ext->shared_map != NULL);
            munmap(ext->shared_map, fr->file_size);
        }
        assert(fr->ext != NULL);
        free(fr->ext);

        _mr_filereader_common_delete(fr);
    }
}


/* ============================= Private functions ========================== */

/**
 * Internal constructor for Filereader.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @param   reader_id[in]     Id of the current Filereader
 * @return  Pointer to the new Filereader structure
 */
Filereader* _mr_filereader_mmap_create(const char *file_path,
                                                          const int reader_id) {

    Filereader *fr = _mr_filereader_common_create(file_path, reader_id);

    /* Set function pointers */
    fr->create_another = mr_filereader_mmap_create_another;
    fr->delete = mr_filereader_mmap_delete;
    fr->get_byte = mr_filereader_mmap_get_byte;
    fr->set_offsets = mr_filereader_mmap_set_offsets;

    /* Alloc and initialize mmap extra data */
    Filereader_mmap *ext = malloc(sizeof(Filereader_mmap));
    assert(ext != NULL);
    ext->shared_map = NULL;
    fr->ext = ext;

    /* Set filereader type */
    fr->type = FR_MMAP;

    /* Set default offsets */
    mr_filereader_mmap_set_offsets(fr, 0, fr->file_size - 1);

    return fr;
}


/* ============================= Public functions =========================== */

/**
 * Set offsets for the provider filereader.
 *
 * @param   fr[inout]            Pointer to the Filereader structure
 * @param   start_offset[in]     Offset where the reader shall start
 * @param   stop_offset[in]      Offset where the reader shall stop
 */
void mr_filereader_mmap_set_offsets(Filereader *fr, long long start_offset,
                                                        long long stop_offset) {
    fr->offset = start_offset;
    fr->start_offset = start_offset;
    fr->stop_offset = stop_offset;
}


/**
 * Get next byte from a filereader.
 *
 * @param   fr[in]               Pointer to the Filereader structure
 * @param   buffer[out]          Buffer to hold the retrieved byte
 * @return  0 if a byte was copied into the buffer, -1 if the end of the file
 *          was reached, or the postion (>0) of the byte in the next area.
 */
int mr_filereader_mmap_get_byte(Filereader *fr, char *buffer) {
    long long offset = fr->offset;
    long long file_size = fr->file_size;

    if (offset < file_size) {
        long long stop_offset = fr->stop_offset;
        Filereader_mmap *ext = fr->ext;
        char *shared_map = ext->shared_map;

        /* Retrieve byte */
        char val = shared_map[offset];
        buffer[0] = val;

        /* Prepare offset for next fonction call */
        fr->offset++;

        /* End_offset reached */
        if (offset > stop_offset) return (offset-stop_offset);
        else return 0;
    }
    else return -1; /* End of file reached */
}
