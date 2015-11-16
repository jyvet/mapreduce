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
 * @file mapreduce.c
 * @brief Mapreduce interface to test several mapreduce implementations.
 * @author Jean-Yves VET
 */

#include "mapreduce.h"
#include "mapreduce_sequential.h"
#include "mapreduce_parallel.h"
#include "tools.h"

void _stats_total(Mapreduce *);

/* ========================= Constructor / Destructor ======================= */

/**
 * Constructor with detailled arguments to make unit tests easier.
 *
 * @param  file_path[in]    String containg the path to the file we want to read
 * @param  nb_threads[in]   Number of threads
 * @param  type[in]         Type of mapreduce to use
 * @param  quiet[in]        Activate the quiet mode (no output)
 * @param  profiling[in]    Activate the profiling mode
 * @return  A Mapreduce structure
 */
Mapreduce* _mr_create(const char *file_path, const int nb_threads,
                       const int type, const bool quiet, const bool profiling) {
    Mapreduce *mr;

    switch(type) {
        default:
        case TYPE_PARALLEL :
            mr = mr_parallel_create(file_path, nb_threads, quiet, profiling);
            break;
        case TYPE_SEQUENTIAL :
            mr = mr_sequential_create(file_path, quiet, profiling);
            break;
    }

    return mr;
}


/**
 * Constructor based on Arguments provided to the program.
 *
 * @param   args[in]        Arguments
 * @return  A Mapreduce structure
 */
Mapreduce* mr_create(Arguments *args) {
    assert(args != NULL);
    return _mr_create(args->file_path, args->nb_threads, args->type,
                                                  args->quiet, args->profiling);
}


/**
 * Delete Mapreduce structure and set pointer to NULL.
 *
 * @param   mr_ptr[inout]     Pointer to pointer of Mapreduce structure
 */
void mr_delete(Mapreduce **mr_ptr) {
    Mapreduce *mr = *mr_ptr;
    assert(mr != NULL);

    _timer_stop(&mr->timer_global);
    mr->delete(mr);

    /* Display profile if requiered */
    _timer_print(&mr->timer_map, "[MapReduce] map");
    _timer_print(&mr->timer_reduce, "[MapReduce] reduce");
    _timer_print(&mr->timer_global, "--> TOTAL");
    _stats_total(mr);

    if (mr != NULL) {
        if (mr->file_path != NULL) free(mr->file_path);
        free(mr);
    }
    *mr_ptr = NULL;
}


/* ============================= Private functions ========================== */

/**
 * Print statistics such as Words/s.
 *
 * @param   mr[in]     Pointer to a Mapreduce structure
 */
void _stats_total(Mapreduce *mr) {
    if (mr->profiling) {
        long int fsize = mr_tools_fsize(mr->file_path)/1048576.0;
        long int words = mr_tools_wc(mr->file_path);
        Timer *timer_global = &mr->timer_global;
        double elapsed = ((double)timer_global->elapsed)/1E6;

        if (elapsed > 0.001) {
            #if MAPREDUCE_DEFAULT_USECOLORS
                printf("\e[33m |---> [File Size: %ld MB]  -> "
                       "\e[1;33m %.3f MB/s\e[0m\n",
                       fsize, ((double)fsize/elapsed));
                printf("\e[33m |---> [Words: %ld]  -> "
                       "\e[1;33m %.3f MWords/s\e[0m\n",
                       words, ((double)words/1E6)/elapsed);
            #else
                printf(" |---> [File Size: %ld MB]  ->  %.3f MB/s\n",
                        fsize, ((double)fsize/elapsed));
                printf(" |---> [Words: %ld]  ->  %.3f MWords/s\n",
                        words, ((double)words/1E6)/elapsed);
            #endif
        }
    }
}


/* ============================= Public functions =========================== */

/**
 * Common constructor for implementations of Mapreduce.
 *
 * @param   file_path[in]     String containing the path to the file to read
 * @param   nb_threads[in]    Total number of threads
 * @param   type[in]          Type of mapreduce (see common.h)
 * @param   quiet[in]         Activate the quiet mode (no output)
 * @param   profiling[in]     Activate the profiling mode
 * @return  Pointer to the new Wordstreamer structure
 */
Mapreduce* mr_common_create(const char *file_path, const int nb_threads,
                       const int type, const bool quiet, const bool profiling) {

    Mapreduce *mr = malloc(sizeof(Mapreduce));

    mr->file_path = malloc(strlen(file_path)+1);
    strcpy(mr->file_path, file_path);
    mr->nb_threads = nb_threads;
    mr->type = type;
    mr->quiet = quiet;
    mr->profiling = profiling;

    _timer_init(&mr->timer_map, profiling);
    _timer_init(&mr->timer_reduce, profiling);
    _timer_init(&mr->timer_global, profiling);
    _timer_start(&mr->timer_global);

    return mr;
}


/**
 * Map operation.
 *
 * @param   mr[inout]     Pointer to a Mapreduce structure
 */
void mr_map(Mapreduce *mr) {
    assert(mr != NULL);

    _timer_start(&mr->timer_map);
    mr->map(mr);
    _timer_stop(&mr->timer_map);
}


/**
 * Map operation : Aggregate results, and display them, sorted by word.
 *
 * @param   mr[inout]     Pointer to a Mapreduce structure
 */
void mr_reduce(Mapreduce *mr) {
    assert(mr != NULL);

    _timer_start(&mr->timer_reduce);
    mr->reduce(mr);
    _timer_stop(&mr->timer_reduce);
}
