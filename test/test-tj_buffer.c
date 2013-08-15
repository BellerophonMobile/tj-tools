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

#include <ctype.h>

#include "cmocka.h"

#include "tj_buffer.h"

static char *argv0 = "";

struct data {
    tj_buffer *a;
    tj_buffer *b;
};

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

static void setup2(void **state) {
    struct data *data = malloc(sizeof(*data));
    assert_non_null(data);

    data->a = tj_buffer_create(0);
    assert_non_null(data->a);

    data->b = tj_buffer_create(0);
    assert_non_null(data->b);

    *state = (void*)data;
}

static void teardown2(void **state) {
    struct data *data = *state;

    if (data != NULL) {
        if (data->a != NULL) {
            tj_buffer_finalize(data->a);
        }

        if (data->b != NULL) {
            tj_buffer_finalize(data->b);
        }

        free(data);
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

static void test_getAtIndex(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_appendString(b, "HELLO"));
    assert_true(tj_buffer_getBytesAtIndex(b, 3) == tj_buffer_getBytes(b) + 3);
}

static void test_pop1(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_appendString(b, "HELLO"));

    tj_buffer_popFront(b, 1);
    assert_int_equal(tj_buffer_getUsed(b), 5);
    assert_string_equal(tj_buffer_getAsString(b), "ELLO");
}

static void test_pop2(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_appendString(b, "HELLO"));

    tj_buffer_popBack(b, 1);
    assert_int_equal(tj_buffer_getUsed(b), 5);
    assert_memory_equal(tj_buffer_getAsString(b), "HELLO", 5);
}

static void test_pop3(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_appendString(b, "HELLO"));

    tj_buffer_popFront(b, 6);
    assert_int_equal(tj_buffer_getUsed(b), 0);
}

static void test_pop4(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_appendString(b, "HELLO"));

    tj_buffer_popBack(b, 6);
    assert_int_equal(tj_buffer_getUsed(b), 0);
}

static void test_pop5(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_appendString(b, "HELLO"));

    tj_buffer_popFront(b, 10);
    assert_int_equal(tj_buffer_getUsed(b), 0);
}

static void test_pop6(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_appendString(b, "HELLO"));

    tj_buffer_popBack(b, 10);
    assert_int_equal(tj_buffer_getUsed(b), 0);
}

static void test_pop7(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_appendString(b, "HELLO"));

    tj_buffer_popFront(b, 0);
    assert_int_equal(tj_buffer_getUsed(b), 6);
    assert_string_equal(tj_buffer_getAsString(b), "HELLO");
}

static void test_pop8(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_appendString(b, "HELLO"));

    tj_buffer_popBack(b, 0);
    assert_int_equal(tj_buffer_getUsed(b), 6);
    assert_string_equal(tj_buffer_getAsString(b), "HELLO");
}

static void test_strip1(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_append(b, (tj_buffer_byte*)"  HELLO WORLD  ", 15));
    tj_buffer_strip(b, &isspace);

    assert_int_equal(tj_buffer_getUsed(b), 11);
    assert_memory_equal(tj_buffer_getAsString(b), "HELLO WORLD", 11);
}

static void test_strip2(void **state) {
    tj_buffer *b = *state;

    assert_true(tj_buffer_append(b, (tj_buffer_byte*)"11HELLO WORLD22", 15));
    tj_buffer_strip(b, &isdigit);

    assert_int_equal(tj_buffer_getUsed(b), 11);
    assert_memory_equal(tj_buffer_getAsString(b), "HELLO WORLD", 11);
}

static void test_appendBuffer1(void **state) {
    struct data *data = *state;

    assert_true(tj_buffer_appendBuffer(data->a, data->b));

    assert_int_equal(tj_buffer_getAllocated(data->a), 0);
    assert_int_equal(tj_buffer_getUsed(data->a), 0);
}

static void test_appendBuffer2(void **state) {
    struct data *data = *state;

    assert_true(tj_buffer_appendString(data->a, "HELLO"));

    assert_true(tj_buffer_appendBuffer(data->a, data->b));

    assert_int_equal(tj_buffer_getAllocated(data->a), 6);
    assert_int_equal(tj_buffer_getUsed(data->a), 6);
    assert_string_equal(tj_buffer_getAsString(data->a), "HELLO");
}

static void test_appendBuffer3(void **state) {
    struct data *data = *state;

    assert_true(tj_buffer_appendString(data->b, "HELLO"));

    assert_true(tj_buffer_appendBuffer(data->a, data->b));

    assert_int_equal(tj_buffer_getAllocated(data->a), 6);
    assert_int_equal(tj_buffer_getUsed(data->a), 6);
    assert_string_equal(tj_buffer_getAsString(data->a), "HELLO");
}

static void test_appendBuffer4(void **state) {
    struct data *data = *state;

    assert_true(tj_buffer_appendString(data->a, "HELLO"));
    assert_true(tj_buffer_appendString(data->b, "WORLD"));

    assert_true(tj_buffer_appendBuffer(data->a, data->b));

    assert_int_equal(tj_buffer_getAllocated(data->a), 12);
    assert_int_equal(tj_buffer_getUsed(data->a), 12);
    assert_memory_equal(tj_buffer_getBytes(data->a), "HELLO\0WORLD", 12);
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
        unit_test_setup_teardown(test_getAtIndex, setup, teardown),
        unit_test_setup_teardown(test_pop1, setup, teardown),
        unit_test_setup_teardown(test_pop2, setup, teardown),
        unit_test_setup_teardown(test_pop3, setup, teardown),
        unit_test_setup_teardown(test_pop4, setup, teardown),
        unit_test_setup_teardown(test_pop5, setup, teardown),
        unit_test_setup_teardown(test_pop6, setup, teardown),
        unit_test_setup_teardown(test_pop7, setup, teardown),
        unit_test_setup_teardown(test_pop8, setup, teardown),
        unit_test_setup_teardown(test_strip1, setup, teardown),
        unit_test_setup_teardown(test_strip2, setup, teardown),

        unit_test_setup_teardown(test_appendBuffer1, setup2, teardown2),
        unit_test_setup_teardown(test_appendBuffer2, setup2, teardown2),
        unit_test_setup_teardown(test_appendBuffer3, setup2, teardown2),
        unit_test_setup_teardown(test_appendBuffer4, setup2, teardown2),
    };

    return run_tests(tests);
}
