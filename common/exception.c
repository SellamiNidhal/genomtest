/*
 * Copyright (c) 2013 LAAS/CNRS
 * All rights reserved.
 *
 * Redistribution  and  use  in  source  and binary  forms,  with  or  without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice and this list of conditions.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice and  this list of  conditions in the  documentation and/or
 *      other materials provided with the distribution.
 *
 * THE SOFTWARE  IS PROVIDED "AS IS"  AND THE AUTHOR  DISCLAIMS ALL WARRANTIES
 * WITH  REGARD   TO  THIS  SOFTWARE  INCLUDING  ALL   IMPLIED  WARRANTIES  OF
 * MERCHANTABILITY AND  FITNESS.  IN NO EVENT  SHALL THE AUTHOR  BE LIABLE FOR
 * ANY  SPECIAL, DIRECT,  INDIRECT, OR  CONSEQUENTIAL DAMAGES  OR  ANY DAMAGES
 * WHATSOEVER  RESULTING FROM  LOSS OF  USE, DATA  OR PROFITS,  WHETHER  IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR  OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *                                           Anthony Mallet on Fri Mar 22 2013
 */
#define GENOM_EXCEPTION_HANDLER

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "genom3/c/event.h"

/* --- local data ---------------------------------------------------------- */

struct genom_exstorage {
  genom_event ex;
  void *detail;
  size_t size;
};

static pthread_once_t exinit = PTHREAD_ONCE_INIT;
static pthread_key_t exdetail;

static void
genom_exfini(void *data)
{
  struct genom_exstorage *exs = data;

  if (!exs) return;
  if (exs->detail) free(exs->detail);
  free(exs);
}

static void
genom_exinit(void)
{
  pthread_key_create(&exdetail, genom_exfini);
}

genom_event
genom_throw(genom_event ex, void *detail, size_t size)
{
  struct genom_exstorage *exs;

  pthread_once(&exinit, genom_exinit);
  exs = pthread_getspecific(exdetail);
  if (!exs) {
    exs = malloc(sizeof(*exs));
    pthread_setspecific(exdetail, exs);
    if (!exs) return ex;

    exs->ex = genom_ok;
    exs->detail = NULL;
    exs->size = 0;
  }

  if (exs->size < size) {
    exs->size = 0;
    exs->detail = realloc(exs->detail, size);
    if (!exs->detail) return ex;
    exs->size = size;
  }
  exs->ex = ex;
  memcpy(exs->detail, detail, size);
  return ex;
}

const void *
genom_thrown(genom_event *ex)
{
  struct genom_exstorage *exs;

  pthread_once(&exinit, genom_exinit);
  exs = pthread_getspecific(exdetail);
  if (!exs) {
    if (ex) *ex = genom_ok;
    return NULL;
  }
  if (ex) *ex = exs->ex;
  return exs->detail;
}
