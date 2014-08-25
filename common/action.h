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
#                                      Anthony Mallet on Thu Dec  8 2011
#
#

# check arguments
if {[llength $argv] != 1} { error "expected arguments: services" }
lassign $argv services

lang c

# compute handy shortcuts
set component [dotgen component]
set comp [$component name]
set roscomp genom_$comp
set COMP [string toupper [$component name]]
'>

#ifndef H_GROS_<"$COMP">_ACTION
#define H_GROS_<"$COMP">_ACTION

#include "ros/serialization.h"
#include "std_msgs/Header.h"
#include "actionlib_msgs/GoalID.h"

#include "<"$comp">_serialize.h"

<'foreach s $services {
  if {[$s kind] != "activity"} continue
'>
/* <"[--- Service [$s name] ------------------------------------------------]"> */

#include "<"$comp">/msg/<"[$s name]">Action.msg.md5"
#include "<"$comp">/msg/<"[$s name]">ActionGoal.msg.md5"
#include "<"$comp">/msg/<"[$s name]">ActionResult.msg.md5"
#include "<"$comp">/msg/<"[$s name]">ActionFeedback.msg.md5"

namespace genom {
  namespace action {
    struct <"[$s name]">_goal {
      typedef ::genom::srv::<"[$s name]">::input _goal_type;

      ::std_msgs::Header header;
      ::actionlib_msgs::GoalID goal_id;
      _goal_type goal;

      boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
    };

    struct <"[$s name]">_result {
      typedef ::genom::srv::<"[$s name]">::output _result_type;

      ::std_msgs::Header header;
      ::actionlib_msgs::GoalStatus status;
      _result_type result;

      boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
    };

    struct <"[$s name]">_feedback {
      struct _feedback_type {};

      ::std_msgs::Header header;
      ::actionlib_msgs::GoalStatus status;
      _feedback_type feedback;

      boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
    };

    struct <"[$s name]"> {
      typedef <"[$s name]">_goal _action_goal_type;
      typedef <"[$s name]">_result _action_result_type;
      typedef <"[$s name]">_feedback _action_feedback_type;

      _action_goal_type action_goal;
      _action_result_type action_result;
      _action_feedback_type action_feedback;

      boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
    };
  };
};

namespace ros {
  namespace message_traits {
    template<>
      struct IsMessage< ::genom::action::<"[$s name]"> > : TrueType {};
    template<>
      struct IsMessage< ::genom::action::<"[$s name]"> const > : TrueType {};
    template<>
      struct IsMessage< ::genom::action::<"[$s name]">_goal > : TrueType {};
    template<>
      struct IsMessage< ::genom::action::<"[$s name]">_goal const > : TrueType {};
    template<>
      struct IsMessage< ::genom::action::<"[$s name]">_result > : TrueType {};
    template<>
      struct IsMessage< ::genom::action::<"[$s name]">_result const > : TrueType {};
    template<>
      struct IsMessage< ::genom::action::<"[$s name]">_feedback > : TrueType {};
    template<>
      struct IsMessage< ::genom::action::<"[$s name]">_feedback const > : TrueType {};

    template<>
      struct MD5Sum< ::genom::action::<"[$s name]"> > {
      static const char *value() {
        return genom_md5_<"[$s name]">Action;
      }

      static const char* value(const ::genom::action::<"[$s name]"> &) {
        return value();
      }
    };

    template<>
      struct MD5Sum< ::genom::action::<"[$s name]">_goal > {
      static const char *value() {
        return genom_md5_<"[$s name]">ActionGoal;
      }

      static const char* value(const ::genom::action::<"[$s name]">_goal &) {
        return value();
      }
    };

    template<>
      struct MD5Sum< ::genom::action::<"[$s name]">_result > {
      static const char *value() {
        return genom_md5_<"[$s name]">ActionResult;
      }

      static const char* value(const ::genom::action::<"[$s name]">_result &) {
        return value();
      }
    };

