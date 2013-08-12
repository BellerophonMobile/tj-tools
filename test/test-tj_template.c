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

#include "tj_buffer.h"
#include "tj_template.h"

struct data {
    tj_template_variables *vars;
    tj_buffer *source;
    tj_buffer *target;
};

static void setup(void **state) {
    struct data *data = malloc(sizeof(*data));
    assert_non_null(data);

    data->vars = tj_template_variables_create();
    assert_non_null(data->vars);

    data->source = tj_buffer_create(0);
    assert_non_null(data->source);

    data->target = tj_buffer_create(0);
    assert_non_null(data->target);

    *state = (void*)data;
}

static void teardown(void **state) {
    struct data *data = *state;

    if (data != NULL) {
        if (data->vars != NULL) {
            tj_template_variables_finalize(data->vars);
        }
        if (data->source != NULL) {
            tj_buffer_finalize(data->source);
        }
        if (data->target != NULL) {
            tj_buffer_finalize(data->target);
        }
        free(data);
    }
}

static void test_1(void **state) {
    struct data *data = *state;

    assert_true(tj_buffer_appendString(data->source, "HEL$XLO!"));
    assert_true(tj_template_variables_setFromString(data->vars, "X", "mushi"));

    assert_true(tj_template_variables_apply(
                data->vars, data->target, data->source));

    assert_string_equal(tj_buffer_getAsString(data->target), "HELmushiLO!");
}

static void test_2(void **state) {
    struct data *data = *state;

    assert_true(tj_buffer_appendString(
                data->source, "A $MAN, a $PLAN, a $CANAL, Panama!"));

    assert_true(tj_template_variables_setFromString(
                data->vars, "MAN", "man"));
    assert_true(tj_template_variables_setFromString(
                data->vars, "PLAN", "plan"));
    assert_true(tj_template_variables_setFromString(
                data->vars, "CANAL", "canal"));

    assert_true(tj_template_variables_apply(
                data->vars, data->target, data->source));

    assert_string_equal(tj_buffer_getAsString(data->target),
                "A man, a plan, a canal, Panama!");
}

static void test_3(void **state) {
    struct data *data = *state;

    assert_true(tj_buffer_appendString(data->source, "$MUSHI"));
    assert_true(tj_template_variables_setFromFile(
                data->vars, "MUSHI", "test/data/mushi"));

    assert_true(tj_template_variables_apply(
                data->vars, data->target, data->source));

    assert_string_equal(tj_buffer_getAsString(data->target), "MUSHI");
}

static void test_4(void **state) {
    struct data *data = *state;

    assert_true(tj_buffer_appendString(data->source, "$MUSHI"));
    assert_true(tj_template_variables_setFromString(data->vars, "X", "mushi"));
    assert_true(tj_template_variables_setFromFile(
                data->vars, "MUSHI", "test/data/mushi2"));

    tj_template_variables_setRecurse(data->vars, "MUSHI", 1);

    assert_true(tj_template_variables_apply(
                data->vars, data->target, data->source));

    assert_string_equal(tj_buffer_getAsString(data->target),
                "mushi mushi mushi");
}

int main(int argc, char *argv[]) {
    const UnitTest tests[] = {
        unit_test_setup_teardown(test_1, setup, teardown),
        unit_test_setup_teardown(test_2, setup, teardown),
        unit_test_setup_teardown(test_3, setup, teardown),
        unit_test_setup_teardown(test_4, setup, teardown),
    };

    return run_tests(tests);
}
