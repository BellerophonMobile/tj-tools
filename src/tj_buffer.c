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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tj_buffer.h"

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

#ifndef TJ_PAGE_SIZE
#define TJ_PAGE_SIZE (size_t) 1024
#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
struct tj_buffer {
  tj_buffer_byte *m_buff;
  size_t m_used;
  size_t m_n;
  char m_own;
};

//----------------------------------------------------------------------
//----------------------------------------------------------------------
tj_buffer *
tj_buffer_create(size_t initial)
{
  tj_buffer *b;
  if ((b = malloc(sizeof(tj_buffer))) == 0) {
    TJ_ERROR("No memory for tj_buffer [%zu bytes].", sizeof(tj_buffer));
    return 0;
  }

  if (initial > 0) {
    if ((b->m_buff = (tj_buffer_byte *) malloc(initial)) == 0) {
      TJ_ERROR("No memory for tj_buffer_byte[%zu bytes].", initial);
      b->m_n = 0;
    } else {
      b->m_n = initial;
    }
  } else {
    b->m_buff = 0;
    b->m_n = 0;
  }

  b->m_own = 1;
  b->m_used = 0;

  TJ_LOG("Buffer[%zu] created.", initial);
  return b;
  // end tj_buffer_create
}

void
tj_buffer_finalize(tj_buffer *x)
{
  if (x->m_own && x->m_buff != 0)
    free(x->m_buff);

  TJ_LOG("Buffer[%zu] finalized.", x->m_n);
  free(x);
  // end tj_buffer_finalize
}

void
tj_buffer_setOwnership(tj_buffer *b, char own)
{
  b->m_own = own;
  // end tj_buffer_setOwnership
}

void
tj_buffer_reset(tj_buffer *b)
{
  b->m_used = 0;
  TJ_LOG("Reset; buffer[%zu/%zu].", b->m_used, b->m_n);
  // end tj_buffer_reset
}

inline
size_t
tj_buffer_getUsed(tj_buffer *b)
{
  return b->m_used;
  // end tj_buffer_getUsed
}

inline
size_t
tj_buffer_getAllocated(tj_buffer *b)
{
  return b->m_n;
  // end tj_buffer_getAllocated
}

inline
tj_buffer_byte *
tj_buffer_getBytes(tj_buffer *b)
{
  return b->m_buff;
  // end tj_buffer_getBytes
}

inline
char *
tj_buffer_getAsString(tj_buffer *b)
{
  return (char *) b->m_buff;
  // end tj_buffer_getBytes
}

inline
tj_buffer_byte *
tj_buffer_getBytesAtIndex(tj_buffer *b, size_t i)
{
  return b->m_buff+i;
  // end tj_buffer_getBytesAtIndex
}

int
tj_buffer_append(tj_buffer *b, const tj_buffer_byte *data, size_t n)
{
  tj_buffer_byte *ot;
  if (b->m_used + n > b->m_n) {
    if ((b->m_buff = (tj_buffer_byte *) realloc(ot=b->m_buff,
                                               b->m_used+n)) == 0) {
      TJ_ERROR("Could not increase buffer from %zu to %zu.", b->m_n, b->m_used+n);
      b->m_buff = ot;
      return 0;
    }
    b->m_n = b->m_used + n;
  }

  memcpy(&b->m_buff[b->m_used], data, n);
  b->m_used += n;

  TJ_LOG("Appended %zu bytes; buffer[%zu/%zu].", n, b->m_used, b->m_n);
  return 1;
  // end tj_buffer_append
}

int
tj_buffer_appendBuffer(tj_buffer *b, const tj_buffer *s)
{
  return tj_buffer_append(b, s->m_buff, s->m_used);
  // end tj_buffer_appendBuffer
}

int
tj_buffer_appendString(tj_buffer *b, const char *str)
{
  int n = strlen(str)+1;

  tj_buffer_byte *ot;
  if (b->m_used + n > b->m_n) {
    if ((b->m_buff = (tj_buffer_byte *) realloc(ot=b->m_buff,
                                               b->m_used+n)) == 0) {
      TJ_ERROR("Could not increase buffer from %zu to %zu.", b->m_n, b->m_used+n);
      b->m_buff = ot;
      return 0;
    }
    b->m_n = b->m_used + n;
  }

  memcpy(&b->m_buff[b->m_used], str, n);
  b->m_used += n;

  TJ_LOG("Appended %zu bytes from string; buffer[%zu/%zu].",
	 (size_t) n, (size_t) b->m_used, (size_t) b->m_n);
  return 1;
  // end tj_buffer_appendString
}


int
tj_buffer_appendAsStringN(tj_buffer *b, const char *str, size_t n)
{

  // Buffer is empty, so add space for null, otherwise replace
  if (b->m_used == 0)
    n++;

  tj_buffer_byte *ot;
  if (b->m_used + n > b->m_n) {
    if ((b->m_buff = (tj_buffer_byte *) realloc(ot=b->m_buff,
                                                b->m_used+n)) == 0) {
      TJ_ERROR("Could not increase buffer from %zu to %zu.",
               b->m_n, b->m_used+n);
      b->m_buff = ot;
      return 0;
    }
    b->m_n = b->m_used + n;
  }

  if (b->m_used == 0)
    memcpy(b->m_buff, str, n);
  else
    memcpy(&b->m_buff[b->m_used-1], str, n+1);
  b->m_used += n;


  TJ_LOG("Appended as string %zu bytes from string; buffer[%zu/%zu].", n, b->m_used, b->m_n);
  return 1;
  // end tj_buffer_appendAsStringN
}

