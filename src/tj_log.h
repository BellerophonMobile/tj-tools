/*
 * Copyright (c) 2012 Joe Kopena <tjkopena@gmail.com>
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

#ifndef __tj_log_h__
#define __tj_log_h__

#include <stdio.h>

#include "tj_error.h"

typedef enum {
  TJ_LOG_LEVEL_VERBOSE,
  TJ_LOG_LEVEL_LOGIC,
  TJ_LOG_LEVEL_COMPONENT,
  TJ_LOG_LEVEL_CRITICAL
} tj_log_level;


#ifdef NDEBUG

//-- No debugging!  Let them eat cake!
#define TJ_LOG_VERBOSE(component, msg, ...)
#define TJ_LOG_LOGIC(component, msg, ...)
#define TJ_LOG_COMPONENT(component, msg, ...)
#define TJ_LOG_CRITICAL(component, e, msg, ...)

#define TJ_LOG_LOG(component, type, msg, ...)

#else

//-- Do some debugging...  BOOOORING!
#ifndef TJ_LOG_LOG
#define TJ_LOG_LOG(level, component, e, msg, ...)                      \
  tj_log_log(level, component,                                         \
             __FILE__, __FUNCTION__, __LINE__,                         \
             e, msg, ##__VA_ARGS__)
#endif

#ifndef TJ_LOG_VERBOSE
  #define TJ_LOG_VERBOSE(component, msg, ...)                          \
    TJ_LOG_LOG(TJ_LOG_LEVEL_VERBOSE, component, 0, msg, ##__VA_ARGS__)
#endif

#ifndef TJ_LOG_LOGIC
#define TJ_LOG_LOGIC(component, msg, ...)                              \
  TJ_LOG_LOG(TJ_LOG_LEVEL_LOGIC, component, 0, msg, ##__VA_ARGS__)
#endif

#ifndef TJ_LOG_COMPONENT
#define TJ_LOG_COMPONENT(component, msg, ...)                          \
  TJ_LOG_LOG(TJ_LOG_LEVEL_COMPONENT, component, 0, msg, ##__VA_ARGS__)
#endif

#ifndef TJ_LOG_CRITICAL
#define TJ_LOG_CRITICAL(component, msg, ...)                           \
  TJ_LOG_LOG(TJ_LOG_LEVEL_CRITICAL, component, 0, msg, ##__VA_ARGS__)
#endif

#ifndef TJ_LOG_ERROR
#define TJ_LOG_ERROR(component, e, msg, ...)                           \
  TJ_LOG_LOG(TJ_LOG_LEVEL_CRITICAL, component, e, msg, ##__VA_ARGS__)
#endif

// end !NDEBUG
#endif


#ifdef TAG
#define VERBOSE(msg, ...) TJ_LOG_VERBOSE(TAG, msg, ##__VA_ARGS__)
#define LOGIC(msg, ...) TJ_LOG_LOGIC(TAG, msg, ##__VA_ARGS__)
#define COMPONENT(msg, ...) TJ_LOG_COMPONENT(TAG, msg, ##__VA_ARGS__)
#define CRITICAL(msg, ...) TJ_LOG_CRITICAL(TAG, msg, ##__VA_ARGS__)
#define ERROR(e, msg, ...) TJ_LOG_ERROR(TAG, e, msg, ##__VA_ARGS__)
#endif


#ifndef TJ_LOG_STREAM
#define TJ_LOG_STREAM stdout
#endif


extern const char *tj_log_level_labels[];

//----------------------------------------------------------------------
//----------------------------------------------------------------------
typedef void (*tj_log_logFunction)(void *,
                                   tj_log_level, const char *,
                                   const char *, const char *, int,
                                   tj_error *, const char *);
typedef void (*tj_log_finalizeFunction)(void *);

typedef struct tj_log_outchannel tj_log_outchannel;

extern tj_log_outchannel tj_log_fprintfChannel;

//----------------------------------------------------------------------
//----------------------------------------------------------------------
tj_log_outchannel *
tj_log_outchannel_create(void *data,
                         tj_log_logFunction log,
                         tj_log_finalizeFunction finalize);

/**
 * Add a channel through which log messages are output.  Channels
 * stack up, such that the most recently added is the first to be
 * processed for output on each log call.
 *
 * \return 0 on success, 1 otherwise.
 */
int tj_log_addOutChannel(tj_log_outchannel *out);

/**
 * The core logging message function.  In general the macros
 * TJ_LOG_VERBOSE, TJ_LOG_LOGIC, TJ_LOG_COMPONENT, and TJ_LOG_CRITICAL
 * should be used instead.
 */
void tj_log_log(tj_log_level level, const char *component,
                const char *file, const char *func, int line,
                tj_error *error, const char *m, ...);

#endif // __tj_log_h__
