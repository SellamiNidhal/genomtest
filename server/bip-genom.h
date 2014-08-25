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

#include "<"$comp">_c_types.h"
#include <string.h>
#include "<"$comp">_internals.h"
/**extern bip rqst parma*/
extern volatile <"$comp">CntrlStrId *bip_cids;



extern volatile bool bip_start;

//////
extern int <"$comp">_<"$tname">_invoke(genom_event &state) ;
extern volatile bool extern_active;
<'foreach s [$component services] {'>
extern volatile int counter_<"[$s name]">;
<'    }'>
extern volatile int counter;
extern volatile int number_of_active_service;
extern volatile double ids_position;
extern volatile double position_to_go;
extern void s_sleep(float seconds) ;
const char genom_extern_weak demo_init[] = "::demo::init";
void bip_state_report(char *service, char *si, char *se, char *BIPas);

<'foreach s [$component services] {'>

// BIP State for service <"[$s name]">

typedef enum {
      <"[$s name]">undefinedBS,
<'  if {![catch {$s task} t]} {'>
<'  foreach e [$s fsm] {'>
      <"[$s name]"><"[$e name]">BS,
<' }'>
      <"[$s name]">etherBS
<' } else { '>
      <"[$s name]">controlBS
<'}'>
<'  if {[llength [$s validate]]} {'>
      <", [$s name]">validateBS
<'}'>
 } <"$comp"><"[$s name]">BIPState;
 
<'  if {![catch {$s task} t]} {'>

class Get_Set_value_<"$comp"><"[$s name]"> {
 

public :
void display_<"[$s name]"> (<"$comp"><"[$s name]">BIPState display_state_<"[$s name]">, char * from_place)

{
if (display_state_<"[$s name]">==<"[$s name]">undefinedBS) printf ("value is : <"[$s name]">undefinedBS from : %s \n", from_place);
<'  if {![catch {$s task} t]} {'>
<'  foreach e [$s fsm] {'>
   if (   display_state_<"[$s name]">==<"[$s name]"><"[$e name]">BS) printf ("value is : <"[$s name]"><"[$e name]">BS from : %s \n", from_place);
<' }'>
      if (display_state_<"[$s name]">==<"[$s name]">etherBS) printf ("value is : <"[$s name]">etherBS from : %s \n", from_place);
<' } else { '>
    if   (display_state_<"[$s name]">==<"[$s name]">controlBS) printf("value is : <"[$s name]">controlBS from : %s \n", from_place);
<'}'>
<'  if {[llength [$s validate]]} {'>
     if  (display_state_<"[$s name]">==<"[$s name]">validateBS) printf ("value is : <"[$s name]">validateBS from : %s \n", from_place);
<'}'>
}
        

       
};


extern  volatile <"$comp"><"[$s name]">BIPState <"$comp">_<"[$s name]">_bip_allow;
extern volatile genom_event <"$comp">_<"[$s name]">_report_to_bip;
<'}'>
extern volatile bool <"$comp">_<"[$s name]">_active;

<'  if {[llength [$s validate]]} {'>
extern volatile genom_event <"$comp">_<"[$s name]">_report_validate_to_bip;
<'}'>

<'}'>

<'foreach task [$component task] {'>
<'if {[llength [$task fsm]]} {'>

typedef enum {
      <"[$task name]">undefinedBS,
<'  foreach e [$task fsm] {'>
      <"[$task name]"><"[$e name]">BS,
<' }'>
<"[$task name]">etherBS
 } <"$comp"><"[$task name]">BIPState;

extern volatile <"$comp"><"[$task name]">BIPState <"$comp">_<"[$task name]">_bip_allow;
extern volatile genom_event <"$comp">_<"[$task name]">_report_to_bip;
extern bool enum_ether_<"[$task name]">(genom_event a);
extern bool enum_start_<"[$task name]">(genom_event a);

<'}'>
<'}'>


<'foreach s [$component services] {'>
extern bool enum_ether_<"[$s name]">(genom_event a);

extern bool enum_start_<"[$s name]">(genom_event a);


extern bool enum_end_<"[$s name]">(genom_event a);


extern bool enum_exec_<"[$s name]">(genom_event a);


extern bool enum_stop_<"[$s name]">(genom_event a);

<' }'>
/* === task codels ========================================================= */
<'
foreach e [$task fsm] {
  set codel [$task fsm $e]
'>
/* state <"[$e name]">  */
extern genom_event
<"$comp">_tcodel_<"$tname">_<"[$e cname]">();

<'  }'>

/* === services codels ===================================================== */

<'foreach s [$task services] {'>
/* <"[--- Service [$s name] ---------------------------------------------]"> */

<'  foreach e [$s fsm] { set codel [$s fsm $e]'>

/* state <"[$e name]"> */
extern genom_event
<"$comp">_<"[$s name]">_codel_<"[$e cname]">(data_<"[$s name]"> *a);



<'  }'>
<'  }'>


/* --- Service input and output structures --------------------------------- */

<'foreach s [$component services] {'>

/* input of <"[$s name]"> */
struct <"$comp">_<"[$s name]">_input {
<'  foreach p [$s parameters in inout] {'>
  <"[[$p type] declarator [$p name]]">;
<'  }'>
};

/* output of <"[$s name]"> */
struct <"$comp">_<"[$s name]">_output {
<'  foreach p [$s parameters out inout] {'>
  <"[[$p type] declarator [$p name]]">;
<'  }'>
};

<'  if {[llength [$s parameters local]]} {'>
/* locals of <"[$s name]"> */
struct <"$comp">_<"[$s name]">_locals {
<'    foreach p [$s parameters local] {'>
  <"[[$p type] declarator [$p name]]">;
<'    }'>
};
<'  }'>

int	genom_<"$comp">_<"[$s name]">_encode(char *buffer, int size,
		char *dst, int maxsize);
int	genom_<"$comp">_<"[$s name]">_decode(char *buffer, int size,
		char *dst, int maxsize);
<'}'>


/* --- Remote input and output structures ---------------------------------- */

<'foreach r [$component remotes] {'>

/* input of <"[$r name]"> */
struct <"$comp">_<"[$r name]">_input {
<'  foreach p [$r parameters in inout] {'>
  <"[[$p type] declarator (*[$p name])]">;
<'  }'>
};

/* output of <"[$r name]"> */
struct <"$comp">_<"[$r name]">_output {
<'  foreach p [$r parameters out inout] {'>
  <"[[$p type] declarator [$p name]]">;
<'  }'>
};

<'}'>


/* --- Exceptions ---------------------------------------------------------- */

<'foreach e [$component throws] {'>
int	genom_<"$comp">_<"[$e cname]">_encodex(char *buffer, int size,
                char *dst, int maxsize);
<'}'>
int	genom_<"$comp">_genom_unkex_encodex(char *buffer, int size, char *dst,
		int maxsize);
int	genom_<"$comp">_genom_syserr_encodex(char *buffer, int size, char *dst,
		int maxsize);

