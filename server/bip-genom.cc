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
set tname [$task name]
lang c
'>
#include "stdio.h"
#include "<"$comp">_bip-genom.h"
#include<time.h>
#include "<"$comp">_internals.h"
volatile <"$comp">CntrlStrId *bip_cids= 0;
   genom::srv::<"[$s name]">::locals bip_locals;
   genom::srv::<"[$s name]">::input bip_in;
   genom::srv::<"[$s name]">::output bip_out;

 volatile bool bip_start=false;
<'foreach s [$component services] {'>
volatile int counter_<"[$s name]">=1;
<'    }'>
volatile int counter=1;
volatile int number_of_active_service = 0;
volatile double ids_position=0.0;
volatile double position_to_go=0.0;
volatile bool extern_active=false;

/* invoke task codels according to state */
extern int <"$comp">_<"$tname">_invoke(genom_event &state)
{
  if (state == <"$comp">_ether) return 0;

<'  foreach e [$task fsm] {'>
  if (state == <"[$e cname]">) {
    genom_event s;
    s = <"$comp">_tcodel_<"$tname">_<"[$e cname]">();
    if (
<'    foreach y [[$task fsm $e] yield] {'>
      s == <"[$y cname]"> ||
<'    }'>
      0) {

  	  <"$comp">_<"[$task name]">_report_to_bip = s;

      state = s;
<'    foreach p [[$task fsm $e] parameters port] {'>
<'      if {"handle" ni [[$p port] kind] && "out" in [[$p port] kind]} {'>
      cids.ports.<"[$p name]">.publish();
<'      }'>
<'    }'>
      if (s != <"$comp">_sleep) <"$comp">_exec_task_signal();
      return 0;
    }
<'    if {[llength [$task throw]]} {'>
    if (
<'      foreach t [$task throw] {'>
      s == <"[$t cname]">_id ||
<'      }'>
      0) {
      state = s;
      return -1;
    }
<'    }'>


    genom_bad_transition_detail d;
    strncpy(d.from, state, sizeof(d.from));
    d.from[sizeof(d.from)-1] = *"";
    strncpy(d.to, s, sizeof(d.to));
    d.to[sizeof(d.to)-1] = *"";
    genom_log_warn(
      "bad transition from %s to %s in task %s", state, s, "<"$tname">");
    state = genom_bad_transition(&d);
    return -1;
 // }
  }

<'  }'>
  /* default actions */
  if (state == <"$comp">_start || state == <"$comp">_stop) {
    state = <"$comp">_ether;
    return 0;
  }
  if (state == <"$comp">_sleep) return 0;

  genom_log_warn("bad state %s in task %s permanent activity",
                 state, "<"$tname">");
  assert(!"internal error");
  return -1;
}

