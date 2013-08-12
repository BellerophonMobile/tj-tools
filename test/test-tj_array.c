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

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cmocka.h"

#define UNIT_TESTING
#include "tj_array.c"

/* chosen by fair dice roll, guaranteed to be random. */
#define VALUE_A 4
#define VALUE_B 8
#define VALUE_C 15
#define VALUE_D 16

static void setup(void **state) {
    struct tj_array *array = tj_array_create(0);
    assert_non_null(array);

    *state = (void*)array;
}

static void teardown(void **state) {
    struct tj_array *array = *state;
    if (array != NULL) {
        tj_array_finalize(array);
    }
}

static void init_array(struct tj_array *array, int *a, int *b, int *c,
                       int *d) {
    *a = VALUE_A;
    *b = VALUE_B;
    *c = VALUE_C;
    *d = VALUE_D;

    assert_true(tj_array_append(array, a));
    assert_true(tj_array_append(array, b));
    assert_true(tj_array_append(array, c));
    assert_true(tj_array_append(array, d));
    assert_int_equal(tj_array_count(array), 4);
}

static void test_array_empty(void **state) {
    struct tj_array *array = tj_array_create(0);

    assert_int_equal(tj_array_count(array), 0);
    assert_int_equal(tj_array_capacity(array), 0);
    expect_assert_failure(tj_array_get(array, 0));
    expect_assert_failure(tj_array_remove(array, 0));

    tj_array_finalize(array);
}

static void test_array_preallocated(void **state) {
    struct tj_array *array = tj_array_create(11);

    assert_int_equal(tj_array_count(array), 0);
    assert_int_equal(tj_array_capacity(array), 11);
    expect_assert_failure(tj_array_get(array, 0));
    expect_assert_failure(tj_array_remove(array, 0));

    int a = VALUE_A;
    tj_array_append(array, &a);
    assert_int_equal(tj_array_count(array), 1);
    assert_int_equal(tj_array_capacity(array), 11);
    assert_int_equal(*(int*)tj_array_get(array, 0), VALUE_A);

    tj_array_finalize(array);
}

static void test_array_append_get(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    assert_int_equal(*(int*)tj_array_get(array, 0), VALUE_A);
    assert_int_equal(*(int*)tj_array_get(array, 1), VALUE_B);
    assert_int_equal(*(int*)tj_array_get(array, 2), VALUE_C);
    assert_int_equal(*(int*)tj_array_get(array, 3), VALUE_D);
    expect_assert_failure(tj_array_get(array, 4));
}

static void test_array_remove1(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    tj_array_remove(array, 0);

    assert_int_equal(*(int*)tj_array_get(array, 0), VALUE_B);
    assert_int_equal(*(int*)tj_array_get(array, 1), VALUE_C);
    assert_int_equal(*(int*)tj_array_get(array, 2), VALUE_D);
    expect_assert_failure(tj_array_get(array, 3));
}

static void test_array_remove2(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    tj_array_remove(array, 1);

    assert_int_equal(*(int*)tj_array_get(array, 0), VALUE_A);
    assert_int_equal(*(int*)tj_array_get(array, 1), VALUE_C);
    assert_int_equal(*(int*)tj_array_get(array, 2), VALUE_D);
    expect_assert_failure(tj_array_get(array, 3));
}

static void test_array_remove3(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    tj_array_remove(array, 2);

    assert_int_equal(*(int*)tj_array_get(array, 0), VALUE_A);
    assert_int_equal(*(int*)tj_array_get(array, 1), VALUE_B);
    assert_int_equal(*(int*)tj_array_get(array, 2), VALUE_D);
    expect_assert_failure(tj_array_get(array, 3));
}

static void test_array_remove4(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    tj_array_remove(array, 3);

    assert_int_equal(*(int*)tj_array_get(array, 0), VALUE_A);
    assert_int_equal(*(int*)tj_array_get(array, 1), VALUE_B);
    assert_int_equal(*(int*)tj_array_get(array, 2), VALUE_C);
    expect_assert_failure(tj_array_get(array, 3));
}

static void test_array_remove5(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    tj_array_remove(array, 0);
    tj_array_append(array, &a);

    assert_int_equal(*(int*)tj_array_get(array, 0), VALUE_B);
    assert_int_equal(*(int*)tj_array_get(array, 1), VALUE_C);
    assert_int_equal(*(int*)tj_array_get(array, 2), VALUE_D);
    assert_int_equal(*(int*)tj_array_get(array, 3), VALUE_A);
    expect_assert_failure(tj_array_get(array, 4));
}

