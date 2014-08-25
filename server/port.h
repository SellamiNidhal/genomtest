<'
#
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
#                                      Anthony Mallet on Thu Dec 15 2011
#
#

lang c

# compute handy shortcuts
set component [dotgen component]
set comp [$component name]
set roscomp genom_$comp
set COMP [string toupper [$component name]]
'>
#ifndef H_GROS_<"$COMP">_PORT
#define H_GROS_<"$COMP">_PORT

#include "ros/master.h"

#include "<"${comp}">_c_types.h"
#include "<"${comp}">_portlib.h"


/* === Handle functions ==================================================== */

extern "C" {
<'foreach p [$component ports] {'>
<'  switch -- [$p kind] {'>
<'    simple {'>

  <"[[$p datatype] argument reference]">
		genom_<"$comp">_<"[$p name]">_data(void);

<'      switch -- [$p dir] {'>
<'        in {'>
  genom_event	genom_<"$comp">_<"[$p name]">_read(void);
<'        }'>
<'        out {'>
  genom_event	genom_<"$comp">_<"[$p name]">_write(void);
<'        }'>
<'      }'>

<'    }'>
<'    multiple {'>

  <"[[$p datatype] argument reference]">
		genom_<"$comp">_<"[$p name]">_data(const char *id);
  genom_event	genom_<"$comp">_<"[$p name]">_open(const char *id);
  genom_event	genom_<"$comp">_<"[$p name]">_close(const char *id);

<'      switch -- [$p dir] {'>
<'        in {'>
  genom_event	genom_<"$comp">_<"[$p name]">_read(const char *id);
<'        }'>
<'        out {'>
  genom_event	genom_<"$comp">_<"[$p name]">_write(const char *id);
<'        }'>
<'      }'>

<'    }'>
<'    default { error "unknown port kind" }'>
<'  }'>
<'}'>
}


/* --- ports --------------------------------------------------------------- */

namespace genom {
  namespace <"$comp">_port {

<'foreach p [$component ports in] {'>
    struct <"[$p name]">_handle {
      boost::shared_ptr<genom::port::<"[$p name]"> const> d;
      ros::Subscriber s;

      void
      datacb(const boost::shared_ptr<genom::port::<"[$p name]"> const> &data);

      inline int
      subscribe(ros::NodeHandle *node, const char *name) {
        std::vector<ros::master::TopicInfo> topics;
        int i, e;
        std::string absname = ros::names::resolve(name);
        if (d) d.reset();
        if (s) s.shutdown();

        if (!ros::master::getTopics(topics)) return 1;
        for(i=e=0; (unsigned)i<topics.size(); i++) {
          if (!topics[i].name.compare(absname)) {
            e = 1;
            break;
          }
        }
        if (!e) return 1;

        s = node->subscribe(
          absname, 1, &genom::<"$comp">_port::<"[$p name]">_handle::datacb, this);
        return !s;
      }

      inline int
      init(ros::NodeHandle *node) {
        return 0;
      }

      inline void
      fini(void) {
        if (d) d.reset();
      }
    };
<'}'>

<'foreach p [$component ports in simple] {'>
    struct <"[$p name]"> : <"[$p name]">_handle {
      <"[[$p type] declarator handle]">;

      <"[$p name]">() {
        handle.data = genom_<"$comp">_<"[$p name]">_data;
        handle.read = genom_<"$comp">_<"[$p name]">_read;
      }
    };
<'}'>
<'foreach p [$component ports in multiple] {'>
    struct <"[$p name]"> {
      <"[[$p type] declarator handle]">;

      struct h {
        char *id;
        <"[$p name]">_handle p;
      } **d;
      int nh;

      <"[$p name]">() {
        d = NULL;
        nh = 0;
        handle.data = genom_<"$comp">_<"[$p name]">_data;
        handle.read = genom_<"$comp">_<"[$p name]">_read;
      }

      inline <"[$p name]">_handle *
      find(const char *id) {
        int i;
        for(i = 0; i < nh; i++)
          if (d[i] && !strcmp(d[i]->id, id))
            return &d[i]->p;
        return NULL;
      }

      inline int
      open(ros::NodeHandle *node, const char *id) {
        h **p;
        int i;

        if (find(id) != NULL) return EEXIST;

        for(i=0; i<nh; i++) if (!d[i]) break;
        if (i >= nh) {
          p = (h **)realloc(d, (nh+1)*sizeof(*p));
          if (!p) return ENOMEM;
          d = p;
          nh++;
        }

        d[i] = new h;
        if (!d[i]) return -1;
        d[i]->id = strdup(id);
        if (d[i]->p.init(node)) {
          delete d[i];
          d[i] = NULL;
          return ENOMEM;
        }

        return 0;
      }

