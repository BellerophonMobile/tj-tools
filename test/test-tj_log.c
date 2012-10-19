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
#include <string.h>

#include <tj_error.h>
#include <tj_log.h>

#include <tj_log_sqlite.h>


//----------------------------------------------------------------------
//----------------------------------------------------------------------
int
main(int argc, char *argv[])
{
  char *component = argv[0];

  tj_log_outchannel *chan;
  if ((chan = tj_log_sqlite_create(0)) == 0) {
    TJ_LOG_CRITICAL(component, "Could not create sqlite outchannel.");
    return -1;
  }
  tj_log_addOutChannel(chan);


  TJ_LOG_VERBOSE(component, "This is a verbose output.");

  TJ_LOG_LOGIC(component, "This is a logic output.");

  TJ_LOG_COMPONENT(component, "This is a component output.");

  TJ_LOG_CRITICAL(component, "This is a critical error output.");

  tj_error *err = tj_error_create(TJ_ERROR_MISSING_SERVICE, "Bad mojo!");
  TJ_LOG_ERROR(component, err,
               "This is a critical error output with an error.");

  return 0;

  // end main
}
