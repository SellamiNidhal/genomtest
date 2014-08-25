<'
# Copyright (c) 2012-2013 LAAS/CNRS
# All rights reserved.
#
# Redistribution and use  in source  and binary  forms,  with or without
# modification, are permitted provided that the following conditions are
# met:
#
#   1. Redistributions of  source  code must retain the  above copyright
#      notice and this list of conditions.
#   2. Redistributions in binary form must reproduce the above copyright
#      notice and  this list of  conditions in the  documentation and/or
#      other materials provided with the distribution.
#
#                                      Anthony Mallet on Sat Jan 14 2012
#

if {[llength $argv] != 2} { error "expected arguments: component task" }
lassign $argv component task

# compute handy shortcuts
set comp [$component name]
set tname [$task name]

lang c
'>
#include "autoconf/acheader.h"

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include "<"$comp">_internals.h"
#include "<"$comp">_bip-genom.h"



/* --- local data ---------------------------------------------------------- */

static void *	<"$comp">_<"$tname">_task(void *data);



/* <"[--- genom_task_${tname}::spawn ------------------------------------]"> */

void
genom_task_<"$tname">::spawn()
{
  int s;

  s = pthread_cond_init(&signal, NULL);
  if (s) err(2, "initializing task condition variable");
  id = 0;

  pthread_mutex_lock(&cids.task_lock);
  runnable = wakeup = false;
  status = <"$comp">_sleep;

  s = pthread_create(&id, NULL, <"$comp">_<"$tname">_task, NULL);
  if (s) err(2, "spawning task <"[$t name]">");
  do {
    s = pthread_cond_wait(&signal, &cids.task_lock);
    if (s) err(2, "spawning task <"[$t name]">");
  } while(status == <"$comp">_sleep);
  pthread_mutex_unlock(&cids.task_lock);
}


/* <"[--- ${comp}_${tname}_task -----------------------------------------]"> */

/** <"$tname"> task of component <"$comp">
 *
 */

static void *
<"$comp">_<"$tname">_task(void *dummy)
{
  genom_task_<"$tname"> &task = cids.tasks.<"$tname">;
  genom_state_component *state = cids.ports.genom_state.handle.data();
  struct timeval tvs, tve;
  genom_activity *a;
  int sched, shutdown, done;
  size_t i;
  int s;

  pthread_mutex_lock(&cids.task_lock);
  task.status = genom_ok;
  task.pact = <"$comp">_start;

  /* permanent activity start codel */
  s = <"$comp">_<"$tname">_invoke(task.pact);
  if (s) task.status = task.pact;
//task.status = task.pact;
  genom_log_info("<"$tname"> task initialized and running");
  pthread_cond_signal(&task.signal);
  pthread_mutex_unlock(&cids.task_lock);

  if (task.status) {
    genom_log_warn("shutting down <"$tname"> task");
    return NULL;
  }

  /* loop */
  done = shutdown = 0;
  pthread_mutex_lock(&cids.task_lock);
  do {
    while(!task.wakeup && !cids.shutdown) {
      s = pthread_cond_wait(&task.signal, &cids.task_lock);
      if (s) err(2, "timer task <"[$t name]">");
    }
    task.runnable = task.wakeup = false;
    sched = 0;
    genom_log_debug("<"$tname"> tick %d", cids.ticks);
    gettimeofday(&tvs, NULL);

    /* handle shutdown: interrupt all if not done already */
    if (cids.shutdown && !shutdown) {
      shutdown = 1;
      task.pact = <"$comp">_stop;
      for(i=0; i<genom_activities::MAX_ACTIVITIES; i++) {
        a = cids.activities.activities[i];
        if (!a || a->task != task.id) continue;
        a->stop("kill");
      }
    }

    /* handle permanent activity */
    s = <"$comp">_<"$tname">_invoke(task.pact);
    if (s) {
      task.status = task.pact; break;
    }

    /* handle current activities for this task */
    for(i=0; i<genom_activities::MAX_ACTIVITIES; i++) {
      a = cids.activities.activities[i];

      if (!a || a->task != task.id) continue;
      if (a->status == ACT_STOP) {
        a->status = ACT_STOPPING;
        a->codel = <"${comp}">_stop;
      }
      if (a->status != ACT_RUNNING && a->status != ACT_STOPPING)
        continue;

      genom_log_debug("<"$tname"> activity %d", a->id);
     

   
    }
  

    /* handle shutdown: continue until there is some activtiy, or break */
   if (cids.shutdown) {
      done = 1;
      if (task.pact != <"$comp">_ether) done = 0;
      for(i=0; i<genom_activities::MAX_ACTIVITIES; i++) {
        a = cids.activities.activities[i];
        if (!a || a->task != task.id) continue;
        if (a->status != ACT_ETHER && a->status != ACT_STOPPED) done = 0;
      }
    }
  } while(!done);

  pthread_mutex_unlock(&cids.task_lock);

  genom_log_info("shutting down <"$tname"> task");
  return NULL;
}








/* === services codels ===================================================== */

<'foreach s [$task services] {'>
/* <"[--- Service [$s name] ---------------------------------------------]"> */

<'  foreach e [$s fsm] { set codel [$s fsm $e]'>

/* state <"[$e name]"> */
<'}'>

/* invoking codels according to <"[$s name]"> state is from Bip Model */

template<> genom_event
genom_activity_data_<"[$s name]">::invoke()
{
 
   

  return NULL;
}


<'}'>








