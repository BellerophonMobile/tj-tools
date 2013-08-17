/*
 * Copyright (c) 2013 Joe Kopena <tjkopena@gmail.com>
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

#define TJ_UTIL_ABORT(msg) CRITICAL(msg), abort()

/**
 * Convenience macro to malloc, and check the result, aborting on failure.
 *
 * You should have the TAG macro defined, this uses tj_log to print errors.
 */
#define TJ_MALLOC(type) \
  ((calloc(1, sizeof(type))) ? : \
   (TJ_UTIL_ABORT("Could not allocate " #type "."), NULL))

// E.g.: tj_kb_naivesqlite *naive;
//       TJ_CALLOC(tj_kb_naivesqlite, naive, goto error);

/**
 * Convenience macro to calloc, and check the result.
 *
 * Jumps to "error" label on error.
 *
 * You should have the TAG macro defined, this uses tj_log to print errors.
 *
 * \code{.c}
 * struct foo *foo_new() {
 *     struct foo *bar;
 *     TJ_CALLOC(bar, goto error);
 *
 *     return bar;
 *
 * error:
 *     return NULL
 * }
 * \endcode
 */
#define TJ_CALLOC(var) do { \
    if (((var) = calloc(1, sizeof(*(var)))) == NULL) { \
        CRITICAL("Could not allocate " #var "."); \
        goto error; \
    } \
} while (0)

/**
 * Convenience macro to strdup, checking the result.
 *
 * Jumps to "error" label on error.
 */
#define TJ_STRDUP(var, str) do { \
    if (((var) = strdup(str)) == NULL) { \
        CRITICAL("Could not duplicate " #str "."); \
        goto error; \
    } \
} while (0)

/**
 * Convenience macro to strndup, checking the result.
 *
 * Jumps to "error" label on error.
 */
#define TJ_STRNDUP(var, str, n) do { \
    if (((var) = strndup(str, n)) == NULL) { \
        CRITICAL("Could not duplicate " #str "."); \
        goto error; \
    } \
} while (0)

#define TJ_CHECKMEM(exp) do { \
    if ((exp) == NULL) { \
        CRITICAL(#exp " returned NULL"); \
        goto error; \
    } \
} while (0)

#endif // __tj_util_h__
