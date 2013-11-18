/*
 * Copyright (c) 2013 Bellerophon Mobile, LLC.
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

/**
 * \file tj_array.h
 *
 * Provides a dynamically expanding array.
 *
 * Useful for storing pointers to data structures.
 */

#pragma once

typedef struct tj_array tj_array;

/**
 * Create a new dynamic array with some initial capacity.
 *
 * \param capacity Initial capacity, may be 0.
 */
tj_array *tj_array_create(size_t capacity);

/** Frees a dynamic array. */
void tj_array_finalize(tj_array *array);

/** Returns the number of elements in the array. */
size_t tj_array_count(const tj_array *array);

/** Returns the capacity of the array. */
size_t tj_array_capacity(const tj_array *array);

/** Returns an element in the array. */
void *tj_array_get(const tj_array *array, size_t index);

/**
 * Append an item to a dynamic array.
 *
 * If the array is full, this will resize the array.
 *
 * \return 0 on failure, 1 otherwise.
 */
int tj_array_append(tj_array *array, void *item);

/**
 * Remove an item at a particular index from a dynamic array.
 *
 * This currently does not resize the array ever.
 */
void tj_array_remove(tj_array *array, size_t index);

/**
 * Remove an item from a dynamic array.
 *
 * This currently does not resize the array ever.
 */
void tj_array_removeItem(tj_array *array, void *item);

/** Removes all items from the array. */
void tj_array_clear(tj_array *array);

/**
 * Searches an array for an element, by comparing pointers.
 *
 * \return -1 if item not found.
 */
ssize_t tj_array_find(const tj_array *array, void *item);
