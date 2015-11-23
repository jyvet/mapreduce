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
 * @file buffalloc.c
 * @brief Create a buffer to allocate data consecutively.
 * @author Jean-Yves VET
 */

#include "buffalloc.h"

/* ========================= Constructor / Destructor ======================= */

/**
 * Create a Buffalloc structure with default values.
 *
 * @return  Buffalloc   Pointer to the new Buffalloc structure
 */
Buffalloc *mr_buffalloc_create() {
    Buffalloc *ba = malloc(sizeof(Buffalloc));
    assert(ba != NULL);

    ba->chunk_size = MAPREDUCE_BUFFALLOC_CHUNK_SIZE;
    ba->alloc_size = 0;
    ba->last_chunk = NULL;

    return ba;
}


 /**
  * Delete a Buffalloc structure.
  *
  * @param   ba_ptr[in]   Pointer to pointer of a Buffalloc structure
  */
void mr_buffalloc_delete(Buffalloc **ba_ptr) {
    Buffalloc *ba = *ba_ptr;

    if (ba != NULL) {
        Buffalloc_chunk *chunk = ba->last_chunk;
        while (chunk != NULL) {
            Buffalloc_chunk *chunk_tmp = chunk;
            chunk = chunk->next;
            free(chunk_tmp);
        }

        free(ba);
    }

    *ba_ptr = NULL;
}


/* ============================ Private Functions =========================== */

/**
 * Create a Buffalloc_chunk structure.
 *
 * @param   size[in]          Size of data which may be stored in the chunk
 * @return  Buffalloc_chunk   Pointer to the new Buffalloc_chunk structure
 */
Buffalloc_chunk *_mr_buffalloc_chunk_create(size_t size) {
    Buffalloc_chunk *chunk = malloc(sizeof(Buffalloc_chunk) + size);
    assert(chunk != NULL);

    chunk->next = NULL;
    chunk->free_size = size;
    chunk->free_ptr = (void*)(chunk + 1);

    return chunk;
}


/**
 * Get a pointer to <size> allocated free space from a chunk.
 *
 * @param   chunk[in]       Pointer to a Buffalloc_chunk structure
 * @param   size[in]        Size of data to be allocated
 * @return  void            Pointer the allocated area
 */
void *_mr_buffalloc_chunk_malloc(Buffalloc_chunk *chunk, size_t size) {
    assert(chunk != NULL);

    if (chunk->free_size < size) {
        return NULL;
    } else {
        void *area = chunk->free_ptr;
        chunk->free_size -= size;
        chunk->free_ptr += size;

        return area;
    }
}


/* ============================ Public Functions ============================ */

/**
 * Get a pointer to <size> allocated free space.
 *
 * @param   ba[in]          Pointer to a Buffalloc structure
 * @param   size[in]        Size of data to be allocated
 * @return  void            Pointer the allocated area
 */
void* mr_buffalloc_malloc(Buffalloc *ba, size_t size) {
    void *area = NULL;
    assert(ba != NULL);
    size_t chunk_size = ba->chunk_size;
    assert(chunk_size >= size);

    /* Try to find available area in allocated chunks */
    Buffalloc_chunk *chunk = ba->last_chunk;
    while (chunk != NULL) {
        area = _mr_buffalloc_chunk_malloc(chunk, size);

        if (area != NULL) {
            ba->alloc_size += size;
            return area;
        }

        chunk = chunk->next;
    }

    /* We need to create a new chunk */
    chunk = _mr_buffalloc_chunk_create(chunk_size);
    area = _mr_buffalloc_chunk_malloc(chunk, size);

    /* Update Buffalloc structure */
    chunk->next = ba->last_chunk;
    ba->alloc_size += chunk_size;
    ba->last_chunk = chunk;

    return area;
}
