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

#define TAG "test-tj_util"
#include "tj_log.h"
#include "tj_util.h"

#define MAKE_WRAPPER(func, type) \
    type __real_ ## func; \
    type __wrap_ ## func { return (type)mock(); }

MAKE_WRAPPER(calloc(size_t nmemb, size_t size), void *);
MAKE_WRAPPER(strndup(const char *s, size_t n), char *);

/* For some reason, wrapping strdup doesn't work... */
#if 0
MAKE_WRAPPER(strdup(const char *s), char *);
#endif

static void test_calloc_success(void **state) {
    will_return(__wrap_calloc, __real_calloc(1, sizeof(int)));

    int *a;
    TJ_CALLOC(a);
    assert_non_null(a);
    free(a);

    return;

error:
    assert_true(0);
}

static void test_calloc_fail(void **state) {
    will_return(__wrap_calloc, NULL);

    int *a;
    TJ_CALLOC(a);

    /* This should never be called.  Just used to stop it from being optimized
     * away. */
    printf("A: %d\n", *a);
    free(a);

    return;

error:
    assert_null(a);
}

/* For some reason, wrapping strdup doesn't work... */
#if 0
static void test_strdup_success(void **state) {
    will_return(__wrap_strdup, __real_strdup("Hello"));

    char *s;
    TJ_STRDUP(s, "Hello");
    assert_non_null(s);
    assert_string_equal(s, "Hello");
    free(s);

    return;

    goto error;

error:
    assert_true(0);
}

static void test_strdup_fail(void **state) {
    will_return(__wrap_strdup, NULL);

    char *s;
    TJ_STRDUP(s, "Hello");
    free(s);

    /* This should never be called.  Just used to stop it from being optimized
     * away. */
    printf("S: %s\n", s);

    return;

error:
    assert_null(s);
}
#endif

static void test_strndup_success(void **state) {
    will_return(__wrap_strndup, __real_strndup("Hello", 5));

    char *s;
    TJ_STRNDUP(s, "Hello", 5);
    assert_non_null(s);
    assert_string_equal(s, "Hello");
    free(s);

    return;

error:
    assert_true(0);
}

static void test_strndup_fail(void **state) {
    will_return(__wrap_strndup, NULL);

    char *s;
    TJ_STRNDUP(s, "Hello", 5);

    /* This should never be called.  Just used to stop it from being optimized
     * away. */
    printf("S: %s\n", s);
    free(s);

    return;

error:
    assert_null(s);
}

int main(int argc, char *argv[]) {
    const UnitTest tests[] = {
        unit_test(test_calloc_success),
        unit_test(test_calloc_fail),
        //unit_test(test_strdup_success),
        //unit_test(test_strdup_fail),
        unit_test(test_strndup_success),
        unit_test(test_strndup_fail),
    };

    return run_tests(tests);
}
