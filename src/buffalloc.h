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

#ifndef HEADER_MAPREDUCE_BUFFALLOC_H
    #define HEADER_MAPREDUCE_BUFFALLOC_H

    #include "common.h"

    /**
     * @struct buffalloc_chunk_s
     * @brief  Structure containing information to manage data stored
     *         consecutively.
     */
    typedef struct buffalloc_chunk_s {
        void*                      free_ptr;   /**<  Pointer to free area     */
        struct buffalloc_chunk_s*  next;       /**<  Path to next chunk       */
        size_t                     free_size;  /**<  Memory available         */
    } Buffalloc_chunk;


    /**
     * @struct buffalloc_s
     * @brief  Structure which holds information about buffered allocations.
     */
    typedef struct buffalloc_s {
        size_t            chunk_size;     /**<  Size in Bytes of each chunk   */
        size_t            alloc_size;     /**<  Size in Bytes of data managed */
        Buffalloc_chunk*  last_chunk;     /**<  Pointer to the first chunk    */
    } Buffalloc;


    /* ============================== Prototypes ============================ */

    Buffalloc*  mr_buffalloc_create();
    void        mr_buffalloc_delete(Buffalloc**);

    void*       mr_buffalloc_malloc(Buffalloc*, size_t);
#endif
