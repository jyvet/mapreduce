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
     * @brief  Structure containing information to manage mapreduce operations
     */
    typedef struct mapreduce_s {
        void          (*map)();     /**<  Pointer to impl. of map function    */
        void          (*reduce)();  /**<  Pointer to impl. of reduce function */
        void          (*delete)();  /**<  Pointer to impl. of delete function */
        char*         file_path;    /**<  Path to the file to open            */
        int           type;         /**<  Type of mapreduce (see common.h)    */
        bool          quiet;        /**<  Display every details               */
        bool          profiling;    /**<  Profiling mode                      */
        unsigned int  nb_threads;   /**<  Number of thread worker used        */
        Timer         timer_map;    /**<  Timer for map [Profiling mode]      */
        Timer         timer_reduce; /**<  Timer for reduce [Profiling mode]   */
        Timer         timer_global; /**<  Global Timer [Profiling mode]       */
        void*         ext;          /**<  Pointer to additional data          */
    } Mapreduce;


    /* ============================== Prototypes ============================ */

    Mapreduce*   mr_common_create(const char *, const int, const int,
                                                        const bool, const bool);
    Mapreduce*   mr_create(Arguments*);
    Mapreduce*   _mr_create(const char*, const int, const int, const int,
                                                        const bool, const bool);
    void         mr_delete(Mapreduce**);
    void         mr_map(Mapreduce*);
    void         mr_reduce(Mapreduce*);
#endif