int
tj_buffer_appendAsString(tj_buffer *b, const char *str)
{

  return tj_buffer_appendAsStringN(b, str, strlen(str));
  // end tj_buffer_appendAsString
}


int
tj_buffer_appendAsStringBackslashEscaped(tj_buffer *b,
                                         const char *str,
                                         const char *escape)
{
  int res, found;
  size_t start, ptr, e;
  char rep[]="\\ ";

  start = 0;
  while (str[start] != 0) {
    ptr = start;
    found = 0;
    while (str[ptr] != 0 && !found) {
      for (e = 0; escape[e] != 0; e++) {
        if (str[ptr] == escape[e]) {
          found = 1;
          break;
        }
      }
      if (!found)
        ptr++;
    }

    if ((res = tj_buffer_appendAsStringN(b, &str[start], ptr-start)) == 0)
      return 0;
    if (found) {
      rep[1] = escape[e];
      if ((res = tj_buffer_appendAsStringN(b, rep, 2)) == 0)
        return 0;
      ptr++;
    }

    start = ptr;
    // end looping overall string
  }


  return 1;
  // end tj_buffer_appendAsStringBackslashEscaped
}

int
tj_buffer_printf(tj_buffer *b, const char *fmt, ...)
{
  int res;

  va_list ap;
  va_start(ap, fmt);

  res = tj_buffer_vaprintf(b, fmt, ap);

  va_end(ap);

  return res;
  // end tj_buffer_printf
}

int
tj_buffer_vaprintf(tj_buffer *b, const char *fmt, va_list ap)
{
  va_list cp;
  int n, t;

  int err = 1;
  tj_buffer_byte *ot;

  while (1) {
    va_copy(cp, ap); // Don't do on Windows?  See utstring.

    if (b->m_used == 0)
      n = vsnprintf((char *) b->m_buff, t=b->m_n,
                    fmt, cp);
    else
      n = vsnprintf((char *) &b->m_buff[b->m_used-1], t=(b->m_n-(b->m_used-1)),
                    fmt, cp);

    va_end(cp);

    //-- The string fit into the given space
    if (n > -1 && n < t) {
      b->m_used += n + ((b->m_used) ? 0 : 1);
      TJ_LOG("Printed %d bytes to buffer[%zu/%zu]; fmt '%s'.",
             n, b->m_n, b->m_used, fmt);
      break;
    }


    if (n > -1) {

      //-- Reallocate for the calculated length
      if ((b->m_buff = (tj_buffer_byte *) realloc(ot=b->m_buff,
                                                  b->m_used + n + ((b->m_used)?0:1))) == 0) {
        TJ_ERROR("Could not increase buffer from %zu to %zu.",
                 b->m_n, b->m_used+n);
        b->m_buff = ot;
        err = 0;
        goto done;
      }

      b->m_n = b->m_used + n + ((b->m_used)?0:1);

    } else {
      TJ_ERROR("Could not vsnprintf to tj_buffer.");
      goto done;
    }
  }

 done:
  return err;

  // end tj_buffer_printf
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
int
tj_buffer_appendFileStream(tj_buffer *b, FILE *fh)
{
  size_t bytes = 0;
  tj_buffer_byte input[TJ_PAGE_SIZE];

  // The file is read in chunks rather than getting the size and
  // allocating all that memory at once so that stdin can be utilized.
  while (!feof(fh) && (bytes=fread(input, 1, TJ_PAGE_SIZE, fh)) != 0) {
    if (!tj_buffer_append(b, input, bytes)) {
      TJ_ERROR("Could not append read page[%zu/%zu].", bytes, TJ_PAGE_SIZE);
      return -1;
    }
  }

  return 1;
  // end ontonet_util_readFile
}

int
tj_buffer_appendFile(tj_buffer *b, const char *filename)
{
  FILE *fp;
  if ((fp = fopen(filename, "rb")) == 0) {
    TJ_ERROR("Could not open file %s for read.", filename);
  }

  if (!tj_buffer_appendFileStream(b, fp)) {
    fclose(fp);
    TJ_ERROR("Could not append file to buffer.");
    return 0;
  }

  fclose(fp);

  return 1;
  // end tj_buffer_appendFile
}

void
tj_buffer_popFront(tj_buffer *b, size_t n)
{
    if (n >= b->m_used) {
        b->m_used = 0;
    } else {
        memmove(b->m_buff, b->m_buff + n, b->m_used - n);
        b->m_used -= n;
    }
}

void
tj_buffer_popBack(tj_buffer *b, size_t n)
{
    if (n >= b->m_used) {
        b->m_used = 0;
    } else {
        b->m_used -= n;
    }
}

void
tj_buffer_strip(tj_buffer *b, int (*func)(int c))
{
    if (b->m_used == 0) {
        return;
    }

    size_t leading;
    for (leading = 0; leading < b->m_used; leading++) {
        if (func(b->m_buff[leading]) == 0) {
            break;
        }
    }

    tj_buffer_popFront(b, leading);

    if (leading == b->m_used) {
        return;
    }

    size_t trailing;
    for (trailing = b->m_used - 1; trailing > 0; trailing--) {
        if (func(b->m_buff[trailing]) == 0) {
            break;
        }
    }

    tj_buffer_popBack(b, b->m_used - trailing - 1);
}
