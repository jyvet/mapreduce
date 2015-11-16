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
 * @file args.c
 * @brief Retrieve, parse and check arguments.
 * @author Jean-Yves VET
 */

#include <argp.h>
#include "args.h"

/* ============================= Static Elements ============================ */

const char *argp_program_version = "MapReduce "MAPREDUCE_VERSION;
const char *argp_program_bug_address = "<"MAPREDUCE_CONTACT">";

/* Program documentation */
static char doc[] = "The program launches N threads to compute the number of "
    "occurrences of words in a file. The program accepts the following "
    "optional arguments:";

/* A description of the arguments we accept */
static char args_doc[] = "<file> <Nthreads>";

/* The options we understand */
static struct argp_option options[] = {
    {"profiling",  'p', 0,         0, "Activate profiling"},
    {"quiet",      'q', 0,         0, "Do not output results"},
    {"type",       't', "MR_TYPE", 0, "Mapreduce type "
                                      "(0=PARALLEL, 1=SEQUENTIAL)"},
    {"wtype",      'w', "WS_TYPE", 0, "WordStreamer type "
                                      "(0=SCATTER, 1=INTERLEAVE)"},
    { 0 }
};

/* Parse a single option */
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
    Arguments *args = state->input;
    int type = 0;

    switch (key) {
        case 'p':
            args->profiling = true;
          	break;
        case 'q':
            args->quiet = true;
            break;
        case 't':
            type = atoi(arg);
            if (type == TYPE_PARALLEL
                || type == TYPE_SEQUENTIAL) {
                args->type = type;
            }
            break;
        case 'w':
            type = atoi(arg);
            if (type == TYPE_WORDSTREAMER_SCATTER
                || type == TYPE_WORDSTREAMER_INTERLEAVE) {
                args->ws_type = type;
            }
            break;
        case ARGP_KEY_ARG:
    	   if (state->arg_num == 0) {
               args->file_path = malloc(strlen(arg)+1);
               assert(args->file_path != NULL);
               strcpy(args->file_path, arg);
           } else if (state->arg_num == 1) {
               args->nb_threads = atoi(arg);
           }
          break;
        case ARGP_KEY_END:
        	if (state->arg_num != 2) {
                argp_usage (state);
            }
        	break;
        default:
        	return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

/* Argp parser */
static struct argp argp = { options, parse_opt, args_doc, doc };


/* ============================ Private Functions =========================== */

/**
 * Parse arguments and fill an Arguments structure.
 *
 * @param   argc[in]    Number of program arguments
 * @param   argv[in]    Program arguments
 * @param   args[out]   Pointer to a new Arguments structure
 */
void _parse_arguments(int argc, char **argv, Arguments *args) {
    argp_parse (&argp, argc, argv, 0, 0, args);
}


/**
 * Check number of threads and file path.
 *
 * @param   args[in]   Arguments structure
 */
void _check_arguments(Arguments *args) {
    /* Check num threads */
    unsigned int nb_threads = args->nb_threads;

    if(nb_threads < MAPREDUCE_MIN_THREADS) {
	mr_error(ERR_MINTHREADS);
    } else if (nb_threads > MAPREDUCE_MAX_THREADS) {
	mr_error(ERR_MAXTHREADS);
    }

    /* Check file access in read mode */
    char *file_path = args->file_path;
    if (access (file_path, R_OK)) {
        mr_error(ERR_FILEACCESS);
    }
}


/* ========================= Constructor / Destructor ======================= */

/**
 * Create an Arguments structure with default values.
 *
 * @param   argc[in]    Number of program arguments
 * @param   argv[in]    Program arguments
 * @return  Arguments   Pointer to the new Arguments structure
 */
Arguments* mr_args_create(int argc, char **argv) {
    Arguments *args = malloc(sizeof(Arguments));
    assert(args != NULL);

    /* Initialize options with default values */
    args->quiet      =   MAPREDUCE_DEFAULT_QUIET;
    args->profiling  =   MAPREDUCE_DEFAULT_PROFILING;
    args->ws_type    =   MAPREDUCE_WS_DEFAULT_TYPE;
    args->type       =   MAPREDUCE_DEFAULT_TYPE;

    return args;
}


/**
 * Delete an Arguments structure.
 *
 * @param   args_ptr[in]   Poiter to pointer of an Arguments structure
 */
void mr_args_delete(Arguments** args_ptr) {
    Arguments* args = *args_ptr;

    if (args != NULL) {
        if (args->file_path != NULL) free(args->file_path);
        free(args);
    }

    *args_ptr = NULL;
}


/* ============================ Public Functions ============================ */

/**
 * Create an Arguments structure containing checked arguments.
 *
 * @param   argc[in]    Number of program arguments
 * @param   argv[in]    Program arguments
 * @return  Arguments   Pointer to the new Arguments structure
 */
Arguments* mr_args_retrieve(int argc, char **argv) {
    Arguments *args = mr_args_create(argc, argv);

    /* Parse arguments */
    _parse_arguments (argc, argv, args);

    /* Check arguments */
    _check_arguments(args);

    return args;
}
