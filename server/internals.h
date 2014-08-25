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
set COMP [string toupper [$component name]]

lang c
'>
#ifndef H_GROS_<"$COMP">_INTERNALS
#define H_GROS_<"$COMP">_INTERNALS

#include <pthread.h>

#include "boost/shared_ptr.hpp"

#include "ros/ros.h"
#include "actionlib/server/action_server.h"

#include "<"$comp">_port.h"
#include "<"$comp">_remote.h"
#include "<"$comp">_service.h"
#include "<"$comp">_action.h"
#include "<"$comp">_locals.h"
#include "<"$comp">_exec-task.h"
#include "<"$comp">_activities.h"

/* service id */
<'set i 0; foreach s [$component services] {'>
#define <"$COMP">_<"[$s name]">_RQSTID	(<"$i">)
<' incr i '>
<'}'>
#define <"$COMP">_NRQSTID	(<"$i">)


/* --- internal state ------------------------------------------------------ */

struct <"$comp">CntrlStrId {
  int shutdown;
  ros::NodeHandle *node;
  ros::Timer clock;
  unsigned int ticks;
  pthread_mutex_t task_lock;

<'if {![catch {$component ids} ids]} {'>
  <"[$ids declarator ids]">;
<'}'>

  struct {
<'foreach p [$component ports] {'>
    genom::<"$comp">_port::<"[$p name]"> <"[$p name]">;
<'}'>
  } ports;

  struct {
<'foreach t [$component tasks] {'>
    genom_task_<"[$t name]"> <"[$t name]">;
<'}'>
  } tasks;

  struct {
<'foreach r [$component remotes] {'>
    genom::<"$comp">_remote::<"[$r name]"> <"[$r name]">;
<'}'>
  } remotes;

  struct {
<'foreach s [$component services] {'>
<'  if {[catch {$s task} t]} {'>
    ros::ServiceServer <"[$s name]">;
<'  } else {'>
    actionlib::ActionServer< genom::action::<"[$s name]"> > *<"[$s name]">;
<'  }'>
<'}'>
  } services;

  /* activities */
  genom_activities activities;
  bool run_map[<"$COMP">_NRQSTID];
};

extern <"$comp">CntrlStrId cids;


/* tasks */
int	<"$comp">_cntrl_task(const char *instance);
void	<"$comp">_cntrl_task_activitycb(genom_activity *a);

/* timer callback */
void	<"$comp">_timercb(const ros::TimerEvent &e);

/* service callbacks */
<'foreach s [$component services] {'>
<'  if {[catch {$s task} t]} {'>
bool	<"$comp">_<"[$s name]">_rqstcb(
                genom::srv::<"[$s name]">::input &in,
                genom::srv::<"[$s name]">::output &out);
<'  } else {'>
void	<"$comp">_<"[$s name]">_rqstcb(
		actionlib::ServerGoalHandle< genom::action::<"[$s name]"> > &rqst);
void	<"$comp">_<"[$s name]">_intercb(
		actionlib::ServerGoalHandle< genom::action::<"[$s name]"> > &rqst);
<'  }'>
<'}'>

/* service validation and simple codels */
<'foreach s [$component services] {'>
genom_event	<"$comp">_<"[$s name]">_controlcb(
			<"$comp">CntrlStrId *cids,
                        genom::srv::<"[$s name]">::locals &locals,
                        genom::srv::<"[$s name]">::input &in,
                        genom::srv::<"[$s name]">::output &out);
<'}'>

/* state port */
int	genom_state_<"$comp">_init(void);

/* log functions */
void	genom_log_info(const char *format, ...)
  __attribute__ ((format (printf, 1, 2)));
void	genom_log_warn(const char *format, ...)
  __attribute__ ((format (printf, 1, 2)));
void	genom_log_debug(const char *format, ...)
  __attribute__ ((format (printf, 1, 2)));

#endif /* H_GROS_<"$COMP">_INTERNALS */
