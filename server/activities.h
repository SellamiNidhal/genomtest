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
#                                      Anthony Mallet on Wed Jan 25 2012
#

if {[llength $argv] != 1} { error "expected arguments: component" }
lassign $argv component

# compute handy shortcuts
set comp [$component name]
set COMP [string toupper [$component name]]
if {[catch { $component version } version]} { set version {}}

lang c
'>
#ifndef H_GROS_<"$COMP">_ACTIVITIES
#define H_GROS_<"$COMP">_ACTIVITIES

#include <err.h>
#include <pthread.h>

#include <string>

#include "ros/ros.h"
#include "ros/callback_queue_interface.h"
#include "actionlib/server/action_server.h"


/* exception encoding/decoding */
void		genom_<"$comp">_encodex(std::string &json, genom_event ex,
			const void *exdetail);
genom_event	genom_<"$comp">_decodex(const char *json);


/* --- activities ---------------------------------------------------------- */

/* Activities start in the START state. The control task does START->RUNNING
 * when all incompatible activities are VOID, ETHER or STOPPED (intermediate
 * reply). The exec task does RUNNING->ETHER when a codel returns ether. Then
 * the control task then does ETHER->VOID (final reply). If another
 * incompatible activity must run, the control task does RUNNING->STOP. The
 * exec task does STOP->STOPPING and then STOPPING->STOPPED when a codel return
 * ether. The control task then does STOPPED->VOID ("interrupted_by" final
 * reply). */




enum genom_activity_status {
  ACT_VOID,
  ACT_START, ACT_STOP,
  ACT_RUNNING, ACT_STOPPING,
  ACT_ETHER, ACT_STOPPED
};

struct genom_activity : public ros::CallbackInterface {
  int id;
  int rqstid;
  pthread_t task;
  genom_activity_status status;
  genom_event codel;
  const void *exdetail;
  const char *interruptedby;
  int stateid;

  virtual const actionlib_msgs::GoalID gid(void) = 0;

  virtual int	interrupt(void) = 0;
  virtual void	accept(void) = 0;
  virtual void	run(void) = 0;
  virtual void	reject(void) = 0;
  virtual void	stop(const char *by) = 0;
  virtual void	report(void) = 0;

  virtual genom_event invoke(void) = 0;
virtual genom_event call_data(void)=0;

  CallResult	call(void);
};

void	<"$comp">_exec_task_signal(void);
void	<"$comp">_cntrl_task_signal(genom_activity *a);

template<class ActionSpec, class localstype>
struct genom_activity_data : genom_activity {
  typedef typename ActionSpec::_action_goal_type::_goal_type intype;
  typedef typename ActionSpec::_action_result_type::_result_type outype;

 actionlib::ServerGoalHandle<ActionSpec> rqst;


  intype in;
  outype out;
  localstype locals;

  genom_activity_data(int rqstid_, pthread_t tid,
                      actionlib::ServerGoalHandle<ActionSpec> &rqst_)
    : rqst(rqst_) {
    rqstid = rqstid_;
    task = tid;
    status = ACT_VOID;
    genom::ids::pcopy(in, *rqst_.getGoal());
  };

  const actionlib_msgs::GoalID gid(void) {
    return rqst.getGoalID();
  }

  int interrupt(void);

  void accept(void) {
    assert(status == ACT_START);
    rqst.setAccepted();
  }

  void reject(void) {
    assert(status == ACT_START);
    exdetail = genom_thrown(&codel);
    assert(codel != genom_ok);
    out.genom_exception = true;
    

    rqst.setAccepted();
    rqst.setSucceeded(out);
    status = ACT_VOID;
  }

  void run(void) {
    assert(status == ACT_START);
    status = ACT_RUNNING;
    codel = <"$comp">_start;
  }

  void stop(const char *by) {
    switch(status) {
      case ACT_START:
        status = ACT_STOPPED;
        codel = <"$comp">_ether;
        interruptedby = by;
        <"$comp">_cntrl_task_signal(this);
        break;

      case ACT_RUNNING:
        status = ACT_STOP;
        interruptedby = by;
        <"$comp">_exec_task_signal();
        break;

      default: break;
    }
  }

  void report() {
    assert(status == ACT_ETHER || status == ACT_STOPPED);

    if (status == ACT_STOPPED && codel == <"$comp">_ether) {
      genom_interrupted_detail d;
      strncpy(d.by, interruptedby, sizeof(d.by));
      d.by[sizeof(d.by)-1] = *"";
      genom_interrupted(&d);
      exdetail = genom_thrown(&codel);
    }
    if (codel == <"$comp">_ether)
      codel = genom_ok;

    out.genom_exception = codel != genom_ok;


    rqst.setSucceeded(out);
    status = ACT_VOID;
  }

  genom_event invoke(void);
genom_event call_data(void);
};

<'foreach s [$component services] {'>
<'  if {[$s kind] == "activity"} {'>
typedef genom_activity_data<
  genom::action::<"[$s name]">,
  genom::srv::<"[$s name]">::locals> genom_activity_data_<"[$s name]">;
typedef genom_activity_data<
  genom::action::<"[$s name]">,
  genom::srv::<"[$s name]">::locals> data_<"[$s name]">;
<'  }'>
<'}'>

class genom_activities {
  size_t last;

public:
  static const size_t MAX_ACTIVITIES = 32;
  genom_activity *activities[MAX_ACTIVITIES];

  genom_event alloc(genom_activity *a);
  void free(genom_activity *a);
  genom_activity *bygid(const actionlib_msgs::GoalID &gid);
};

#endif /* H_GROS_<"$COMP">_ACTIVITIES */
