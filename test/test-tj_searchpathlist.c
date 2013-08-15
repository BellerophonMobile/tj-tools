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

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cmocka.h"

#include "tj_searchpathlist.h"

static const int MAX_PATH_LEN = 256;

static void setup(void **state) {
    tj_searchpathlist *s = tj_searchpathlist_create();
    assert_non_null(s);

    *state = (void*)s;
}

static void teardown(void **state) {
    tj_searchpathlist *s = *state;

    if (s != NULL) {
        tj_searchpathlist_finalize(s);
    }
}

static void test_empty(void **state) {
    tj_searchpathlist *s = *state;

    char result[MAX_PATH_LEN];
    memset(result, 0, MAX_PATH_LEN);

    assert_false(tj_searchpathlist_locate(s, "EMPTY", result, MAX_PATH_LEN));
    assert_string_equal(result, "");
}

static void test_relative1(void **state) {
    tj_searchpathlist *s = *state;

    char result[MAX_PATH_LEN];
    memset(result, 0, MAX_PATH_LEN);

    assert_true(tj_searchpathlist_add(s, "."));

    assert_true(tj_searchpathlist_locate(s, "README.md",
                result, MAX_PATH_LEN));
    assert_string_equal(result, "./README.md");
}

static void test_relative2(void **state) {
    tj_searchpathlist *s = *state;

    char result[MAX_PATH_LEN];
    memset(result, 0, MAX_PATH_LEN);

    assert_true(tj_searchpathlist_add(s, "."));
    assert_true(tj_searchpathlist_add(s, "src"));

    assert_true(tj_searchpathlist_locate(s, "README.md",
                result, MAX_PATH_LEN));
    assert_string_equal(result, "./README.md");
}

static void test_relative3(void **state) {
    tj_searchpathlist *s = *state;

    char result[MAX_PATH_LEN];
    memset(result, 0, MAX_PATH_LEN);

    assert_true(tj_searchpathlist_add(s, "src"));
    assert_true(tj_searchpathlist_add(s, "."));

    assert_true(tj_searchpathlist_locate(s, "README.md",
                result, MAX_PATH_LEN));
    assert_string_equal(result, "./README.md");
}

static void test_relative4(void **state) {
    tj_searchpathlist *s = *state;

    char result[MAX_PATH_LEN];
    memset(result, 0, MAX_PATH_LEN);

    assert_true(tj_searchpathlist_add(s, "."));
    assert_true(tj_searchpathlist_add(s, "src"));

    assert_true(tj_searchpathlist_locate(s, "tj_searchpathlist.c",
                result, MAX_PATH_LEN));
    assert_string_equal(result, "src/tj_searchpathlist.c");
}

static void test_relative5(void **state) {
    tj_searchpathlist *s = *state;

    char result[MAX_PATH_LEN];
    memset(result, 0, MAX_PATH_LEN);

    assert_true(tj_searchpathlist_add(s, "src"));
    assert_true(tj_searchpathlist_add(s, "."));

    assert_true(tj_searchpathlist_locate(s, "tj_searchpathlist.c",
                result, MAX_PATH_LEN));
    assert_string_equal(result, "src/tj_searchpathlist.c");
}

int main(int argc, char *argv[]) {
    const UnitTest tests[] = {
        unit_test_setup_teardown(test_empty, setup, teardown),
        unit_test_setup_teardown(test_relative1, setup, teardown),
        unit_test_setup_teardown(test_relative2, setup, teardown),
        unit_test_setup_teardown(test_relative3, setup, teardown),
        unit_test_setup_teardown(test_relative4, setup, teardown),
        unit_test_setup_teardown(test_relative5, setup, teardown),
    };

    return run_tests(tests);
}