      inline void
      close(const char *id) {
        int i;
        for(i = 0; i < nh; i++)
          if (d[i] && !strcmp(d[i]->id, id)) {
            d[i]->p.fini();
            delete d[i];
            d[i] = NULL;
            return;
          }
      }

      inline int
      subscribe(ros::NodeHandle *node, const char *id, const char *name) {
        <"[$p name]">_handle *p = find(id);
        int s;
        if (!p) {
          s = open(node, id);
          if (s) return s;
          p = find(id);
          if (!p) return ENOENT;
        }
        return p->subscribe(node, name);
      }

      inline int
      init(ros::NodeHandle *node, const char *id) {
        <"[$p name]">_handle *p = find(id);
        if (!p) return ENOENT;
        return p->init(node);
      }

      inline void
      fini() {
        int i;
        for(i = 0; i < nh; i++) {
          if (!d[i]) continue;
          d[i]->p.fini();
          delete d[i];
        }
        if (d) free(d);
        nh = 0;
      }
    };
<'}'>

<'foreach p [$component ports out] {'>
    struct <"[$p name]">_handle {
      <"[[$p type] declarator handle]">;
      genom::port::<"[$p name]"> d;
      ros::Publisher p;

      inline void
      publish(void) {
        p.publish(d);
      }

      inline void
      fini(void) {
        p.shutdown();
      }
    };
<'}'>

<'foreach p [$component ports out simple] {'>
    struct <"[$p name]"> : <"[$p name]">_handle {
      <"[[$p type] declarator handle]">;

      <"[$p name]">() {
        handle.data = genom_<"$comp">_<"[$p name]">_data;
        handle.write = genom_<"$comp">_<"[$p name]">_write;
      }

      inline int
      init(ros::NodeHandle *node) {
        p = node->advertise<genom::port::<"[$p name]"> >(
          "<"[$p name]">", 1, true);
        return !p;
      }
    };
<'}'>
<'foreach p [$component ports out multiple] {'>
    struct <"[$p name]"> {
      <"[[$p type] declarator handle]">;

      struct h {
        char *id;
        <"[$p name]">_handle p;
      } **d;
      int nh;

      <"[$p name]">() {
        d = NULL;
        nh = 0;
        handle.data = genom_<"$comp">_<"[$p name]">_data;
        handle.write = genom_<"$comp">_<"[$p name]">_write;
        handle.open = genom_<"$comp">_<"[$p name]">_open;
        handle.close = genom_<"$comp">_<"[$p name]">_close;
      }

      inline <"[$p name]">_handle *
      find(const char *id) {
        int i;
        for(i = 0; i < nh; i++)
          if (d[i] && !strcmp(d[i]->id, id))
            return &d[i]->p;
        return NULL;
      }

      inline int
      open(ros::NodeHandle *node, const char *id) {
        h **p;
        int i;

        if (find(id) != NULL) return EEXIST;

        for(i=0; i<nh; i++) if (!d[i]) break;
        if (i >= nh) {
          p = (h **)realloc(d, (nh+1)*sizeof(*p));
          if (!p) return ENOMEM;
          d = p;
          nh++;
        }

        d[i] = new h;
        if (!d[i]) return -1;
        d[i]->id = strdup(id);
        if (init(node, id)) {
          delete d[i];
          d[i] = NULL;
          return ENOMEM;
        }

        return 0;
      }

      inline void
      close(const char *id) {
        int i;
        for(i = 0; i < nh; i++)
          if (d[i] && !strcmp(d[i]->id, id)) {
            d[i]->p.fini();
            delete d[i];
            d[i] = NULL;
            return;
          }
      }

      inline void
      publish(const char *id) {
        <"[$p name]">_handle *p = find(id);
        if (p) p->publish();
      }

      inline int
      init(ros::NodeHandle *node, const char *id) {
        <"[$p name]">_handle *p = find(id);
        if (!p) return ENOENT;

        std::string n = std::string("<"[$p name]">/") + id;
        p->p = node->advertise<genom::port::<"[$p name]"> >(n, 1, true);
        return !p->p;
      }

      inline void
      fini() {
        int i;
        for(i = 0; i < nh; i++) {
          if (!d[i]) continue;
          d[i]->p.fini();
          delete d[i];
        }
        if (d) free(d);
        nh = 0;
      }
    };
<'}'>
  };  
};

#endif /* H_GROS_<"$COMP">_PORT */
