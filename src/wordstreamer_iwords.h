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

#ifndef HEADER_MAPREDUCE_WORDSTREAMER_IWORDS_H
    #define HEADER_MAPREDUCE_WORDSTREAMER_IWORDS_H

    #include "wordstreamer.h"

    /* ============================== Prototypes ============================ */

    Wordstreamer*  mr_wordstreamer_iwords_create_first(const char*,
                            const int, const fr_type, const unsigned int, bool);
    Wordstreamer*  mr_wordstreamer_iwords_create_another(const Wordstreamer*,
                                                                     const int);
    void           mr_wordstreamer_iwords_delete(Wordstreamer*);

    int            mr_wordstreamer_iwords_get(Wordstreamer*, char*);
#endif
