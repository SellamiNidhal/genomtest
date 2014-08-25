<'
# Copyright (c) 2011-2013 LAAS/CNRS
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
#                                      Anthony Mallet on Tue Dec 13 2011
#



if {[llength $argv] != 1} { error "expected arguments: component" }
lassign $argv component

# compute handy shortcuts
set comp [$component name]

lang c
'>
#include "autoconf/acheader.h"

#include <cstring>
#include <unistd.h>
#include <signal.h>

#include "ros/ros.h"
#include "ros/callback_queue.h"
#include "actionlib/server/action_server.h"

#include "<"$comp">_internals.h"
#include "<"$comp">_typecopy.h"
#include "<"$comp">_bip-genom.h"


/* --- local data ---------------------------------------------------------- */

<"$comp">CntrlStrId cids;


/* <"[--- ${comp}_cntrl_task ----------------------------------------------- ]"> */

/** Control task of component <"$comp">
 *
 * This task waits for incoming messages or internal events. Upon reception
 * of a request, the corresponding callback function is invoked. Internal
 * events are handled and final replies are sent to the client.
 */

int
<"$comp">_cntrl_task(const char *instance)
{
  ros::CallbackQueue *queue = ros::getGlobalCallbackQueue();
  pthread_mutexattr_t attr;
  int s, done;

  cids.shutdown = 0;
  cids.node = new ros::NodeHandle(instance);

  /* disable actionlib status updates when goal status does not change */
  cids.node->setParam("actionlib_status_frequency", 0);

  s = pthread_mutexattr_init(&attr);
  s += pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  s += pthread_mutex_init(&cids.task_lock, &attr);
  if (s) err(2, "initializing task lock");

  cids.ticks = 0;
  memset(cids.run_map, 0, sizeof(cids.run_map));
<'if {![catch {$component ids}]} {'>
  genom::ids::pinit(cids.ids);
<'}'>

  /* advertise data outports */
  genom_log_info("advertising ports");
<'foreach p [$component ports out simple] {'>
  cids.ports.<"[$p name]">.init(cids.node);
<'}'>
  s = genom_state_<"$comp">_init();
  if (s) errx(2, "error while initializing state port");

  /* create tasks */
<'foreach t [$component tasks] {'>
  cids.tasks.<"[$t name]">.spawn();
<'}'>

  /* advertise services */
  genom_log_info("advertising services");
<'foreach s [$component services] {'>
<'  if {[catch {$s task} t]} {'>
  cids.services.<"[$s name]"> =
    cids.node->advertiseService("<"[$s name]">",
                                <"$comp">_<"[$s name]">_rqstcb);
<'  } else {'>
  cids.services.<"[$s name]"> =
    new actionlib::ActionServer< genom::action::<"[$s name]"> >(
      *cids.node, "<"[$s name]">",
      <"$comp">_<"[$s name]">_rqstcb, <"$comp">_<"[$s name]">_intercb,
      false);
  cids.services.<"[$s name]">->start();
<'  }'>
<'}'>

<'if {![catch {$component clock-rate} rate]} {'>
  /* create timer */
  cids.clock = cids.node->createTimer(ros::Duration(<"[$rate value]">/10),
                                      <"$comp">_timercb,
                                      false, true);
<'}'>

  /* 
   * If SIGUSR1 is set to SIG_IGN, it can be two things:
   *   - the parent really ignores it, so it won't care about it
   *   - the parent want to significate that it have a special handler to deal
   *   with it. In this case, it is valuable to send it. It is in particular
   *   true in the case of the daemon mode.
   */
  sig_t handler = signal(SIGUSR1, SIG_IGN);
  if (handler == SIG_IGN)
	  kill(getppid(), SIGUSR1);
  signal(SIGUSR1, handler);

  /* go */
  genom_log_info("control task initialized and running");
  done = 0;
  while(ros::ok() && !done) {
    queue->callAvailable(ros::WallDuration(1.));

    if (cids.shutdown) {
      done = 1;
      pthread_mutex_lock(&cids.task_lock);
      for(unsigned i=0; i<genom_activities::MAX_ACTIVITIES; i++) {
        if (cids.activities.activities[i]) done = 0;
      }
      pthread_mutex_unlock(&cids.task_lock);
    }
  }

  /* cleanup */
  pthread_mutex_lock(&cids.task_lock);
<'foreach task [$component tasks] {'>
  pthread_cond_signal(&cids.tasks.<"[$task name]">.signal);
<'}'>
  pthread_mutex_unlock(&cids.task_lock);
<'foreach task [$component tasks] {'>
  pthread_join(cids.tasks.<"[$task name]">.id, NULL);
<'}'>

  genom_log_info("shutting down services");
<'foreach s [$component services] {'>
<'  if {[catch {$s task} t]} {'>
  cids.services.<"[$s name]">.shutdown();
<'  } else {'>
  delete cids.services.<"[$s name]">;
<'  }'>
<'}'>

  genom_log_info("shutting down ports");
<'foreach p [$component ports] {'>
  cids.ports.<"[$p name]">.fini();
<'}'>

  genom_log_info("shutting down control task");
<'if {![catch {$component clock-rate} rate]} {'>
  cids.clock.stop();
<'}'>
<'if {![catch {$component ids}]} {'>
  genom::ids::pfini(cids.ids);
<'}'>
  cids.node->shutdown();
  delete cids.node;

  genom_log_info("shutdown complete");
  return 0;
}


