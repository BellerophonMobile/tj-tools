#!/usr/bin/env python
'''
tj-tools - Collection of C code snippets.

Copyright (c) 2013 Joe Kopena <tjkopena@gmail.com>
'''

import os.path
import platform

import waflib
import waflib.Build
import waflib.Tools.waf_unit_test

def options(ctx):
    ctx.load('compiler_c')
    ctx.load('waf_unit_test')

    opts = ctx.add_option_group('Build Options')
    opts.add_option('--optimize', action='store_true',
                    help='Build with optimization flags (default debug).')

def configure(ctx):
    ctx.load('compiler_c')
    ctx.load('waf_unit_test')

    if 'android' in ctx.env.CC[0]:
        ctx.check_cc(lib='log')

    ctx.check_cc(lib='sqlite3')

    if ctx.env.CC_NAME == 'gcc':
        ctx.env.CFLAGS += ['-std=gnu99', '-Wall', '-Wextra', '-Werror',
                           '-Wno-unused-parameter', '-Wno-sign-compare']

        if ctx.options.optimize:
            ctx.env.DEFINES += ['NDEBUG']
            ctx.env.CFLAGS += ['-O3']
        else:
            ctx.env.CFLAGS += ['-O2', '-g']

def build(ctx):
    ctx.post_mode = waflib.Build.POST_LAZY
    ctx.add_post_fun(waflib.Tools.waf_unit_test.summary)

    ctx(
        name = 'uthash',
        export_includes = 'deps/uthash/src',
    )

    ctx.stlib(
        target = 'tj-tools',
        use = ['uthash', 'LOG', 'SQLITE3'],
        export_includes = 'src',
        source = [
            'src/tj_array.c',
            'src/tj_buffer.c',
            'src/tj_error.c',
            'src/tj_log.c',
            'src/tj_log_sqlite.c',
            'src/tj_searchpathlist.c',
            'src/tj_solibrary.c',
            'src/tj_template.c',
        ],
    )

    ## Unit tests
    ctx.stlib(
        target = 'cmocka',
        defines = 'HAVE_MALLOC_H',
        includes = 'deps/cmocka/include',
        export_includes = 'deps/cmocka/include',
        source = 'deps/cmocka/src/cmocka.c',
    )

    ctx.program(
        features = 'test',
        target = 'test-tj_buffer',
        # Run from the root directory
        ut_cwd = ctx.top_dir,
        use = ['tj-tools', 'cmocka'],
        source = 'test/test-tj_buffer.c',
    )

    ctx.program(
        features = 'test',
        target = 'test-tj_template',
        # Run from the root directory
        ut_cwd = ctx.top_dir,
        use = ['tj-tools', 'cmocka'],
        source = 'test/test-tj_template.c',
    )

    ctx.program(
        features = 'test',
        target = 'test-tj_heap',
        use = ['tj-tools', 'cmocka'],
        source = 'test/test-tj_heap.c',
    )

    ctx.program(
        features = 'test',
        target = 'test-tj_array',
        use = ['tj-tools', 'cmocka'],
        source = 'test/test-tj_array.c',
    )

    #ctx.program(
    #    target = 'test-tj_log',
    #    use = ['tj-tools'],
    #    source = 'test/test-tj_log.c',
    #)

    #ctx.program(
    #    target = 'test-tj_error',
    #    use = 'tj-tools',
    #    source = 'test/test-tj_error.c',
    #)


## Setup for unit tests.

def test(ctx):
    global run_tests
    run_tests = True
    waflib.Options.options.all_tests = True
    waflib.Options.commands = ['build'] + waflib.Options.commands

class Test(waflib.Build.BuildContext):
    'Context class for our unit testing command.'
    cmd = 'test'
    fun = 'test'

def _test_runnable_status(self):
    'Control when unit tests are run'
    global run_tests
    if not run_tests:
        return waflib.Task.SKIP_ME
    else:
        return old_runnable_status(self)

# Hack to control when the unit tests are run
run_tests = False

# Monkey-patch this function to take over control of when unit-tests are run.
# Normally, they are run every time you build.
old_runnable_status = waflib.Tools.waf_unit_test.utest.runnable_status
waflib.Tools.waf_unit_test.utest.runnable_status = _test_runnable_status
