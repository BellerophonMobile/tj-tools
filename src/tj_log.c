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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __ANDROID__
#include <android/log.h>
#endif

#include "tj_log.h"
#include "tj_error.h"

const char *tj_log_level_labels[] =
  {
    "VERBOSE",
    "LOGIC",
    "COMPONENT",
    "CRITICAL"
  };


struct tj_log_outchannel {
  int m_allocated;

  void *m_data;

  tj_log_logFunction log;
  tj_log_finalizeFunction finalize;

  tj_log_outchannel *m_next;

  // end tj_log_outchannel
};


//----------------------------------------------------------------------
//----------------------------------------------------------------------
void
tj_log_finalize(void);


// This is done like this so that you could add the fprintf channel in
// addition to the logcat channel on Android.

void
tj_log_fprintfLog(void *data,
                  tj_log_level level, const char *component,
                  const char *file, const char *func, int line,
                  tj_error *error, const char *msg);

tj_log_outchannel tj_log_fprintfChannel =
  {
    .m_allocated = 0,
    .m_data = 0,
    .log = &tj_log_fprintfLog,
    .finalize = 0,
    .m_next = 0
  };

#ifdef __ANDROID__
void
tj_log_logcatLog(void *data,
                 tj_log_level level, const char *component,
                 const char *file, const char *func, int line,
                 tj_error *error, const char *msg);

tj_log_outchannel tj_log_logcatChannel =
  {
    .m_allocated = 0,
    .m_data = 0,
    .log = &tj_log_logcatLog,
    .finalize = 0,
    .m_next = 0
  };

tj_log_outchannel *tj_log_channelStack = &tj_log_logcatChannel;

#else
tj_log_outchannel *tj_log_channelStack = &tj_log_fprintfChannel;
#endif

int tj_log_atexit = 0;


//----------------------------------------------------------------------
//----------------------------------------------------------------------
tj_log_outchannel *
tj_log_outchannel_create(void *data,
                         tj_log_logFunction log,
                         tj_log_finalizeFunction finalize)
{
  tj_log_outchannel *x;
  if ((x = (tj_log_outchannel *) malloc(sizeof(tj_log_outchannel))) == 0) {
    TJ_LOG_CRITICAL("tj_log", "No memory to allocate tj_log_outchannel.");
    return 0;
  }

  x->m_allocated = 1;
  x->m_data = data;
  x->log = log;
  x->finalize = finalize;
  x->m_next = 0;

  return x;

  // end tj_log_outchannel_finalize
}

void
tj_log_outchannel_finalize(tj_log_outchannel *x)
{
  if (x->finalize != 0)
    x->finalize(x->m_data);

  if (x->m_allocated)
    free(x);
  // end tj_log_outchannel_finalize
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
int
tj_log_addOutChannel(tj_log_outchannel *out)
{
  out->m_next = tj_log_channelStack;
  tj_log_channelStack = out;

  if (!tj_log_atexit) {
    atexit(&tj_log_finalize);
    tj_log_atexit = 1;
  }

  return 0;
  // end tj_log_addOutChannel
}

//----------------------------------------------------------------------
void
tj_log_finalize(void)
{
  tj_log_outchannel *next;
  tj_log_outchannel *out = tj_log_channelStack;
  while (out != 0) {
    next = out->m_next;
    tj_log_outchannel_finalize(out);
    out = next;
  }

  tj_log_atexit = 0;
  // end tj_log_finalize
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
void
tj_log_log(tj_log_level level, const char *component,
           const char *file, const char *func, int line,
           tj_error *error, const char *m, ...)
{
  char msg[255]; // Allocated on stack to be thread safe.

  va_list ap;
  va_start(ap, m);
  vsnprintf(msg, 255, m, ap);
  va_end(ap);

  tj_log_outchannel *out = tj_log_channelStack;
  while (out != 0) {
    out->log(out->m_data, level, component, file, func, line, error, msg);
    out = out->m_next;
  }

  // end tj_log_log
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
void
tj_log_fprintfLog(void *data,
                  tj_log_level level, const char *component,
                  const char *file, const char *func, int line,
                  tj_error *error, const char *msg)
{

  if (level == TJ_LOG_LEVEL_CRITICAL)
    fprintf(TJ_LOG_STREAM, "[%s] ", tj_log_level_labels[level]);

  fprintf(TJ_LOG_STREAM, "%s %s:%s:%d: %s\n",
          component, file, func, line, msg);

  if (error != 0)
    fprintf(TJ_LOG_STREAM, "%s\n", tj_error_getMessage(error));

  // end tj_log_fprintfLog
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
#ifdef __ANDROID__
void
tj_log_logcatLog(void *data,
                 tj_log_level level, const char *component,
                 const char *file, const char *func, int line,
                 tj_error *error, const char *msg)
{

  android_LogPriority pri;
  switch (level) {
  case TJ_LOG_LEVEL_VERBOSE:
    pri = ANDROID_LOG_VERBOSE;
    break;

  case TJ_LOG_LEVEL_LOGIC:
    pri = ANDROID_LOG_DEBUG;
    break;

  case TJ_LOG_LEVEL_COMPONENT:
    pri = ANDROID_LOG_INFO;
    break;

  case TJ_LOG_LEVEL_CRITICAL:
    pri = ANDROID_LOG_ERROR;
    break;
  }

  __android_log_print(pri, component, "%s:%s:%d: %s",
                      file, func, line, msg);

  if (error != 0)
    __android_log_print(pri, component, "%s", tj_error_getMessage(error));

  // end tj_log_logcatLog
}
#endif
