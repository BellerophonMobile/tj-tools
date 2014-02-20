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
    opts.add_option('--no-static', action='store_true',
                    help='Disable building static library.')
    opts.add_option('--no-shared', action='store_true',
                    help='Disable building shared library.')
    opts.add_option('--no-solibrary', action='store_true',
                    help='Disable building tj_solibrary.')
    opts.add_option('--no-sqlite', action='store_true',
                    help='Disable building tj_log_sqlite.')

    opts = ctx.add_option_group('Test Options')
    opts.add_option('--no-test', action='store_true',
                    help='Don\'t build or run unit tests.')
    opts.add_option('--valgrind', action='store_true',
                    help='Run tests through valgrind.')

def configure(ctx):
    ctx.load('compiler_c')
    ctx.load('waf_unit_test')

    if 'android' in ctx.env.CC[0]:
        # For tj_log
        ctx.check_cc(lib='log')

    # For tj_searchpathlist
    if not (ctx.options.no_solibrary or
            ctx.check_cc(lib='dl', mandatory=False)):
        print('Disabling tj_solibrary.c')

    # For tj_log_sqlite
    if not (ctx.options.no_sqlite or
            ctx.check_cc(lib='sqlite3', mandatory=False)):
        print('Disabling tj_log_sqlite.c')

    if ctx.env.CC_NAME == 'gcc':
        ctx.env.CFLAGS += ['-std=gnu99', '-Wall', '-Wextra', '-Werror',
                           '-Wno-sign-compare', '-Wno-unused-parameter',
                           '-Wno-unknown-pragmas', '-Wno-pragmas',
                           '-Wno-unknown-warning-option']

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

    src = [
        'src/tj_array.c',
        'src/tj_buffer.c',
        'src/tj_error.c',
        'src/tj_log.c',
        'src/tj_searchpathlist.c',
        'src/tj_template.c',
    ]

    if ctx.env.LIB_DL:
        src.append('src/tj_solibrary.c')

    if ctx.env.LIB_SQLITE3:
        src.append('src/tj_log_sqlite.c')

    if not ctx.options.no_static:
        ctx.stlib(
            target = 'tj-tools',
            use = ['uthash', 'LOG', 'DL', 'SQLITE3'],
            export_includes = 'src',
            source = src,
        )


    if not ctx.options.no_shared:
        ctx.shlib(
            target = 'tj-tools',
            features = 'c',
            use = ['uthash', 'LOG', 'DL', 'SQLITE3'],
            export_includes = 'src',
            source = src,
        )

    ## Unit tests
    if not ctx.options.no_test:
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
        _create_test(ctx, 'tj_log')
        _create_test(ctx, 'tj_searchpathlist')
        if ctx.env.LIB_DL:
            _create_test(ctx, 'tj_solibrary')
        _create_test(ctx, 'tj_template')
        _create_test(ctx, 'tj_util', ['calloc', 'strdup', 'strndup'])


def _create_test(ctx, src, wrappers=None):
    if wrappers is None:
        wrappers = []

    ctx.program(
        features = 'test',
        target = 'test-' + src,
        # Execute test from root project directory, for data files.
        ut_cwd = ctx.top_dir,
        use = ['tj-tools', 'cmocka'],
        source = 'test/test-{}.c'.format(src),
        linkflags = ['-Wl,--wrap=' + symbol for symbol in wrappers],
    )
