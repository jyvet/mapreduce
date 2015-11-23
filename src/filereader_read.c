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
 * @file filereader_read.c
 * @brief Filereader read implementation.
 * @author Jean-Yves VET
 */

#include "filereader_read.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

Filereader* _mr_filereader_read_create(const char*, const int,
                                                            const unsigned int);

/* ========================= Constructor / Destructor ======================= */

/**
 * Constructor for the first filereader.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @return  Pointer to the new Filereader structure
 */
Filereader* mr_filereader_read_create_first(const char* file_path,
                                          const unsigned int read_buffer_size) {

    return _mr_filereader_read_create(file_path, 0, read_buffer_size);
}


/**
 * Constructor for each other readers.
 *
 * @param   first[in]        Pointer to the first Filereader structure
 * @return  Pointer to the new Filereader structure
 */
Filereader* mr_filereader_read_create_another(const Filereader* first) {
    assert(first != NULL);

    Filereader_read *ext = first->ext;
    assert(ext != NULL);

    Filereader *fr = _mr_filereader_read_create(first->file_path, 1,
                                                              ext->buffer_size);

    return fr;
}


/**
 * Delete a Filereader structure.
 *
 * @param   fr_ptr[in]   Pointer to the Filereader structure
 */
void  mr_filereader_read_delete(Filereader* fr) {
    if (fr != NULL) {
        close(fr->fd);

        Filereader_read *ext =  fr->ext;
        assert(ext != NULL);

        free(ext->buffer);
        free(ext);

        _mr_filereader_common_delete(fr);
    }
}


/* ============================= Private functions ========================== */

/**
 * Internal constructor for Filereader.
 *
 * @param   file_path[in]         String containing the path to the file to read
 * @param   reader_id[in]         Id of the current Filereader
 * @param   read_buffer_size[in]  Size in bytes of the read buffer
 * @return  Pointer to the new Filereader structure
 */
Filereader* _mr_filereader_read_create(const char *file_path,
                     const int reader_id, const unsigned int read_buffer_size) {

    Filereader *fr = _mr_filereader_common_create(file_path, reader_id);

    /* Set function pointers */
    fr->create_another = mr_filereader_read_create_another;
    fr->delete = mr_filereader_read_delete;
    fr->get_byte = mr_filereader_read_get_byte;
    fr->set_offsets = mr_filereader_read_set_offsets;

    /* Alloc and initialize read extra data */
    Filereader_read *ext = malloc(sizeof(Filereader_read));
    assert(ext != NULL);
    fr->ext = ext;
    ext->buffer = malloc(read_buffer_size+1);
    assert(ext->buffer != NULL);
    ext->buffer_size = read_buffer_size;

    /* Open file */
    fr->fd = open(file_path, O_RDONLY | O_NONBLOCK);
    assert(fr->fd >= 0);

    /* Set filereader type */
    fr->type = FR_MMAP;

    /* Set default offsets */
    mr_filereader_read_set_offsets(fr, 0, fr->file_size);

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
void mr_filereader_read_set_offsets(Filereader *fr, long long start_offset,
                                                        long long stop_offset) {
    fr->offset = start_offset;
    fr->start_offset = start_offset;
    fr->stop_offset = stop_offset;

    /* Reset reader pos */
    int ret = lseek(fr->fd, start_offset, SEEK_SET);
    assert(ret != -1);

    Filereader_read *ext = fr->ext;

    /* Advise the kernel we need to read the file in sequential order */
    ret = posix_fadvise(fr->fd, 0, 0, POSIX_FADV_SEQUENTIAL);
    assert(!ret);

    /* Force read on next use */
    ext->buffer_offset = ext->buffer_size;
}


/**
 * Get next byte from a filereader.
 *
 * @param   fr[in]               Pointer to the Filereader structure
 * @param   buffer[out]          Buffer to hold the retrieved byte
 * @return  0 if a byte was copied into the buffer, -1 if the end of the file
 *          was reached, or the postion (>0) of the byte in the next area.
 */
int mr_filereader_read_get_byte(Filereader *fr, char *buffer) {
    long long offset = fr->offset;
    long long file_size = fr->file_size;

    if (offset < file_size) {
        long long stop_offset = fr->stop_offset;
        Filereader_read *ext = fr->ext;

        /* If end of buffer reached, we nead to read again */
        if (ext->buffer_offset >= ext->buffer_size) {
            int ret = read(fr->fd, ext->buffer, ext->buffer_size);
            assert(ret != -1);

            ext->buffer_offset = 0;
        }

        /* Retrieve byte */
        char *read_buffer = ext->buffer;
        char val = read_buffer[ext->buffer_offset++];
        buffer[0] = val;

        /* Prepare offset for next fonction call */
        fr->offset++;

        /* End_offset reached */
        if (offset > stop_offset) return (offset-stop_offset);
        else return 0;
    }
    else return -1; /* End of file reached */
}
