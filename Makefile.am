#
# Copyright (c) 2012-2013 LAAS/CNRS
# All rights reserved.
#
# Permission to use, copy, modify, and distribute this software for any purpose
# with or without   fee is hereby granted, provided   that the above  copyright
# notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS  SOFTWARE INCLUDING ALL  IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR  BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR  ANY DAMAGES WHATSOEVER RESULTING  FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION,   ARISING OUT OF OR IN    CONNECTION WITH THE USE   OR
# PERFORMANCE OF THIS SOFTWARE.
#
#                                            Anthony Mallet on Tue Feb  7 2012
#
ACLOCAL_AMFLAGS=-I autoconf

biprosdir=${prefix}/${genom3sitedir}/bip-ros

# --- common ---------------------------------------------------------------

commondir=${biprosdir}
nobase_dist_common_DATA=\
	common/genrosmsg.tcl		\
	common/ros.manifest.xml		\
					\
	common/typecopy.h		\
	common/genom-serialize.h	\
	common/serialize.h		\
	common/portlib.h		\
	common/service.h		\
	common/action.h			\
	common/exception.c

commonautoconfdir=${biprosdir}/common/autoconf
dist_commonautoconf_DATA=\
	autoconf/ax_pthread.m4		\
	autoconf/ax_boost_base.m4	\
	autoconf/ax_boost_thread.m4

pkgconfigdir=	$(libdir)/pkgconfig
pkgconfig_DATA=	genom3-bip-ros.pc

bin_SCRIPTS=	rosaction


# --- server ---------------------------------------------------------------

serverdir=${biprosdir}
nobase_dist_server_DATA=\
	server/template.tcl		\
					\
	server/bip.configure.ac		\
	server/bip.Makefile.am		\
					\
	server/activities.h		\
	server/codels.mappings.h	\
	server/exec-task.h		\
	server/internals.h		\
	server/locals.h			\
	server/remote.h			\
	server/port.h			\
					\
	server/bip-genom.cc		\
	server/bip-genom.h		\
					\
	server/activities.cc		\
	server/control-task.cc		\
	server/control-codels.cc	\
	server/exec-task.cc		\
	server/main.cc			\
	server/remote.cc		\
	server/port.cc

# --- server/bip ---------------------------------------------------------------

serverbipdir=${biprosdir}
nobase_dist_serverbip_DATA=\
	server/bip/bip.Makefile		\
					\
	server/bip/model.bip		\
                                        \
	server/bip/Package.bip		\
	server/bip/Package.cpp		\
	server/bip/Package.hpp


