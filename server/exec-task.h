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
#                                      Anthony Mallet on Fri Jan 27 2012
#

if {[llength $argv] != 1} { error "expected arguments: component" }
lassign $argv component

# compute handy shortcuts
set comp [$component name]
set COMP [string toupper [$component name]]
if {[catch { $component version } version]} { set version {}}

lang c
'>
#ifndef H_GROS_<"$COMP">_EXEC_TASK
#define H_GROS_<"$COMP">_EXEC_TASK

#include <pthread.h>

struct <"$comp">CntrlStrId;

<'foreach t [$component tasks] {'>

struct genom_task_<"[$t name]"> {
  pthread_t id;
  int stateid;
  pthread_cond_t signal;
  bool runnable, wakeup;
  genom_event status, pact;

  void	spawn(void);
};
<'}'>


#endif /* H_GROS_<"$COMP">_EXEC_TASK */
