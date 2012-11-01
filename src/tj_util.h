/*
 * Copyright (c) 2012 Joe Kopena <tjkopena@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __tj_util_h__
#define __tj_util_h__

#include <stdlib.h>

// E.g.: tj_kb_naivesqlite *naive = TJ_ALLOC(tj_kb_naivesqlite);
#define TJ_ALLOC(type)                                                 \
  ((calloc(1, sizeof(type))) ? :                                       \
   (CRITICAL("Could not allocate " #type "."), abort(), NULL))

// E.g.: tj_kb_naivesqlite *naive;
//       TJ_CALLOC(tj_kb_naivesqlite, naive, goto error);
#define TJ_CALLOC(type, var, err)                                      \
  if ((var = calloc(1, sizeof(type))) == 0) {                          \
    CRITICAL("Could not allocate " #type ".");                         \
    err;                                                               \
  }

#define TJ_STRDUP(str)                                                 \
  ((strdup(str)) ? :                                                   \
   (CRITICAL("Could not duplicate " #str "."), abort(), NULL))

#define TJ_STRNDUP(str, n)                                             \
  ((strndup(str, n)) ? :                                               \
   (CRITICAL("Could not duplicate " #str "."), abort(), NULL))

#define TJ_CHECKMEM(exp)                                               \
  ((exp) ? : (CRITICAL(#exp " returned NULL"), abort(), NULL))

#endif // __tj_util_h__
