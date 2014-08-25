#
# Copyright (c) 2012-2013 LAAS/CNRS
#
# GenoM3/ros template autoconf file
#
AC_PREREQ(2.62)
AC_INIT([genom3-bip-ros],[1.1],[openrobots@laas.fr])

AC_CONFIG_AUX_DIR([autoconf])
AC_CONFIG_MACRO_DIR([autoconf])
AM_INIT_AUTOMAKE([foreign no-define])

# Compilers

# override and disable libtool's deplibs_check_method:
lt_cv_deplibs_check_method=pass_all
# The default is already "pass_all" anyway on most platforms and this is needed
# at least on NetBSD + libtool-2.4.2 to handle correctly -l:/path/to/library.so
# dependency style in ros pkg-config .pc files.
# See http://debbugs.gnu.org/cgi/bugreport.cgi?bug=14709

LT_INIT
AC_PROG_CXX
AC_PROG_INSTALL
AX_PTHREAD(, [AC_MSG_ERROR([pthread support not available])])

# boost
AX_BOOST_BASE([1.40],, [AC_MSG_ERROR([boost not available])])

# genom
PKG_CHECK_MODULES(genom3, [genom3 >= 2.99.21])
_PKG_CONFIG(genom3prefix, [variable=prefix], [genom3 >= 2.99.21])
_PKG_CONFIG(genom3tmpldir, [variable=tmpldir], [genom3 >= 2.99.21])
_PKG_CONFIG(genom3sitedir, [variable=sitetmpldir], [genom3 >= 2.99.21])
_PKG_CONFIG(genom3, [variable=GENOM_TOOL], [genom3 >= 2.99.21])
AC_SUBST(genom3tmpldir, [$pkg_cv_genom3tmpldir])
AC_SUBST(genom3sitedir, [${pkg_cv_genom3sitedir##$pkg_cv_genom3prefix/}])
AC_SUBST(genom3, [${pkg_cv_genom3}])

if test "x$prefix" = xNONE; then
  AC_MSG_CHECKING([default prefix...])
  prefix=$pkg_cv_genom3prefix
  AC_MSG_RESULT([$prefix])
fi

dnl python
AC_ARG_VAR(PYTHON, [python interpreter])
AC_PATH_PROGS(PYTHON, [python python2.7 python2.6])
if ! test -x "$PYTHON"; then AC_MSG_FAILURE([python not found]); fi

dnl ROS
PKG_CHECK_MODULES(ROS, [roscpp >= 1.8.15])
AC_MSG_CHECKING([rosaction dependencies])
if $PKG_CONFIG --exists --print-errors '
    genpy >= 0.3.7
    roslib >= 1.8.10
    topic_tools >= 1.8.15
    rosgraph_msgs >= 1.8.15
    actionlib >= 1.8.7'; then
  AC_MSG_RESULT([ok])
else
  AC_MSG_RESULT([no])
  AC_MSG_FAILURE([missing ros dependencies])
fi

# work out the rpath for -Ldirs and bare .so files found in ros .pc Libs:
for arg in $ROS_LIBS; do
  case "$arg" in
    -L*)  arg=-R${arg#-L};;
    -l:/*.so) arg=${arg#-l:} && arg=-R${arg%/*};;
  esac
  case " $ROS_LIBS " in
    *" $arg "*) ;;
    *) ROS_LIBS="$ROS_LIBS $arg";;
  esac
done

# output
AC_CONFIG_FILES([
	genom3-bip-ros.pc
	rosaction
	Makefile
])
AC_OUTPUT
