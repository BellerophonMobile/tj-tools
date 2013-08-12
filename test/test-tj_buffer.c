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

#include "tj_buffer.h"

static char *argv0 = "";

static void setup(void **state) {
    tj_buffer *b = tj_buffer_create(0);
    assert_non_null(b);
    *state = (void*)b;
}

static void teardown(void **state) {
    tj_buffer *b = *state;
    if (b != NULL) {
        tj_buffer_finalize(b);
    }
}

static void test_append1(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_append(b, (tj_buffer_byte*)"HELLO", 5));

    assert_int_equal(tj_buffer_getAllocated(b), 5);
    assert_int_equal(tj_buffer_getUsed(b), 5);
}

static void test_append2(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_append(b, (tj_buffer_byte*)"HELLO", 5));
    assert_true(tj_buffer_append(b, (tj_buffer_byte*)"HELLO", 5));

    assert_int_equal(tj_buffer_getAllocated(b), 10);
    assert_int_equal(tj_buffer_getUsed(b), 10);
}

static void test_appendString(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_append(b, (tj_buffer_byte*)"HELLO", 5));
    assert_true(tj_buffer_append(b, (tj_buffer_byte*)"HELLO", 5));
    assert_true(tj_buffer_appendString(b, "HELLO"));

    assert_int_equal(tj_buffer_getAllocated(b), 16);
    assert_int_equal(tj_buffer_getUsed(b), 16);
    assert_string_equal((char*)tj_buffer_getBytes(b), "HELLOHELLOHELLO");
}

static void test_reset1(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_append(b, (tj_buffer_byte*)"HELLO", 5));
    assert_true(tj_buffer_append(b, (tj_buffer_byte*)"HELLO", 5));
    assert_true(tj_buffer_appendString(b, "HELLO"));

    tj_buffer_reset(b);

    assert_int_equal(tj_buffer_getAllocated(b), 16);
    assert_int_equal(tj_buffer_getUsed(b), 0);
}

static void test_reset2(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_append(b, (tj_buffer_byte*)"HELLO", 5));
    assert_true(tj_buffer_append(b, (tj_buffer_byte*)"HELLO", 5));
    assert_true(tj_buffer_appendString(b, "HELLO"));

    tj_buffer_reset(b);

    assert_true(tj_buffer_appendAsString(b, "HELLO"));
    assert_true(tj_buffer_appendAsString(b, "HELLO"));

    assert_int_equal(tj_buffer_getAllocated(b), 16);
    assert_int_equal(tj_buffer_getUsed(b), 11);
    assert_string_equal(tj_buffer_getAsString(b), "HELLOHELLO");
}

static void test_fileStream1(void **state) {
    tj_buffer *b = *state;

    FILE *f = fopen("test/data/mushi", "r");
    assert_non_null(f);
    assert_true(tj_buffer_appendFileStream(b, f));
    fclose(f);

    assert_true(tj_buffer_appendString(b, ""));
    assert_string_equal(tj_buffer_getAsString(b), "MUSHI");
}

static void test_fileStream2(void **state) {
    tj_buffer *b = *state;
    FILE *f;

    f = fopen("test/data/mushi", "r");
    assert_non_null(f);
    assert_true(tj_buffer_appendFileStream(b, f));
    fclose(f);

    f = fopen("test/data/mushi", "r");
    assert_non_null(f);
    assert_true(tj_buffer_appendFileStream(b, f));
    fclose(f);

    f = fopen("test/data/mushi", "r");
    assert_non_null(f);
    assert_true(tj_buffer_appendFileStream(b, f));
    fclose(f);

    f = fopen("test/data/mushi", "r");
    assert_non_null(f);
    assert_true(tj_buffer_appendFileStream(b, f));
    fclose(f);

    assert_true(tj_buffer_appendString(b, ""));
    assert_string_equal(tj_buffer_getAsString(b), "MUSHIMUSHIMUSHIMUSHI");
}

static void test_fileStream3(void **state) {
    tj_buffer *b = *state;

    FILE *f = fopen(argv0, "rb");
    assert_non_null(f);

    fseek(f, 0L, SEEK_END);
    size_t flen = ftell(f);
    fseek(f, 0L, SEEK_SET);

    assert_true(tj_buffer_appendFileStream(b, f));
    fclose(f);

    assert_int_equal(tj_buffer_getUsed(b), flen);
}

static void test_appendFile(void **state) {
    tj_buffer *b = *state;

    FILE *f = fopen(argv0, "rb");
    assert_non_null(f);

    fseek(f, 0L, SEEK_END);
    size_t flen = ftell(f);
    fseek(f, 0L, SEEK_SET);
    fclose(f);

    assert_true(tj_buffer_appendFile(b, argv0));

    assert_int_equal(tj_buffer_getUsed(b), flen);
}

int main(int argc, char *argv[]) {
    if (argc > 0) {
        argv0 = argv[0];
    }

    const UnitTest tests[] = {
        unit_test_setup_teardown(test_append1, setup, teardown),
        unit_test_setup_teardown(test_append2, setup, teardown),
        unit_test_setup_teardown(test_appendString, setup, teardown),
        unit_test_setup_teardown(test_reset1, setup, teardown),
        unit_test_setup_teardown(test_reset2, setup, teardown),
        unit_test_setup_teardown(test_fileStream1, setup, teardown),
        unit_test_setup_teardown(test_fileStream2, setup, teardown),
        unit_test_setup_teardown(test_fileStream3, setup, teardown),
        unit_test_setup_teardown(test_appendFile, setup, teardown),
    };

    return run_tests(tests);
}
