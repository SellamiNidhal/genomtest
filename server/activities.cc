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
#                                      Anthony Mallet on Mon Jan 16 2012
#

if {[llength $argv] != 1} { error "expected arguments: component" }
lassign $argv component

# compute handy shortcuts
set comp [$component name]

lang c
'>
#include "autoconf/acheader.h"

#include <assert.h>
#include <err.h>

#include "<"$comp">_internals.h"


/* --- genom_activity::call ------------------------------------------------ */

genom_activity::CallResult
genom_activity::call()
{
  <"$comp">_cntrl_task_activitycb(this);
  return Success;
}


/* --- genom_activities::alloc --------------------------------------------- */

genom_event
genom_activities::alloc(genom_activity *a)
{
  size_t id, i;

  /* look for the first free activity */
  for(i = 0; i<MAX_ACTIVITIES; i++) {
    id = (last + i+1) % MAX_ACTIVITIES;
    if (activities[id] == NULL) break;
  }

  if (/*activity*/ i == MAX_ACTIVITIES)
    return genom_too_many_activities();
  last = id;

  /* initialize activity */
  a->id = id;
  a->status = ACT_START;
  activities[id] = a;

  return genom_ok;
}


/* --- genom_activities::free ---------------------------------------------- */

void
genom_activities::free(genom_activity *a)
{
  assert(activities[a->id] == a);
  assert(a->status == ACT_VOID);

  activities[a->id] = NULL;
}


/* --- genom_activities::bygid --------------------------------------------- */

genom_activity *
genom_activities::bygid(const actionlib_msgs::GoalID &gid)
{
  size_t id;

  /* iterate through allocated activities */
  for(id = 0; id<MAX_ACTIVITIES; id++) {
    if (activities[id] == NULL) continue;
    if (activities[id]->gid().id == gid.id)
      return activities[id];
  }

  return NULL;
}
