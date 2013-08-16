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

/**
 * If TAG is defined, the tj_log.h header will define shorthand macros
 * such as CRITICAL and VERBOSE.  The full ones remain available.
 */
#define TAG "test-tj_log"
#include "tj_log.h"

// Defines an option log output channel
#include "tj_log_sqlite.h"

struct args {
    tj_log_level level;
    char *component;
    char *file;
    char *func;
    int line;
    tj_error *error;
    char *msg;
};

static void log_func(void *data, tj_log_level level, const char *component,
        const char *file, const char *func, int line, tj_error *error,
        const char *msg) {
    struct args *args = data;

    args->level = level;
    args->component = strdup(component);
    args->file = strdup(file);
    args->func = strdup(func);
    args->line = line;
    args->error = error;
    args->msg = strdup(msg);
}

static void test_1(void **state) {
    struct args args;
    memset(&args, 0, sizeof(args));

    tj_log_outchannel *out = tj_log_outchannel_create(&args,
            &log_func, NULL);
    assert_non_null(out);

    assert_false(tj_log_addOutChannel(out));

    tj_log_log(TJ_LOG_LEVEL_CRITICAL, "tj_log", "test-tj_log.c", "test_1",
            1, NULL, "Hello: %s", "World");

    assert_int_equal(args.level, TJ_LOG_LEVEL_CRITICAL);
    assert_string_equal(args.component, "tj_log");
    assert_string_equal(args.file, "test-tj_log.c");
    assert_string_equal(args.func, "test_1");
    assert_int_equal(args.line, 1);
    assert_null(args.error);
    assert_string_equal(args.msg, "Hello: World");

    free(args.component);
    free(args.file);
    free(args.func);
    free(args.msg);
}

int main(int argc, char **argv) {
    const UnitTest tests[] = {
        unit_test(test_1),
    };

    return run_tests(tests);
}