/* <"[--- ${comp}_cntrl_task_signal -------------------------------------]"> */

/** \brief Signal the control task that an activity is terminated
 */

static const struct genom_activity_cbd_ {
  void operator()(genom_activity *) {}
} genom_activity_cbd = {};

void
<"$comp">_cntrl_task_signal(genom_activity *a)
{
  ros::CallbackInterfacePtr cb(a, genom_activity_cbd);
  ros::getGlobalCallbackQueue()->addCallback(cb);
}


/* <"[--- ${comp}_exec_task_signal --------------------------------------]"> */

/** \brief Signal executions tasks
 */

void
<"$comp">_exec_task_signal(void)
{
<'foreach t [$component tasks] {'>
  if (!cids.tasks.<"[$t name]">.runnable) {
    cids.tasks.<"[$t name]">.runnable = true;
<'  if {[catch {$t period}]} {'>
    cids.tasks.<"[$t name]">.wakeup = true;
    pthread_cond_signal(&cids.tasks.<"[$t name]">.signal);
<'  }'>
  }
<'}'>
}


/* --- Timer callback ------------------------------------------------------ */

void
<"$comp">_timercb(const ros::TimerEvent &e)
{
  pthread_mutex_lock(&cids.task_lock);
  cids.ticks++;
<'foreach task [$component tasks] {'>
<'  if {![catch {$task period} period]} {'>
<'
    set rate [[$component clock-rate] value]
    set ticks [expr {int([$period value]/$rate)}]
    if {![catch {$task delay} delay]} {
      set d [expr {int([$delay value]/$rate)}]
    } else {
      set d 0
    }
'>
   if (cids.tasks.<"[$task name]">.runnable)
     // &&
   // cids.ticks % <"$ticks"> == <"$d">)
     {
       cids.tasks.<"[$task name]">.wakeup = true;
       pthread_cond_signal(&cids.tasks.<"[$task name]">.signal);
     }
<'  }'>
<'}'>
  pthread_mutex_unlock(&cids.task_lock);
}


<'foreach s [$component services] {'>


<'  if {[catch {$s task} task]} {'>
/* <"[--- Control service [$s name] ----------------------------------------]"> */

bool
<"$comp">_<"[$s name]">_rqstcb(
  genom::srv::<"[$s name]">::input &in, genom::srv::<"[$s name]">::output &out)
{
  genom::srv::<"[$s name]">::locals locals;
  pthread_mutex_lock(&cids.task_lock);
  genom_event s;

  genom_log_debug("handling service %s", "<"[$s name]">");

  s = <"$comp">_<"[$s name]">_controlcb(&cids, locals, in, out);
  out.genom_exception = s != genom_ok;
 
  if (!out.genom_exception) {
    cids.run_map[<"$COMP">_<"[$s name]">_RQSTID] = true;

<'    if {[llength [$s interrupt]]} {'>
    /* interrupt incompatible activities */
    for(size_t i=0; i<genom_activities::MAX_ACTIVITIES; i++) {
      genom_activity *c = cids.activities.activities[i];
      if (!c) continue;
      switch(c->rqstid) {
<'      foreach c [$s interrupt] {'>
        case <"$COMP">_<"[$c name]">_RQSTID:
<'      }'>
          c->stop("<"[$s name]">");

          break;

        default: break;
      }
    }
<'    }'>
  }

  <"$comp">_exec_task_signal(); /* wake up sleeping activities */

  genom_log_debug("done service %s", "<"[$s name]">");
  pthread_mutex_unlock(&cids.task_lock);
  return true;
}
<'  } else {'>
/* <"[--- Execution service [$s name] --------------------------------------]"> */

template<> int
genom_activity_data_<"[$s name]">::interrupt()
{

  int wait = 0;
<'    if {[llength [$s interrupt]]} {'>
  genom_activity *c;
  size_t i;

  /* interrupt incompatible activities */
  for(i=0; i<genom_activities::MAX_ACTIVITIES; i++) {
    c = cids.activities.activities[i];
    if (!c || c->id == id) continue;
    switch(c->rqstid) {
<'      foreach c [$s interrupt] {'>
      case <"$COMP">_<"[$c name]">_RQSTID:
<'      }'>
        if (c->status == ACT_STOP ||
            c->status == ACT_RUNNING || c->status == ACT_STOPPING)
          wait = 1;
        c->stop("<"[$s name]">");
        break;

      default: break;
    }
  }
<'    }'>

  return wait;
}

