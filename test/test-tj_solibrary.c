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

#include "tj_solibrary.h"

typedef int (*print_func)(const char *format, ...);

static const char *LIB_PATH = "/usr/lib/libc.so.6";

static void setup(void **state) {
    tj_solibrary *so = tj_solibrary_create();
    assert_non_null(so);

    *state = (void*)so;
}

#include <stdio.h>

static void teardown(void **state) {
    tj_solibrary *so = *state;

    if (so != NULL) {
        tj_solibrary_finalize(so);
    }
}

static void test_1(void **state) {
    tj_solibrary *so = *state;

    tj_solibrary_entry *entry = tj_solibrary_load(so, LIB_PATH);
    assert_non_null(entry);

    print_func f = tj_solibrary_entry_getSymbol(entry, "printf");
    assert_non_null(f);

    assert_int_equal(f("Hello\n"), 6);
}

int main(int argc, char *argv[]) {
    const UnitTest tests[] = {
        unit_test_setup_teardown(test_1, setup, teardown),
    };

    return run_tests(tests);
}
