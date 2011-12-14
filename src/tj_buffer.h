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

#ifndef __tj_buffer_h__
#define __tj_buffer_h__

#include <stdio.h>

// #include <uthash.h>

//----------------------------------------------------------------------
//----------------------------------------------------------------------
typedef unsigned char           tj_buffer_byte;
typedef struct tj_buffer        tj_buffer;

/*
 * Create a tj_buffer.  Data can be added to a tj_buffer and it will
 * grow, if possible, to accommodate.  The buffer can then be reset
 * and the memory reused.  Note that none of the tj_buffer operations
 * check if the passed tj_buffer * is null.  Operation on buffers
 * created statically are also undefined.  All operations assume the
 * structure has been created using tj_buffer_create.
 *
 * \param n The initial buffer size; can be 0.  If you write directly
 * into the buffer rather than use tj_buffer_append, it must be the
 * total length.
 */
tj_buffer *
tj_buffer_create(size_t initial);

/*
 * Destroys a buffer and frees its memory.  Behavior of any future
 * calls on the buffer are undefined, but will probably segfault.
 *
 * \param x The buffer to deallocate.
 */
void
tj_buffer_finalize(tj_buffer *x);

/*
 * Reset the buffer but do not release the memory.  Future calls to
 * tj_buffer_append() overwrite previous contents but reuse the
 * current memory allocation.
 *
 * \param b The buffer to operate on.
 */
void
tj_buffer_reset(tj_buffer *b);

/*
 * Get the currently used extent of the buffer.
 *
 * \param b The buffer to operate on.
 * 
 * \return The number of bytes consumed by data since creation or the
 * last reset.
 */
size_t
tj_buffer_getUsed(tj_buffer *b);

/*
 * Get how much memory is currently allocated for the buffer.
 * 
 * \param b The buffer to operate on.
 *
 * \return The current total internal memory allocation for the
 * buffer.
 */
size_t
tj_buffer_getAllocated(tj_buffer *b);

/*
 * Get a pointer to the internal byte array.
 *
 * \param b The buffer to operate on.
 *
 * \return
 */
tj_buffer_byte *
tj_buffer_getBytes(tj_buffer *b);

/*
 * Get a pointer to the internal byte array as a string.  Same as
 * (char *) tj_buffer_getBytes(b).
 *
 * \param b The buffer to operate on.
 *
 * \return
 */
char *
tj_buffer_getAsString(tj_buffer *b);

/*
 * Get a pointer to the internal byte array from a given position.
 * This is no different from tj_buffer_getBytes(b)+i.
 *
 * \param b The buffer to operate on.
 *
 * \return
 */
tj_buffer_byte *
tj_buffer_getBytesAtIndex(tj_buffer *b, size_t i);

/*
 * Write data into a buffer, growing its memory allocation if
 * necessary.  The new data is pushed onto the end of the buffer.  If
 * the internal memory allocation cannot be grown to encompass all of
 * the data, none of it is written and the previous buffer contents
 * and size are maintained.
 *
 * \param b The buffer to operate on.
 * \param data A byte array of at least length n.
 * \param n The number of bytes from data to add to b.
 *
 * \return 0 on failure, 1 otherwise.
 */
int
tj_buffer_append(tj_buffer *b, tj_buffer_byte *data, size_t n);

/*
 *
 *
 * \param b The buffer to operate on.
 * \param str Null terminated string.
 *
 * \return 0 on failure, 1 otherwise.
 */
int
tj_buffer_appendString(tj_buffer *b, const char *str);

/*
 *
 *
 * \param b The buffer to operate on.
 * \param str Null terminated string.
 *
 * \return 0 on failure, 1 otherwise.
 */
int
tj_buffer_appendAsString(tj_buffer *b, const char *str);


/*
 * Reads a text file into a buffer.
 *
 * \param b The buffer to operate on.
 * \param fh An open file descriptor to read from.  This can be a
 * stream such as stdin, but the entire contents will be read before
 * ontonet_util_readTextFile returns.
 * \param buff Contains the read contents.  The buffer will not shrink
 * but may grow.  The actual location of the internal data may also
 * change.  buff may be empty to begin but any previously allocated
 * memory will be used.  The actual number of content bytes may be
 * less than the length of the buffer if reused.
 *
 * \return The number of characters read.  The succeeding byte in the
 * buffer will also be set to 0.
 */
int
tj_buffer_appendFileStream(tj_buffer *b, FILE *fh);

#endif // __tj_buffer_h__
