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

    opts = ctx.add_option_group('Test Options')
    opts.add_option('--valgrind', action='store_true',
                    help='Run tests through valgrind.')

def configure(ctx):
    ctx.load('compiler_c')
    ctx.load('waf_unit_test')

    if 'android' in ctx.env.CC[0]:
        # For tj_log
        ctx.check_cc(lib='log')

    # For tj_searchpathlist
    ctx.check_cc(lib='dl')

    # For tj_log_sqlite
    ctx.check_cc(lib='sqlite3')

    if ctx.env.CC_NAME == 'gcc':
        ctx.env.CFLAGS += ['-std=gnu99', '-Wall', '-Wextra', '-Werror',
                           '-Wno-unused-parameter', '-Wno-sign-compare',
                           '-Wno-clobbered', '-Wno-unknown-warning-option']

        if ctx.options.optimize:
            ctx.env.DEFINES += ['NDEBUG']
            ctx.env.CFLAGS += ['-O3']
        else:
            ctx.env.CFLAGS += ['-O2', '-g']

def build(ctx):
    # Add extra unit testing output
    ctx.post_mode = waflib.Build.POST_LAZY
    ctx.add_post_fun(waflib.Tools.waf_unit_test.summary)
    ctx.add_post_fun(waflib.Tools.waf_unit_test.set_exit_code)

    if ctx.options.valgrind:
        ctx.options.testcmd = 'valgrind --error-exitcode=1 --leak-check=full --show-reachable=yes %s'

    ctx(
        name = 'uthash',
        export_includes = 'deps/uthash/src',
    )

    ctx.stlib(
        target = 'tj-tools',
        use = ['uthash', 'LOG', 'DL', 'SQLITE3'],
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

    _create_test(ctx, 'tj_array')
    _create_test(ctx, 'tj_buffer')
    _create_test(ctx, 'tj_error')
    _create_test(ctx, 'tj_heap')
    _create_test(ctx, 'tj_template')
    _create_test(ctx, 'tj_log')
    _create_test(ctx, 'tj_searchpathlist')
    _create_test(ctx, 'tj_solibrary')

def _create_test(ctx, src):
    ctx.program(
        features = 'test',
        target = 'test-' + src,
        # Execute test from root project directory, for data files.
        ut_cwd = ctx.top_dir,
        use = ['tj-tools', 'cmocka'],
        source = 'test/test-{}.c'.format(src),
    )
