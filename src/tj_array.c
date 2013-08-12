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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "tj_array.h"

#ifdef UNIT_TESTING
#   undef assert
#   define assert(x) mock_assert((int)(x), #x, __FILE__, __LINE__)
#endif /* UNIT_TESTING */

struct tj_array {
    size_t count;
    size_t capacity;

    void **array;
};

static const size_t DEFAULT_LIST_SIZE = 5;

struct tj_array *tj_array_create(size_t capacity) {
    struct tj_array *array = malloc(sizeof(*array));
    if (array == NULL) {
        return NULL;
    }

    array->count = 0;
    array->capacity = capacity;
    array->array = NULL;

    if (array->capacity > 0) {
        array->array = malloc(capacity * sizeof(void*));
        if (array->array == NULL) {
            free(array);
            return NULL;
        }
    }

    return array;
}

void tj_array_finalize(struct tj_array *array) {
    if (array->array != NULL) {
        free(array->array);
    }
    free(array);
}

size_t tj_array_count(const struct tj_array *array) {
    return array->count;
}

size_t tj_array_capacity(const struct tj_array *array) {
    return array->capacity;
}

void *tj_array_get(const struct tj_array *array, size_t index) {
    assert(index < array->count);
    return array->array[index];
}

int tj_array_append(struct tj_array *array, void *item) {
    if (array->count == array->capacity) {
        if (array->array == NULL) {
            array->capacity = DEFAULT_LIST_SIZE;
        } else {
            array->capacity = (array->capacity) * 2;
        }

        void **new_array = realloc(array->array,
                array->capacity * sizeof(void*));
        if (new_array == NULL) {
            return 0;
        }
        array->array = new_array;
    }
    array->array[array->count] = item;
    array->count += 1;

    return 1;
}

void tj_array_remove(struct tj_array *array, size_t index) {
    assert(index < array->count);
    if (index < array->capacity - 1) {
        size_t remaining = (array->capacity - index - 1) * sizeof(void*);
        memmove(array->array + index, array->array + index + 1, remaining);
    }
    array->count -= 1;
}

void tj_array_removeItem(struct tj_array *array, void *item) {
    tj_array_remove(array, tj_array_find(array, item));
}

void tj_array_clear(struct tj_array *array) {
    array->count = 0;
}

ssize_t tj_array_find(const struct tj_array *array, void *item) {
    size_t i;
    for (i = 0; i < array->count; i++) {
        if (array->array[i] == item) {
            return i;
        }
    }
    return -1;
}
