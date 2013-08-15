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

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cmocka.h"

#include "tj_heap.h"

#define INT_PEEK_POP(key, value) do { \
    assert_true(intheap_peek(heap, &k, &v)); \
    assert_int_equal(k, key); \
    assert_string_equal(v, value); \
    assert_true(intheap_pop(heap, &k, &v)); \
    assert_int_equal(k, key); \
    assert_string_equal(v, value); \
} while (0)

#define FLT_PEEK_POP(key) do { \
    assert_true(floatheap_peek(heap, &k, &v)); \
    assert_in_range(k, key, key + 0.1); \
    assert_string_equal(v, #key); \
    assert_true(floatheap_pop(heap, &k, &v)); \
    assert_in_range(k, key, key + 0.1); \
    assert_string_equal(v, #key); \
} while (0)

static int intless(int a, int b) { return a  < b; }
static int intfind(void *d, int k, char *v) { return (strcmp(v, d) == 0); }
TJ_HEAP_DECL(intheap, int, char *, intless);

static int floatmore(float a, float b) { return a  > b; }
TJ_HEAP_DECL(floatheap, float, char *, floatmore);

static void int_setup(void **state) {
    intheap *heap = intheap_create(4);
    assert_non_null(heap);

    *state = (void*)heap;
}

static void int_teardown(void **state) {
    intheap *heap = *state;
    if (heap != NULL) {
        intheap_finalize(heap);
    }
}

static void float_setup(void **state) {
    floatheap *heap = floatheap_create(4);
    assert_non_null(heap);

    *state = (void*)heap;
}

static void float_teardown(void **state) {
    floatheap *heap = *state;
    if (heap != NULL) {
        floatheap_finalize(heap);
    }
}

static void test_int_min(void **state) {
    intheap *heap = *state;

    assert_true(intheap_add(heap, 923, "h"));
    assert_true(intheap_add(heap, 467, "d"));
    assert_true(intheap_add(heap, 23, "a1"));
    assert_true(intheap_add(heap, 500, "f1"));
    assert_true(intheap_add(heap, 23, "a2"));
    assert_true(intheap_add(heap, 234, "d"));
    assert_true(intheap_add(heap, 468, "e"));
    assert_true(intheap_add(heap, 900, "g"));
    assert_true(intheap_add(heap, 90, "c"));
    assert_true(intheap_add(heap, 500, "f2"));
    assert_true(intheap_add(heap, 80, "b"));

    int k = 0;
    char *v = NULL;

    INT_PEEK_POP(23, "a1");
    INT_PEEK_POP(23, "a2");
    INT_PEEK_POP(80, "b");
    INT_PEEK_POP(90, "c");
    INT_PEEK_POP(234, "d");
    INT_PEEK_POP(467, "d");
    INT_PEEK_POP(468, "e");
    INT_PEEK_POP(500, "f1");
    INT_PEEK_POP(500, "f2");
    INT_PEEK_POP(900, "g");
    INT_PEEK_POP(923, "h");

    assert_false(intheap_peek(heap, &k, &v));
    assert_false(intheap_pop(heap, &k, &v));
}

static void test_int_find(void **state) {
    intheap *heap = *state;

    intheap_add(heap, 923, "d");
    intheap_add(heap, 467, "b");
    intheap_add(heap, 23, "a");
    intheap_add(heap, 500, "c");

    int k;
    char *v;

    k = intheap_find(heap, &intfind, "a");
    assert_int_not_equal(k, -1);
    assert_true(intheap_remove(heap, k, &k, &v));

    k = intheap_find(heap, &intfind, "c");
    assert_int_not_equal(k, -1);
    assert_true(intheap_remove(heap, k, &k, &v));

    k = intheap_find(heap, &intfind, "x");
    assert_int_equal(k, -1);
    assert_false(intheap_remove(heap, k, &k, &v));

    assert_true(intheap_pop(heap, &k, &v));
    assert_int_equal(k, 467);
    assert_string_equal(v, "b");

    assert_true(intheap_pop(heap, &k, &v));
    assert_int_equal(k, 923);
    assert_string_equal(v, "d");

    assert_false(intheap_pop(heap, &k, &v));
}

static void test_float_max(void **state) {
    floatheap *heap = *state;

    floatheap_add(heap, 2.4, "2.4");
    floatheap_add(heap, 0.3, "0.3");
    floatheap_add(heap, 0.7, "0.7");
    floatheap_add(heap, 7.8, "7.8");
    floatheap_add(heap, 4.0, "4.0");

    float k;
    char *v;

    FLT_PEEK_POP(7.8);
    FLT_PEEK_POP(4.0);
    FLT_PEEK_POP(2.4);
    FLT_PEEK_POP(0.7);
    FLT_PEEK_POP(0.3);

    assert_false(floatheap_peek(heap, &k, &v));
    assert_false(floatheap_pop(heap, &k, &v));
}

int main(int argc, char *argv[0]) {
    const UnitTest tests[] = {
        unit_test_setup_teardown(test_int_min, int_setup, int_teardown),
        unit_test_setup_teardown(test_int_find, int_setup, int_teardown),
        unit_test_setup_teardown(test_float_max, float_setup, float_teardown),
    };

    return run_tests(tests);
}
