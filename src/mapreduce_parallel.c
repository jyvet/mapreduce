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
 * @file mapreduce_parallel.c
 * @brief Mapreduce parallel implementation.
 * @author Jean-Yves VET
 */

#include "mapreduce.h"
#include "mapreduce_parallel.h"

/* ========================= Constructor / Destructor ======================= */

/**
 * Constructor for Mapreduce parallel.
 *
 * @param  file_path[in]    String containg the path to the file we want to read
 * @param  nb_threads[in]   Number of threads to use
 * @param  quiet[in]        Activate the quiet mode (no output)
 * @param  profiling[in]    Activate the profiling mode
 * @return  A Mapreduce structure
 */
Mapreduce* mr_parallel_create(const char *file_path,
                               const unsigned int nb_threads, const int ws_type,
                                       const bool quiet, const bool profiling) {
    Mapreduce *mr = mr_common_create(file_path, nb_threads, TYPE_PARALLEL,
                                                              quiet, profiling);

    mr->map = mr_parallel_map;
    mr->reduce = mr_parallel_reduce;
    mr->delete = mr_parallel_delete;

    Mapreduce_parallel_thread *threads =
                         malloc(nb_threads*sizeof(Mapreduce_parallel_thread));
    mr->ext = threads;

    /* First thread */
    threads[0].wordstreamer = mr_wordstreamer_create_first(file_path,
                                                nb_threads, ws_type, profiling);
    threads[0].dictionary = mr_dictionary_create(profiling);
    threads[0].thread = malloc(sizeof(pthread_t));
    assert(threads[0].thread != NULL);

    /* Next threads */
    for(int i=1; i<nb_threads; i++) {
        threads[i].wordstreamer = mr_wordstreamer_create_another(
                                                    threads[0].wordstreamer, i);
        threads[i].dictionary = mr_dictionary_create(profiling);
        threads[i].thread = malloc(sizeof(pthread_t));
        assert(threads[i].thread != NULL);
    }

    return mr;
}


/**
 * Delete Mapreduce and associated structures.
 *
 * @param   mr[in]     Pointer to a Mapreduce structure
 */
void mr_parallel_delete(Mapreduce *mr) {
    if (mr != NULL) {
        Mapreduce_parallel_thread *threads = (Mapreduce_parallel_thread *) mr->ext;
        int nb_threads =  mr->nb_threads;

        for(int i=0; i<nb_threads; i++) {
            mr_wordstreamer_delete(&threads[i].wordstreamer);
            mr_dictionary_delete(&threads[i].dictionary);
            free(threads[i].thread);
        }

        free(threads);
    }
}


/* ============================ Private functions =========================== */

/**
 * Thread function which performs map operation.
 *
 * @param   t_struct[inout]     Pointer to a struct dedicated to the thread
 */
void* _thread_map(void *t_struct) {
    Mapreduce_parallel_thread *t = (Mapreduce_parallel_thread *) t_struct;

    Dictionary *dico = t->dictionary;
    Wordstreamer *ws = t->wordstreamer;
    char word[MAPREDUCE_MAX_WORD_SIZE];

    while (!mr_wordstreamer_get(ws, word)) {
        mr_dictionary_put_word(dico, word);
    }

    return NULL;
}


/* ============================= Public functions =========================== */

/**
 * Map operation.
 *
 * @param   mr[inout]     Pointer to a Mapreduce structure
 */
void mr_parallel_map(Mapreduce *mr) {
    assert(mr != NULL);
    int nb_threads = mr->nb_threads;
    Mapreduce_parallel_thread *threads = (Mapreduce_parallel_thread *) mr->ext;

    /* Launch all threads */
    for(int i=0; i<nb_threads; i++) {
        pthread_create(threads[i].thread, NULL, _thread_map, &threads[i]);
    }

    /* Join all threads */
    for(int i=0; i<nb_threads; i++) {
        pthread_join(*threads[i].thread, NULL);
    }
}


/**
 * Map operation : Aggregate results, and display them, sorted by word.
 *
 * @param   mr[inout]     Pointer to a Mapreduce structure
 */
void mr_parallel_reduce(Mapreduce *mr) {
    assert(mr != NULL);
    int nb_threads = mr->nb_threads;
    Mapreduce_parallel_thread *threads = (Mapreduce_parallel_thread *) mr->ext;
    Dictionary *dico = threads[0].dictionary;

    for(int i=1; i<nb_threads; i++) {
        mr_dictionary_merge(dico, threads[i].dictionary);
    }

    if (!mr->quiet) mr_dictionary_display(dico);
}
