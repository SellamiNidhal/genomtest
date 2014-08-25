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
#                                      Anthony Mallet on Mon Dec 19 2011
#

if {[llength $argv] != 1} { error "expected arguments: component" }
lassign $argv component

# compute handy shortcuts
set comp [$component name]
set COMP [string toupper [$component name]]
if {[catch { $component version } version]} { set version {}}

lang c
'>
#include "autoconf/acheader.h"

#include "<"$comp">_internals.h"
#include "<"$comp">_typecopy.h"

#include "<"$comp">_bip-genom.h"


/* --- genom_abort_activity_codel ------------------------------------------ */

genom_event
genom_abort_activity_codel(uint32_t activity)
{
  struct genom_activity *a;

  if (activity >= genom_activities::MAX_ACTIVITIES)
    return genom_no_such_activity();

  a = cids.activities.activities[activity];
  if (!a) return genom_no_such_activity();
  a->stop("abort_activity");

  return genom_ok;
}


/* --- genom_connect_port_codel -------------------------------------------- */

/** Implement connect_port service codel */
genom_event
genom_connect_port_codel(const char local[128], const char remote[128])
{
<'foreach p [$component ports in simple] {'>
  if (!strcmp(local, "<"[$p name]">")) {
    if (cids.ports.<"[$p name]">.subscribe(cids.node, remote))
      return genom_no_such_outport();
    return genom_ok;
  }
<'}'>
<'foreach p [$component ports in multiple] {'>
  if (!strncmp(local, "<"[$p name]">/", sizeof("<"[$p name]">/")-1)) {
    if (cids.ports.<"[$p name]">.subscribe(
          cids.node, local + sizeof("<"[$p name]">/")-1, remote))
      return genom_no_such_outport();
    return genom_ok;
  }
<'}'>

  return genom_no_such_inport();
}


/* --- genom_connect_remote_codel ------------------------------------------ */

genom_event
genom_connect_remote_codel(const char local[128], const char remote[128])
{
  /* find the remote */
<'foreach r [$component remotes] {'>
  if (!strcmp(local, "<"[$r name]">")) {
    return cids.remotes.<"[$r name]">.connect(remote);
  }
<'}'>

  return genom_no_such_remote();
}


/* --- genom_kill_codel ---------------------------------------------------- */

genom_event
genom_kill_codel(void)
{
  cids.shutdown = 1;
  return genom_ok;
}

<'foreach s [$component services] {'>


/* <"[--- Service [$s name] control codels ------------------------------]"> */

genom_event
<"$comp">_<"[$s name]">_controlcb(
  <"$comp">CntrlStrId *cids,
  genom::srv::<"[$s name]">::locals &locals,
  genom::srv::<"[$s name]">::input &in,
  genom::srv::<"[$s name]">::output &out)
{
  genom_event s = genom_ok;
double _locals,_in,_in_out,_ids;

  /* check allowance (before/after statements) */
<'  foreach other [$s after] {'>
  if (!cids->run_map[<"$COMP">_<"[$other name]">_RQSTID])
    return genom_disallowed();
<'}'>
<'  foreach other [$s before] {'>
  if (cids->run_map[<"$COMP">_<"[$other name]">_RQSTID])
    return genom_disallowed();
<'}'>

  /* copy inout parameters to output */
<'  foreach p [$s parameters inout] {'>
  genom::ids::pcopy(out.<"[$p name]">, in.<"[$p name]">);

<'  }'>


  /* call validate codel */
/***update cids*////
bip_cids= cids;
<'  if {[llength [$s validate]]} {'>
<'    # build parameter list
    set plist [list]
    foreach p [[$s validate] parameters] {
      switch -- [$p src] {
        ids	{ set m "cids->ids[$p member]" 
                  set y "cids->ids[$p member]"}
        local	{
          switch -- [[$p param] dir] {
            local	{ set m "locals.[$p name]"
                           }
            in		{ set m "in.[$p name]" 
                          set param_in "in.[$p name]" }
            inout - out { set m "out.[$p name]"
                          set z "out.[$p name]" }
            default	{ error "unsupported codel parameter" }
          }
        }
        default	{ error "unsupported codel parameter" }
      }
      switch -- [$p dir] {
	in		{ lappend plist [[$p type] pass value $m] }
	inout - out	{ lappend plist [[$p type] pass reference $m] }
        default		{ error "unsupported direction" }
      }
    }'>
  
/****build param for bip uses***///

<'  if {$y in $plist} {'>
_ids = <" $y">;
ids_position=_ids;
printf (" ///**Ros : Actual state Position is : %f \n",_ids);
<'  }'>

<'  if {$param_in in $plist} {'>
_in = <" $param_in">;
 position_to_go=_in;
printf (" ///**Ros : Value to Go is : %f \n",_in);
<'  }'>

s = <"[[$s validate] invoke $plist]">;


  /* report to BIP the result of the validate codel */
 
  genom_log_debug("service %s validation returned %s",
                  "<"[$s name]">", s?s:"ok");
  if (s) return s;
<'  }'>

  /* copy ids parameters to ids (attributes) */
<'  foreach p [$s parameters in inout] {'>
<'    if {[$p src] == "ids"} {'>
  genom::ids::pcopy(cids->ids<"[$p member]">, in.<"[$p name]">);
<'    }'>
<'  }'>
<'  foreach p [$s parameters out inout] {'>
<'    if {[$p src] == "ids"} {'>
  genom::ids::pcopy(out.<"[$p name]">, cids->ids<"[$p member]">);
<'    }'>
<'  }'>

  /* call simple codels */
<'foreach c [$s codels simple] {'>
<'  # build parameter list
    set plist [list]
    foreach p [$c parameters] {
      switch -- [$p src] {
        ids	{ set m "cids->ids[$p member]" }
        local	{
          switch -- [[$p param] dir] {
            local	{ set m "locals.[$p name]" }
            in		{ set m "in.[$p name][$p member]" }
            inout - out { set m "out.[$p name][$p member]" }
            default	{ error "unsupported local direction" }
          }
        }
        default	{ error "unsupported codel parameter" }
      }
      switch -- [$p dir] {
	in		{ lappend plist [[$p type] pass value $m] }
	inout - out	{ lappend plist [[$p type] pass reference $m] }
        default		{ error "unsupported direction" }
      }
    }'>
  s = <"[$c invoke $plist]">;
  genom_log_debug("service %s codel %s returned %s",
                  "<"[$s name]">", "<"[$c name]">", s?s:"ok");
  if (s
<'  foreach t [$s throw] {'>
    && s != <"[$t cname]">_id
<'  }'>
    ) {
    genom_unkex_detail d;
    strncpy(d.what, s, sizeof(d.what)); d.what[sizeof(d.what)-1] = *"";
    genom_log_warn(
      "unknown exception %s for codel %s in service %s",
      s, "<"[$c name]">", "<"[$s name]">");
    s = genom_unkex(&d);
  }
  if (s) return s;
<'  }'>

  return s;
}
<'}'>
