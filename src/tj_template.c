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

#include <string.h>

#include "tj_template.h"

//----------------------------------------------------------------------
//----------------------------------------------------------------------
#ifndef TJ_LOG_STREAM
#define TJ_LOG_STREAM stdout
#endif

#ifndef TJ_ERROR_STREAM
#define TJ_ERROR_STREAM stderr
#endif

#ifndef TJ_LOG
#ifdef NDEBUG
#define TJ_LOG(M, ...)
#else
#define TJ_LOG(M, ...) fprintf(TJ_LOG_STREAM, "%s: " M "\n", __FUNCTION__, ##__VA_ARGS__)
#endif // ifndef NDEBUG else
#endif // ifndef TJ_LOG

#ifndef TJ_ERRROR
#define TJ_ERROR(M, ...) fprintf(TJ_ERROR_STREAM, "[ERROR] %s:%s:%d: " M "\n", __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#ifndef TJ_PAGE_SIZE
#define TJ_PAGE_SIZE 1024
#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
typedef enum {
  SCAN,
  MARK,
  TRACK
} tmpl_scan_mode;

int
tj_template_appendExpansion(tj_buffer *dest,
                            tj_buffer *src,
                            const tj_template_variable *variables,
                            int numVariables)
{
  int tmplIndex = 0;
  int v;
  int start = 0, end = 0;
  char tracking[TJ_TEMPLATE_MAX_VARS];
  size_t sublen[TJ_TEMPLATE_MAX_VARS];
  tmpl_scan_mode mode = SCAN;
  tj_buffer_byte *template = tj_buffer_getBytes(src);
  int varScanLen;

  if (numVariables > TJ_TEMPLATE_MAX_VARS) {
    TJ_ERROR("Too many variables, %d > max %d\n",
                      numVariables, TJ_TEMPLATE_MAX_VARS);
    return 0;
  }

  for (v = 0; v < numVariables; v++) {
    if (variables[v].substitution != 0) {
      sublen[v] = strlen(variables[v].substitution);
    } else {
      sublen[v] = 0;
    }
  }

  while (tmplIndex < tj_buffer_getUsed(src)) {
    if (template[tmplIndex] == '$') {
      if (mode == SCAN) {
	mode = MARK;
      } else if (mode == MARK) {
        if (!tj_buffer_append(dest, &template[start], tmplIndex-start)) {
          TJ_ERROR("Could not append text before mark.");
          goto error;
        }
	mode = SCAN;
	start = tmplIndex+1;
	// end mode == MARK
      }
    } else {
      if (mode == MARK) {
	for (v = 0; v < numVariables; v++) {
	  tracking[v] = 1;
        }
	varScanLen = 0;
	end = tmplIndex-1; // Cannot happen before tmplIndex==1, so safe
	mode = TRACK;
      }

      if (mode == TRACK) {
	char alive = 0;

	for (v = 0; v < numVariables; v++) {

	  if (tracking[v]) {

	    if (variables[v].label[varScanLen] == 0) {
	      alive = 0;

              if (!tj_buffer_append(dest, &template[start], end-start)) {
                TJ_ERROR("Could not append pre-substitution text.");
                goto error;
              }

              if (!tj_buffer_append(dest,
                                    (tj_buffer_byte *)variables[v].substitution,
                                    sublen[v])) {
                TJ_ERROR("Could not append substitution.");
                goto error;
              }
	      start = tmplIndex;

	      break;
	    } else if (variables[v].label[varScanLen] != template[tmplIndex]) {
	      tracking[v] = 0;
	    } else {
              alive = 1;
            }
	  }

	  // end looping vars
	}

	if (alive) {
	  varScanLen++;
	} else {
	  mode = SCAN;
	}
	// end track
      }
    }

    tmplIndex++;
    // end looping over template
  }

  if (!tj_buffer_append(dest, &template[start], tmplIndex-start)) {
    TJ_ERROR("Could not append final chunk.");
    goto error;
  }

  return 1;

 error:
  return 0;
  // end tj_template_expand
}
