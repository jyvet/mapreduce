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

#ifndef HEADER_MAPREDUCE_ARGS_H
    #define HEADER_MAPREDUCE_ARGS_H

    #include "common.h"

    /**
     * @struct arguments_s
     * @brief  Structure which holds all provided arguments.
     */
    typedef struct arguments_s {
        char*        file_path;        /**<  Path to the file to open         */
        unsigned int nb_threads;       /**<  Number of threads to use         */
        unsigned int read_buffer_size; /**<  Size in bytes of the read buffer */
        bool         profiling;        /**<  Profiling mode                   */
        bool         quiet;            /**<  Display every details            */
        fr_type      freader_type;     /**<  Type of filereader (see common.h)*/
        ws_type      wstreamer_type;   /**<  Type of wordstreamer (common.h)  */
        mr_type      type;             /**<  Type of mapreduce (see common.h) */
    } Arguments;


    /* ============================== Prototypes ============================ */

    Arguments*  mr_args_retrieve(int, char**);
    Arguments*  mr_args_create(int, char**);
    void        mr_args_delete(Arguments**);

    void        _parse_arguments(int, char**, Arguments*);
    void        _check_arguments(Arguments*);
    void        _print_verbose(Arguments*);
#endif