    template<>
      struct MD5Sum< ::genom::action::<"[$s name]">_feedback > {
      static const char *value() {
        return genom_md5_<"[$s name]">ActionFeedback;
      }

      static const char* value(const ::genom::action::<"[$s name]">_feedback &) {
        return value();
      }
    };

    template<> struct DataType< ::genom::action::<"[$s name]"> > {
      static const char* value() {
        return "<"$comp">/<"[$s name]">Action";
      }

      static const char* value(const ::genom::action::<"[$s name]"> &) {
        return value();
      }
    };

    template<> struct DataType< ::genom::action::<"[$s name]">_goal > {
      static const char* value() {
        return "<"$comp">/<"[$s name]">ActionGoal";
      }

      static const char* value(const ::genom::action::<"[$s name]">_goal &) {
        return value();
      }
    };

    template<> struct DataType< ::genom::action::<"[$s name]">_result > {
      static const char* value() {
        return "<"$comp">/<"[$s name]">ActionResult";
      }

      static const char* value(const ::genom::action::<"[$s name]">_result &) {
        return value();
      }
    };

    template<> struct DataType< ::genom::action::<"[$s name]">_feedback > {
      static const char* value() {
        return "<"$comp">/<"[$s name]">ActionFeedback";
      }

      static const char* value(const ::genom::action::<"[$s name]">_feedback &) {
        return value();
      }
    };

    template<> struct Definition< ::genom::action::<"[$s name]"> > {
      static const char* value()
      {
        return "<"[string map {\n \\n\\\n} [rossrv $s]]">";
      }

      static const char* value(const ::genom::action::<"[$s name]"> &) {
        return value();
      }
    };

    template<> struct Definition< ::genom::action::<"[$s name]">_goal > {
      static const char* value()
      {
        return "<"[string map {\n \\n\\\n} [rossrv $s]]">";
      }

      static const char* value(const ::genom::action::<"[$s name]">_goal &) {
        return value();
      }
    };

    template<> struct Definition< ::genom::action::<"[$s name]">_result > {
      static const char* value()
      {
        return "<"[string map {\n \\n\\\n} [rossrv $s]]">";
      }

      static const char* value(const ::genom::action::<"[$s name]">_result &) {
        return value();
      }
    };

    template<> struct Definition< ::genom::action::<"[$s name]">_feedback > {
      static const char* value()
      {
        return "<"[string map {\n \\n\\\n} [rossrv $s]]">";
      }

      static const char* value(const ::genom::action::<"[$s name]">_feedback &) {
        return value();
      }
    };
  };

  namespace serialization {
    template<> struct Serializer< ::genom::action::<"[$s name]"> > {
      template<typename Stream, typename T>
      inline static void allInOne(Stream& stream, T m) {
        stream.next(m.action_goal);
        stream.next(m.action_result);
        stream.next(m.action_feedback);
      }
      ROS_DECLARE_ALLINONE_SERIALIZER;
    };

    template<> struct Serializer< ::genom::action::<"[$s name]">_goal > {
      template<typename Stream, typename T>
      inline static void allInOne(Stream& stream, T m) {
        stream.next(m.header);
        stream.next(m.goal_id);
        stream.next(m.goal);
      }
      ROS_DECLARE_ALLINONE_SERIALIZER;
    };

    template<> struct Serializer< ::genom::action::<"[$s name]">_result > {
      template<typename Stream, typename T>
      inline static void allInOne(Stream& stream, T m) {
        stream.next(m.header);
        stream.next(m.status);
        stream.next(m.result);
      }
      ROS_DECLARE_ALLINONE_SERIALIZER;
    };

    template<> struct Serializer< ::genom::action::<"[$s name]">_feedback > {
      template<typename Stream, typename T>
      inline static void allInOne(Stream& stream, T m) {
        stream.next(m.header);
        stream.next(m.status);
      }
      ROS_DECLARE_ALLINONE_SERIALIZER;
    };
  };
};

<'}'>

#endif /* H_GROS_<"$COMP">_ACTION */