/* === task codels ========================================================= */
<'
foreach e [$task fsm] {
  set codel [$task fsm $e]
'>

/* state <"[$e name]">  */
extern genom_event
<"$comp">_tcodel_<"$tname">_<"[$e cname]">()
{
  genom_event s;
<'  # copy parameters for async codels
  if {[$codel kind] == "async"} {
    foreach p [$codel parameters ids] {'>
  <"[[$p type] declarator [$p name]_]">;
<'    }'>

<'    foreach p [$codel parameters ids] {'>
  genom::ids::pinit(<"[$p name]_">);
  genom::ids::pcopy(<"[$p name]_">, cids.ids<"[$p member]">);
<'    }'>
<'  }'>

<'  # build parameter list
  set plist [list]
  foreach p [$codel parameters] {
    switch -glob -- "[$codel kind]/[$p src]" {
      */local	{ error "task codel with service parameter" }
      */remote	{ set m "cids.remotes.[[$p remote] name].handle" }
      */port	{ set m "cids.ports.[[$p port] name].handle" }
      async/ids	{ set m [$p name]_ }
      sync/ids	{ set m "cids.ids[$p member]" }
      default	{ error "reached unreachable code" }
    }

    switch  -glob -- "[$p dir]/[$p src]" {
      in/* - */port {
        lappend plist [[$p type] pass value $m]
      }
      default {
        lappend plist [[$p type] pass reference $m]
      }
    }
  }
'>
<'if {[$codel kind] == "async"} {'>
  pthread_mutex_unlock(&cids.task_lock);
<'}'>
  s = <"[$codel invoke $plist]">;
<'if {[$codel kind] == "async"} {'>
  pthread_mutex_lock(&cids.task_lock);

<'  foreach p [$codel parameters ids] {'>
<'    if {[$p dir] != "in"} {'>
  genom::ids::pcopy(cids.ids<"[$p member]">, <"[$p name]_">);
<'    }'>
  genom::ids::pfini(<"[$p name]_">);
<'  }'>
<'}'>
  return s;
}
<'}'>

/****************end***************/
 /***sleep function**////
extern void s_sleep(float seconds) {
    /* Init. */
    time_t start_time = 0;
    time_t current_time = 0;
 
    /* Operate. */
    start_time = time(NULL);
    while(current_time-start_time+1 <= seconds) {
        current_time = time(NULL);
    }
}
<'foreach s [$component services] {'>
extern bool enum_ether_<"[$s name]">(genom_event a){

genom_event s;
bool result=false; 
s=demo_ether;
if (a==s) {result=true; 

}

return result;}

extern bool enum_start_<"[$s name]">(genom_event a){

genom_event s;
bool result=false; 
s=demo_start;
if (a==s) {result=true;
}

return result;}

extern bool enum_end_<"[$s name]">(genom_event a){

genom_event s;
bool result=false; 
s=demo_end;
if (a==s) {result=true;
}

return result;}

extern bool enum_exec_<"[$s name]">(genom_event a){

genom_event s;
bool result=false; 
s=demo_exec;
if (a==s) {result=true;

}
return result; }

extern bool enum_stop_<"[$s name]">(genom_event a){
genom_event s;
bool result=false; 
s=demo_stop;
if (a==s) {result=true;

}

return result;}
<' }'>
void
bip_state_report(char *service, char *si, char *se, char *BIPas)
{
  fprintf(stderr, "--- BIP: Service %s from state: %s, to state: %s, BIP Allow State: %s.\n", service, si, se, BIPas);
}


<'foreach s [$component services] {'>
<'  if {![catch {$s task} t]} {'>
  
volatile <"$comp"><"[$s name]">BIPState <"$comp">_<"[$s name]">_bip_allow ;
volatile  genom_event <"$comp">_<"[$s name]">_report_to_bip;
<'}'>
volatile bool <"$comp">_<"[$s name]">_active = false;

<'  if {[llength [$s validate]]} {'>
volatile genom_event <"$comp">_<"[$s name]">_report_validate_to_bip;
<'}'>

<'}'>

<'foreach task [$component task] {'>
<'if {[llength [$task fsm]]} {'>
volatile  <"$comp"><"[$task name]">BIPState <"$comp">_<"[$task name]">_bip_allow =  <"[$task name]">undefinedBS;
volatile genom_event <"$comp">_<"[$task name]">_report_to_bip;
extern bool enum_ether_<"[$task name]">(genom_event a){

genom_event s;
bool result=false; 
s=demo_ether;
if (a==s) {result=true;

}
}
extern bool enum_start_<"[$task name]">(genom_event a){

genom_event s;
bool result=false; 
s=demo_start;
if (a==s) {result=true;

}
}
<'}'>
<'}'>






/* === services codels ===================================================== */

<'foreach s [$task services] {'>
/* <"[--- Service [$s name] ---------------------------------------------]"> */

<'  foreach e [$s fsm] { set codel [$s fsm $e]'>

/* state <"[$e name]"> */
extern genom_event
<"$comp">_<"[$s name]">_codel_<"[$e cname]">(data_<"[$s name]"> *a)
{
  genom_event s;

<'  # copy parameters for async codels
  if {[$codel kind] == "async"} {
    foreach p [$codel parameters ids] {'>
  <"[[$p type] declarator [$p name]_]">;
<'    }'>

<'    foreach p [$codel parameters ids] {'>
  genom::ids::pinit(<"[$p name]_">);
  genom::ids::pcopy(<"[$p name]_">, cids.ids<"[$p member]">);
<'    }'>
<'  }'>

<'    # build parameter list
    set plist [list]
    foreach p [$codel parameters] {
      switch -glob -- "[$codel kind]/[$p src]/[$p dir]" {
        */local/*	{
          switch -- [[$p param] dir] {
            local	{ set m "a->locals.[$p name]" }
            in		{ set m "a->in.[$p name]" }
            inout - out { set m "a->out.[$p name]" }
            default	{ error "unsupported local direction" }
          }
        }
        */remote/*	{ set m "cids.remotes.[[$p remote] name].handle" }
        */port/*	{ set m "cids.ports.[[$p port] name].handle" }
        async/ids/*	{ set m [$p name]_ }
        sync/ids/*	{ set m "cids.ids[$p member]" }
        default		{ error "reached unreachable code" }
      }

      switch  -glob -- "[$p dir]/[$p src]" {
	in/* - */port {
          lappend plist [[$p type] pass value $m]
        }
	default {
	  lappend plist [[$p type] pass reference $m]
	}
      }
    }'>
<'    if {[$codel kind] == "async"} {'>
  pthread_mutex_unlock(&cids.task_lock);
<'    }'>
  s = <"[$codel invoke $plist]">;
<'    if {[$codel kind] == "async"} {'>
  pthread_mutex_lock(&cids.task_lock);

<'      foreach p [$codel parameters ids] {'>
<'        if {[$p dir] != "in"} {'>
  genom::ids::pcopy(cids.ids<"[$p member]">, <"[$p name]_">);
<'        }'>
  genom::ids::pfini(<"[$p name]_">);
<'      }'>
<'    }'>
  return s;
}
<'  }'>
<'  }'>

