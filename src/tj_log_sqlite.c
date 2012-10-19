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

#include <stdlib.h>

#include <sqlite3.h>

#include "tj_log.h"


#define TJ_LOG_SQLITE_COMPONENT "tj_log_sqlite"

#ifdef __ANDROID__
#define TJ_LOG_SQLITE_DEFAULT_DB_FILE "/sdcard/tj_log.db"
#else
#define TJ_LOG_SQLITE_DEFAULT_DB_FILE "tj_log.db"
#endif


//----------------------------------------------------------------------
//----------------------------------------------------------------------
typedef struct tj_log_sqlite tj_log_sqlite;
struct tj_log_sqlite {
  sqlite3 *m_db;
  sqlite3_stmt *m_insertStmt;
};

void
tj_log_sqlite_finalize(void *x);

void
tj_log_sqlite_log(void *data,
                  tj_log_level level, const char *component,
                  const char *file, const char *func, int line,
                  tj_error *error, const char *msg);


//----------------------------------------------------------------------
//----------------------------------------------------------------------
tj_log_outchannel *
tj_log_sqlite_create(const char *dbfile)
{

  tj_log_sqlite *logger = malloc(sizeof(tj_log_sqlite));
  if (logger == 0) {
    TJ_LOG_CRITICAL(TJ_LOG_SQLITE_COMPONENT,
                    "No memory to allocate tj_log_sqlite.");
    goto error;
  }

  if (dbfile == 0)
    dbfile = TJ_LOG_SQLITE_DEFAULT_DB_FILE;

  //-- Setup the database stuff
  if (sqlite3_open(dbfile, &logger->m_db)) {
    TJ_LOG_CRITICAL(TJ_LOG_SQLITE_COMPONENT,
                    "Could not open or create sqlite3 database %s:\n%s",
                    dbfile, sqlite3_errmsg(logger->m_db));
    goto error;
  }

  char *errBuff = 0;
  if (sqlite3_exec
      (logger->m_db,
       "create table if not exists log (date, time, level, "
       "                                component, file, func, line, msg);",
       0, 0,
       &errBuff) != SQLITE_OK || errBuff != 0) {
    TJ_LOG_CRITICAL(TJ_LOG_SQLITE_COMPONENT,
                    "Could not create table log in %s:\n%s",
                    dbfile, sqlite3_errmsg(logger->m_db));
    goto error;
  }

  if (sqlite3_prepare_v2(logger->m_db,
                         "insert into log values (date('now'), time('now'), "
                         "                        ?1, ?2, ?3, ?4, ?5, ?6);", -1,
                         &logger->m_insertStmt,
                         0)) {
    TJ_LOG_CRITICAL(TJ_LOG_SQLITE_COMPONENT,
                    "Could not prepare insert statement:\n%s",
                    sqlite3_errmsg(logger->m_db));
    goto error;
  }

  tj_log_outchannel *channel =
    tj_log_outchannel_create(logger,
                             &tj_log_sqlite_log,
                             &tj_log_sqlite_finalize);

  if (channel != 0)
    return channel;


 error:
  if (logger != 0)
    tj_log_sqlite_finalize(logger);

  return 0;

  // end tj_log_sqlite_create
}

//----------------------------------------------
void
tj_log_sqlite_finalize(void *x)
{
  tj_log_sqlite *data = (tj_log_sqlite *) x;

  if (data->m_insertStmt != 0)
    sqlite3_finalize(data->m_insertStmt);

  if (data->m_db != 0)
    sqlite3_close(data->m_db);

  free(x);
  // end tj_log_sqlite_finalize
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
void
tj_log_sqlite_log(void *data,
                  tj_log_level level, const char *component,
                  const char *file, const char *func, int line,
                  tj_error *error, const char *msg)
{
  tj_log_sqlite *sqlite = (tj_log_sqlite *) data;

  // level, component, file, func, line, msg

  sqlite3_bind_text(sqlite->m_insertStmt, 1,
                    tj_log_level_labels[level], -1, SQLITE_STATIC);

  sqlite3_bind_text(sqlite->m_insertStmt, 2,
                    component, -1, SQLITE_TRANSIENT);

  sqlite3_bind_text(sqlite->m_insertStmt, 3,
                    file, -1, SQLITE_STATIC);

  sqlite3_bind_text(sqlite->m_insertStmt, 4,
                    func, -1, SQLITE_STATIC);

  sqlite3_bind_int(sqlite->m_insertStmt, 5, line);

  sqlite3_bind_text(sqlite->m_insertStmt, 6,
                    msg, -1, SQLITE_TRANSIENT);

  int dbres = sqlite3_step(sqlite->m_insertStmt);
  if (dbres != SQLITE_OK && dbres != SQLITE_DONE) {
    TJ_LOG_CRITICAL(TJ_LOG_SQLITE_COMPONENT,
                    "Could not insert new log into db:\n%s",
                    sqlite3_errmsg(sqlite->m_db));
  }
  sqlite3_reset(sqlite->m_insertStmt);

  // end tj_log_sqlite_log
}
