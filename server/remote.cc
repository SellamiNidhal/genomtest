<'
#
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
#                                      Anthony Mallet on Thu Sep 20 2012
#
#

lang c

# compute handy shortcuts
set component [dotgen component]
set comp [$component name]
set COMP [string toupper [$component name]]
'>
#include "autoconf/acheader.h"

#include "<"$comp">_internals.h"
#include "<"$comp">_typecopy.h"

<'foreach r [$component remotes] {'>

/* <"[--- Remote [$r name] ----------------------------------------------]"> */

<'
  set arg [list]
  foreach p [$r parameters] {
    switch -- [$p dir] {
      in		{ set a [[$p type] argument value [$p name]] }
      out - inout	{ set a [[$p type] argument reference [$p name]] }
      default		{ error "invalid parameter direction" }
    }
    lappend arg $a
  }
  if {[llength $arg]} { set arg [join $arg {, }] } else { set arg "void" }
'>
extern "C" genom_event
genom_<"$comp">_remote_<"[$r name]">_call(<"$arg">)
{
<'
  set l [list]
  foreach p [$r parameters] { lappend l [$p name] }
'>
  return cids.remotes.<"[$r name]">.call(<"[join $l ,]">);
}

/* connect */
genom_event
genom::<"$comp">_remote::<"[$r name]">::connect(const char remote[128])
{
  handle.call = NULL;
<'  switch -- "[$r kind]" {'>
<'    attribute - function {'>
  c = cids.node->serviceClient<
    genom::srv::<"[$r name]">::input, genom::srv::<"[$r name]">::output
    >(remote, true);
  if (!c.exists()) return genom_no_such_remote();
<'    }'>
<'    activity {'>
  if (c) delete c;
  c = new client(*cids.node, remote, &cbq);
  if (!c) return genom_no_such_remote();

  int count = 3;
  while(ros::ok() && !c->isServerConnected() && count-->0)
    cbq.callAvailable(ros::WallDuration(1.));
  if (!c->isServerConnected()) return genom_no_such_remote();
<'    }'>
<'  }'>
  handle.call = genom_<"$comp">_remote_<"[$r name]">_call;
  return genom_ok;
}

/* call */
genom_event
genom::<"$comp">_remote::<"[$r name]">::call(<"$arg">)
{
<'  foreach p [$r parameter in inout] {'>
  genom::ids::pcopy(in.<"[$p name]">, <"[[$p type] deref [$p name]]">);
<'  }'>
<'  switch -- "[$r kind]" {'>
<'    attribute - function {'>
  genom::srv::<"[$r name]">::output out;

  if (!c.call(in, out)) return genom_remote_io();

<'      foreach p [$r parameter inout out] {'>
  genom::ids::pcopy(<"[[$p type] deref [$p name]]">, out.<"[$p name]">);
<'      }'>
<'    }'>
<'    activity {'>
  boost::shared_ptr<const genom::srv::<"[$r name]">::output> out;
  client::GoalHandle gh;

  gh = c->sendGoal(in);
  while(ros::ok()) {
    cbq.callAvailable(ros::WallDuration(1.));
    if (gh.getCommState() == actionlib::CommState::DONE) break;
  }

  out = gh.getResult();
  if (!out) return genom_remote_io();

<'      foreach p [$r parameter inout out] {'>
  genom::ids::pcopy(<"[[$p type] deref [$p name]]">, out-><"[$p name]">);
<'      }'>
<'    }'>
<'  }'>
  if (out.genom_exception)
    return genom_<"$comp">_decodex(out.genom_exdetail.c_str());
  return genom_ok;
}

<'}'>