static void test_array_remove6(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    tj_array_remove(array, 3);
    tj_array_append(array, &a);

    assert_int_equal(*(int*)tj_array_get(array, 0), VALUE_A);
    assert_int_equal(*(int*)tj_array_get(array, 1), VALUE_B);
    assert_int_equal(*(int*)tj_array_get(array, 2), VALUE_C);
    assert_int_equal(*(int*)tj_array_get(array, 3), VALUE_A);
    expect_assert_failure(tj_array_get(array, 4));
}

static void test_array_remove7(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    tj_array_remove(array, 3);
    tj_array_append(array, &d);

    assert_int_equal(*(int*)tj_array_get(array, 0), VALUE_A);
    assert_int_equal(*(int*)tj_array_get(array, 1), VALUE_B);
    assert_int_equal(*(int*)tj_array_get(array, 2), VALUE_C);
    assert_int_equal(*(int*)tj_array_get(array, 3), VALUE_D);
    expect_assert_failure(tj_array_get(array, 4));
}

static void test_array_remove8(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    tj_array_remove(array, 0);
    tj_array_remove(array, 0);
    tj_array_remove(array, 0);
    tj_array_remove(array, 0);
    assert_int_equal(tj_array_count(array), 0);
    expect_assert_failure(tj_array_remove(array, 0));
    expect_assert_failure(tj_array_get(array, 0));
}

static void test_array_find1(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    int e = 0;

    assert_int_equal(tj_array_find(array, &a), 0);
    assert_int_equal(tj_array_find(array, &b), 1);
    assert_int_equal(tj_array_find(array, &c), 2);
    assert_int_equal(tj_array_find(array, &d), 3);

    assert_int_equal(tj_array_find(array, &e), -1);
}

static void test_array_find2(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    tj_array_remove(array, 0);

    assert_int_equal(tj_array_find(array, &b), 0);
    assert_int_equal(tj_array_find(array, &c), 1);
    assert_int_equal(tj_array_find(array, &d), 2);
}

static void test_array_find3(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    tj_array_remove(array, 0);
    tj_array_append(array, &a);

    assert_int_equal(tj_array_find(array, &b), 0);
    assert_int_equal(tj_array_find(array, &c), 1);
    assert_int_equal(tj_array_find(array, &d), 2);
    assert_int_equal(tj_array_find(array, &a), 3);
}

static void test_array_find4(void **state) {
    struct tj_array *array = (struct tj_array*)*state;
    int a, b, c, d;
    init_array(array, &a, &b, &c, &d);

    tj_array_remove(array, 3);
    tj_array_append(array, &d);

    assert_int_equal(tj_array_find(array, &a), 0);
    assert_int_equal(tj_array_find(array, &b), 1);
    assert_int_equal(tj_array_find(array, &c), 2);
    assert_int_equal(tj_array_find(array, &d), 3);
}

static void test_array_find5(void **state) {
    struct tj_array *array = (struct tj_array*)*state;

    int a = VALUE_A;
    int b = VALUE_B;
    int c = VALUE_C;
    int d = VALUE_D;

    tj_array_append(array, &a);
    tj_array_append(array, &a);
    tj_array_append(array, &b);
    tj_array_append(array, &a);

    assert_int_equal(tj_array_find(array, &a), 0);
    assert_int_equal(tj_array_find(array, &b), 2);
    assert_int_equal(tj_array_find(array, &c), -1);
    assert_int_equal(tj_array_find(array, &d), -1);
}

int main(int argc, char **argv) {
    const UnitTest tests[] = {
        unit_test(test_array_empty),
        unit_test(test_array_preallocated),
        unit_test_setup_teardown(test_array_append_get, setup, teardown),
        unit_test_setup_teardown(test_array_remove1, setup, teardown),
        unit_test_setup_teardown(test_array_remove2, setup, teardown),
        unit_test_setup_teardown(test_array_remove3, setup, teardown),
        unit_test_setup_teardown(test_array_remove4, setup, teardown),
        unit_test_setup_teardown(test_array_remove5, setup, teardown),
        unit_test_setup_teardown(test_array_remove6, setup, teardown),
        unit_test_setup_teardown(test_array_remove7, setup, teardown),
        unit_test_setup_teardown(test_array_remove8, setup, teardown),
        unit_test_setup_teardown(test_array_find1, setup, teardown),
        unit_test_setup_teardown(test_array_find2, setup, teardown),
        unit_test_setup_teardown(test_array_find3, setup, teardown),
        unit_test_setup_teardown(test_array_find4, setup, teardown),
        unit_test_setup_teardown(test_array_find5, setup, teardown),
    };
    return run_tests(tests);
}
