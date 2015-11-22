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

#ifndef HEADER_MAPREDUCE_H
    #define HEADER_MAPREDUCE_H

    #include "common.h"
    #include "args.h"

    /**
     * @struct mapreduce_s
     * @brief  Structure containing information to manage mapreduce operations.
     */
    typedef struct mapreduce_s {
        void          (*map)();     /**<  Pointer to impl. of map function    */
        void          (*reduce)();  /**<  Pointer to impl. of reduce function */
        void          (*delete)();  /**<  Pointer to impl. of delete function */
        char*         file_path;    /**<  Path to the file to open            */
        mr_type       type;         /**<  Type of mapreduce (see common.h)    */
        bool          quiet;        /**<  Display every details               */
        bool          profiling;    /**<  Profiling mode                      */
        unsigned int  nb_threads;   /**<  Number of thread worker used        */
        Timer         timer_map;    /**<  Timer for map [Profiling mode]      */
        Timer         timer_reduce; /**<  Timer for reduce [Profiling mode]   */
        Timer         timer_global; /**<  Global Timer [Profiling mode]       */
        void*         ext;          /**<  Pointer to additional data          */
    } Mapreduce;


    /* =========================== Static Elements ========================== */

    /**
     * Common constructor for implementations of Mapreduce. Static inline
     * definition to avoid to compile mapreduce.c when using an implemention.
     *
     * @param   file_path[in]     String containing the path to the file to read
     * @param   nb_threads[in]    Total number of threads
     * @param   type[in]          Type of mapreduce (see common.h)
     * @param   quiet[in]         Activate the quiet mode (no output)
     * @param   profiling[in]     Activate the profiling mode
     * @return  Pointer to the new mapreduce structure
     */
    static inline Mapreduce* _mr_common_create(const char *file_path,
                                       const int nb_threads, const mr_type type,
                                       const bool quiet, const bool profiling) {

        Mapreduce *mr = malloc(sizeof(Mapreduce));

        mr->file_path = malloc(strlen(file_path)+1);
        strcpy(mr->file_path, file_path);
        mr->nb_threads = nb_threads;
        mr->type = type;
        mr->quiet = quiet;

        /* Initialize variables for profiling */
        mr->profiling = profiling;
        _timer_init(&mr->timer_map, profiling);
        _timer_init(&mr->timer_reduce, profiling);
        _timer_init(&mr->timer_global, profiling);
        _timer_start(&mr->timer_global);

        return mr;
    }


    /* ============================== Prototypes ============================ */

    Mapreduce*   mr_create(Arguments*);
    void         mr_delete(Mapreduce**);

    void         mr_map(Mapreduce*);
    void         mr_reduce(Mapreduce*);
#endif
