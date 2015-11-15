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

#ifndef HEADER_MAPREDUCE_TIMER_H
    #define HEADER_MAPREDUCE_TIMER_H

    #include "common.h"
    #include <sys/time.h>

    /**
     * @struct timer_s
     * @brief  Structure containing timing information (used in Profiling mode).
     */
    typedef struct timer_s {
        long long        elapsed;      /**<  Elapsed time (in micro second) */
        struct timeval   start;        /**<  Start time                     */
        bool             profiling;    /**<  Profiling mode                 */
    } Timer;


    /* =========================== Static functions ========================= */

    /**
     * Initialize a timer structure.
     *
     * @param   timer[inout]   Pointer to the Timer structure to initialize
     * @param   profiling[in]  Activate the profiling mode
     */
    static inline void _timer_init(Timer* timer, bool profiling) {
        timer->elapsed = 0;
        timer->profiling = profiling;
    }


    /**
     * Start the timer if the profiling mode is activated.
     *
     * @param   timer[inout]  Pointer to the Timer structure
     */
    static inline void _timer_start(Timer* timer) {
        if (timer->profiling) gettimeofday(&timer->start, 0);
    }


    /**
     * Stop the timer if the profiling mode is activated and accumulate the
     * duration in the elapsed variable.
     *
     * @param   timer[inout]  Pointer to the Timer structure
     */
    static inline void _timer_stop(Timer* timer) {
        if (timer->profiling) {
            struct timeval stop;
            struct timeval start = timer->start;
            gettimeofday(&stop, 0);
            timer->elapsed += (stop.tv_sec - start.tv_sec)*1000000LL
                            + stop.tv_usec - start.tv_usec;
        }
    }


    /**
     * Display the elapsed time along with a dedicated text.
     *
     * @param   timer[in]     Pointer to the Timer structure
     * @param   disp_str[in]  String to display
     */
    static inline void _timer_print(Timer* timer, char *disp_str) {
        if (timer->profiling) {
            #if MAPREDUCE_DEFAULT_USECOLORS
                printf("\e[34m |-%s:\e[1m %0.3f ms\e[0m\n", disp_str,
                                             ((double)(timer->elapsed)/1000.0));
            #else
                printf(" |-%s: %0.3f ms\n", disp_str,
                                             ((double)(timer->elapsed)/1000.0));
            #endif
        }
    }
#endif