void
<"$comp">_<"[$s name]">_rqstcb(
  actionlib::ServerGoalHandle< genom::action::<"[$s name]"> > &rqst)
{
  genom_activity_data_<"[$s name]"> *a;
  genom_event s;

  genom_log_debug("handling service %s", "<"[$s name]">");

  a = new genom_activity_data_<"[$s name]">(
    <"$COMP">_<"[$s name]">_RQSTID, cids.tasks.<"[$task name]">.id,
    rqst);
////inform bip of handling service
<"$comp">_<"[$s name]">_active = true;
 <"$comp">_<"[$s name]">_report_validate_to_bip = s;
  pthread_mutex_lock(&cids.task_lock);
  s = cids.activities.alloc(a);
/***copy param for bip uses*/
<"$comp">CntrlStrId *aacids=&cids;
bip_cids= aacids;

/***go*/
  if (!s)
    s = <"$comp">_<"[$s name]">_controlcb(&cids, a->locals, a->in, a->out);
  if (s) {
    a->reject();
    cids.activities.free(a);
    pthread_mutex_unlock(&cids.task_lock);
    delete a;

counter +=1 ; // 
    return;
  }
number_of_active_service +=1;

  a->accept();

  int delay = a->interrupt();

  /* update state port */
  genom_state_component *state = cids.ports.genom_state.handle.data();
  if (state->activity._length < state->activity._maximum) {
    a->stateid = state->activity._length++;
    state->activity._buffer[a->stateid].id = a->id;
    strncpy(state->activity._buffer[a->stateid].name, "<"[$s name]">",
            sizeof(state->activity._buffer[a->stateid].name));
    if (delay)
      state->activity._buffer[a->stateid].state[0] = *"";
    else
      strncpy(state->activity._buffer[a->stateid].state, <"$comp">_start,
              sizeof(state->activity._buffer[a->stateid].state));
    cids.ports.genom_state.publish();
  } else
    a->stateid = -1;

  if (delay) {
    /* wait for other activities */
    pthread_mutex_unlock(&cids.task_lock);
    return;
  }

  /* start activity */
  a->run();
  <"$comp">_exec_task_signal();

  genom_log_debug("done service %s", "<"[$s name]">");
  pthread_mutex_unlock(&cids.task_lock);
}

void
<"$comp">_<"[$s name]">_intercb(
  actionlib::ServerGoalHandle< genom::action::<"[$s name]"> > &rqst)
{
  actionlib_msgs::GoalID gid;
  genom_activity *a;

  pthread_mutex_lock(&cids.task_lock);
  gid = rqst.getGoalID();
  a = cids.activities.bygid(gid);
  if (a == NULL) {
    pthread_mutex_unlock(&cids.task_lock);
    genom_log_warn(
      "cancel request for non-existent activity %s", gid.id.c_str());
    return;
  }

  a->stop("<"[$s name]">");

  <"$comp">_exec_task_signal();
  pthread_mutex_unlock(&cids.task_lock);
}

<'  }'>
<'}'>


/* --- Activity callback --------------------------------------------------- */

void
<"$comp">_cntrl_task_activitycb(genom_activity *a)
{
  genom_state_component *state = cids.ports.genom_state.handle.data();
  genom_activity *c;
  size_t i;
  int sid;

  pthread_mutex_lock(&cids.task_lock);
  if (a->status == ACT_ETHER && a->codel == <"$comp">_ether)
    cids.run_map[a->rqstid] = true;
  a->report();
  sid = a->stateid;
  cids.activities.free(a);
  <"$comp">_exec_task_signal();

  /* update state port */
  if (sid >= 0) {
    state->activity._length--;
    memcpy(&state->activity._buffer[sid],
           &state->activity._buffer[sid+1],
           (state->activity._length - sid) *
           sizeof(state->activity._buffer[sid]));
  }

  /* check for pending activities */
  for(i=0; i<genom_activities::MAX_ACTIVITIES; i++) {
    c = cids.activities.activities[i];
    if (!c) continue;
    if (sid >= 0 && c->stateid > sid) c->stateid--;
    if (c->status != ACT_START) continue;
    if (c->interrupt()) continue;

    /* update state port */
    if (c->stateid >= 0) {
      strncpy(state->activity._buffer[c->stateid].state, <"$comp">_start,
              sizeof(state->activity._buffer[c->stateid].state));
    }

    c->run();
  }

  cids.ports.genom_state.publish();
  pthread_mutex_unlock(&cids.task_lock);
  delete a;
}
