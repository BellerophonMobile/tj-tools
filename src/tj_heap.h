/*
 * Copyright (c) 2011 Joe Kopena <tjkopena@gmail.com>
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

#ifndef __tj_heap_h__
#define __tj_heap_h__

#include <stddef.h>

//----------------------------------------------------------------------
//----------------------------------------------------------------------
#ifndef TJ_LOG_STREAM
#define TJ_LOG_STREAM stdout
#endif

#ifndef TJ_ERROR_STREAM
#define TJ_ERROR_STREAM stderr
#endif

#ifndef TJ_LOG
#ifdef NDEBUG
#define TJ_LOG(M, ...)
#else
#define TJ_LOG(M, ...) fprintf(TJ_LOG_STREAM, "%s: " M "\n", __FUNCTION__, ##__VA_ARGS__)
#endif // ifndef NDEBUG else
#endif // ifndef TJ_LOG

#ifndef TJ_ERRROR
#define TJ_ERROR(M, ...) fprintf(TJ_ERROR_STREAM, "[ERROR] %s:%s:%d: " M "\n", __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__)
#endif


//----------------------------------------------------------------------
//----------------------------------------------------------------------

#define TJ_HEAP_DECL(type, keytype, valuetype, comparator)              \
  typedef struct { keytype m_key;                                       \
                   valuetype m_value;                                   \
                 } type##_element;                                      \
  typedef struct { type##_element *m_array;                             \
                   size_t m_n; size_t m_used;                           \
                 } type;                                                \
  int (*type##_cmp)(keytype a, keytype b) = &comparator;                \
  type *                                                                \
  type##_create(int initial)                                            \
  { type *t;                                                            \
    if ((t = malloc(sizeof(type))) == 0) {                              \
      TJ_ERROR("Could not allocate " #type ".");                        \
      return 0;                                                         \
    }                                                                   \
    if ((t->m_array = malloc(sizeof(type##_element) * initial)) == 0) { \
      TJ_ERROR("Could not allocate " #type "_element[%d].", initial);   \
      free(t);                                                          \
      return 0;                                                         \
    }                                                                   \
    t->m_n = initial;                                                   \
    t->m_used = 0;                                                      \
  }                                                                     \
  void                                                                  \
  type##_finalize(type *x)                                              \
  {                                                                     \
    free(x->m_array);                                                   \
    free(x);                                                            \
  }                                                                     \
int\
 type##_add(type *h, keytype k, valuetype v)                         \
{\
  TJ_LOG("Adding key.");\
  if (h->m_used == h->m_n) {\
  type##_element *oa;\
  if ((h->m_array = realloc(oa=h->m_array, sizeof(type##_element) * (h->m_n*2))) == 0) { \
      TJ_ERROR("Could not reallocate " #type "_element[%d].", h->m_n*2);   \
      h->m_array = oa;\
      return 0;                                                         \
    }                                                                   \
h->m_n *= 2;\
}\
size_t newindex = h->m_used, parentindex = h->m_used/2;\
while (newindex > 0 && type##_cmp(k, h->m_array[parentindex].m_key)) {\
 TJ_LOG("n %d, p %d", newindex, parentindex);\
  h->m_array[newindex].m_key = h->m_array[parentindex].m_key;\
  h->m_array[newindex].m_value = h->m_array[parentindex].m_value;\
  newindex = parentindex;\
  parentindex = newindex/2;\
 }\
h->m_array[newindex].m_key = k;\
h->m_array[newindex].m_value = v;\
 h->m_used++;\
return 0;\
}

//----------------------------------------------
#define TJ_HEAP_IMPL(type)                                              \

#endif // __tj_heap_h__
