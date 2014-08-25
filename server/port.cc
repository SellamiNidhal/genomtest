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
#                                      Anthony Mallet on Thu Feb 16 2012
#

if {[llength $argv] != 1} { error "expected arguments: component" }
lassign $argv component

# compute handy shortcuts
set comp [$component name]

lang c
'>
#include "autoconf/acheader.h"

#include "<"$comp">_internals.h"

<'foreach p [$component ports in] {'>

/* <"[--- Port [$p name] ------------------------------------------------]"> */

void
genom::<"$comp">_port::<"[$p name]">_handle::datacb(
  const boost::shared_ptr<genom::port::<"[$p name]"> const> &data) {
  pthread_mutex_lock(&cids.task_lock);
  d = data;
  pthread_mutex_unlock(&cids.task_lock);
}

<"[[$p datatype] argument reference]">
genom_<"$comp">_<"[$p name]">_data(
<'  if {[$p kind] == "multiple"} {'>
  const char *id
<'  }'>
  )
{
  genom::<"$comp">_port::<"[$p name]">_handle *h;

<'  if {[$p kind] == "multiple"} {'>
  h = cids.ports.<"[$p name]">.find(id);
<'  } else {'>
  h = &cids.ports.<"[$p name]">;
<'  }'>
  if (!h || !h->d) return NULL;
  return const_cast<<"[[$p datatype] argument reference]">>(
    <"[[$p datatype] pass reference h->d->v]">);
}

genom_event
genom_<"$comp">_<"[$p name]">_read(
<'  if {"multiple" in [$p kind]} {'>
  const char *id
<'  }'>
  )
{
<'  if {[$p kind] == "multiple"} {'>
  if (!cids.ports.<"[$p name]">.find(id)) {
     genom_syserr_detail d = { ENOENT };
     return genom_syserr(&d);
  }
<'  }'>
  return genom_ok;
}

<'}'>

<'foreach p [$component ports out] {'>

/* <"[--- Port [$p name] ------------------------------------------------]"> */

<"[[$p datatype] argument reference]">
genom_<"$comp">_<"[$p name]">_data(
<'  if {[$p kind] == "multiple"} {'>
  const char *id
<'  }'>
  )
{
  genom::<"$comp">_port::<"[$p name]">_handle *h;

<'  if {[$p kind] == "multiple"} {'>
  h = cids.ports.<"[$p name]">.find(id);
<'  } else {'>
  h = &cids.ports.<"[$p name]">;
<'  }'>
  if (!h) return NULL;
  return <"[[$p datatype] pass reference h->d.v]">;
}

genom_event
genom_<"$comp">_<"[$p name]">_write(
<'  if {"multiple" in [$p kind]} {'>
  const char *id
<'  }'>
  )
{
<'  if {[$p kind] == "multiple"} {'>
  cids.ports.<"[$p name]">.publish(id);
<'  } else {'>
  cids.ports.<"[$p name]">.publish();
<'  }'>
  return genom_ok;
}

<'  if {"multiple" in [$p kind]} {'>
genom_event
genom_<"$comp">_<"[$p name]">_open(const char *id)
{
  return cids.ports.<"[$p name]">.open(cids.node, id);
}

genom_event
genom_<"$comp">_<"[$p name]">_close(const char *id)
{
  cids.ports.<"[$p name]">.close(id);
  return genom_ok;
}
<'  }'>

<'}'>


/* --- state port ---------------------------------------------------------- */

int
genom_state_<"$comp">_init(void)
{
  genom_state_component *data = cids.ports.genom_state.handle.data();

  /* task list */
<'
set max [[[dotgen types ::genom::state::component::task] type] length]
set tasks [lrange [$component tasks] 0 ${max}-1]
'>
  data->task._length = <"[llength $tasks]">;
<'foreach t $tasks {'>
  strncpy(data->task._buffer[<"[lsearch $tasks $t]">].name, "<"[$t name]">",
          sizeof(data->task._buffer[<"[lsearch $tasks $t]">].name));
  cids.tasks.<"[$t name]">.stateid = <"[lsearch $tasks $t]">;
<'}'>

  /* automatic versioning - nice idea borrowed from ROS */
  strncpy(data->digest, "<"[$component digest]">", sizeof(data->digest));
  strncpy(data->date, "<"[clock format [clock sec]]">", sizeof(data->date));
  strncpy(data->version, "<"[$component version]">", sizeof(data->version));

  cids.ports.genom_state.publish();

  genom_log_info("initialized outport genom_state");
  return 0;
}
