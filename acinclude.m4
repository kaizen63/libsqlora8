dnl =========================================================================
dnl acinclude.m4
dnl
dnl Author: Kai Poitschke <kai.poitschke@computer.org>
dnl
dnl $Revision: 283 $
dnl
dnl Defines the macros:
dnl  ACX_HPUX
dnl  ACX_C_PRAGMA
dnl  ACX_DEBUG
dnl  ACX_PURIFY


dnl -------------------------------------------------------------------------
dnl ACX_HPUX
dnl Checks if this is an HP-UX system. In this case we define
dnl _HPUX_SOURCE.
dnl If CC is not set, we set it to c89 and CFLAGS to:
dnl +e +O3 +Oinline
dnl Sets the variable $HPUX to "yes" or "no" according to the 
dnl result
AC_DEFUN([ACX_HPUX], [

  AC_CACHE_CHECK([for HP-UX], acx_cv_hpux, [
    AC_EGREP_CPP(yes,
    [#ifdef __hpux
     yes
     #endif
    ], acx_cv_hpux=yes, acx_cv_hpux=no)
  ])


# set some hpux specific things
#
  if test "$acx_cv_hpux" = "yes" ; then

    CPPFLAGS="$CPPFLAGS -D_HPUX_SOURCE"

    AC_DEFINE(_HPUX_SOURCE, [1], [Defined when this is HP-SUCKS])
#
# Try the hpux ansi compiler.
#
    acx_save_cc=$CC
    acx_save_cflags=$CFLAGS
#   have to save it in HPUX_CC, because CC has an already cached value
    AC_CHECK_PROG(HPUX_CC, c89, c89)
    CC=$HPUX_CC
    if test "$CC" = "c89" ; then
      CFLAGS="+e +O3 +Oinline" # check for extended ansi mode
#      AC_PROG_CC_WORKS 
#
#   restore previous settings if this didn't work
#
      if test "$ac_prog_cc_works" = "no"; then
#       lets use the one found by AC_PROG_CC
        CC=$acx_save_cc
        CFLAGS=$acx_save_cflags
      fi
    fi
  fi
])dnl



dnl -------------------------------------------------------------------------
dnl ACX_C_PRAGMA(pragma, [foo], variable_name )
dnl Checks if it is valid to compile with this pragma and defines
dnl the variable. Note: You have to include this variable name
dnl into your acconfig.h!
dnl Specify the function name foo, if the pragma needs a function name
dnl
AC_DEFUN([ACX_C_PRAGMA], [
  AC_MSG_CHECKING([Checking if compiler supports pragma $1])

  AC_TRY_COMPILE([
#pragma $1 $2
static int foo(int x) {return x;}
], [
    foo(0);
   ],
     acx_cc_pragma=yes , acx_cc_pragma=no
  )
  AC_MSG_RESULT($acx_cc_pragma)
  if test $acx_cc_pragma = "yes" ; then
    AC_DEFINE($3)
  fi

])

dnl -------------------------------------------------------------------------
dnl ACX_DEBUG(yes|no)
dnl Used to set some variables in case we want to debug or not
dnl if $1 is yes, we remove all [+-]O flags from CFLAGS and
dnl add -g if possible
dnl if $1  is no, we define the preprocessor consant NDEBUG in config.h
dnl The variable ACX_DEBUG_ENABLED is set to $1.
dnl
dnl
AC_DEFUN([ACX_DEBUG], [
  AC_MSG_CHECKING(if debug is enabled)
  if test "$1" = "yes" ; then
    changequote(<<,>>)
    CFLAGS=`echo $CFLAGS | sed 's/[+-]O[a-zA-Z0-9]* *//g'`

    if test $ac_cv_prog_cc_g = "yes"; then
      CFLAGS="$CFLAGS -g"
    fi
    changequote([,])
  else
    AC_DEFINE(NDEBUG, [1], [Defined when debugging and assertions are disabled])
  fi
  ACX_DEBUG_ENABLED=$1
  AC_MSG_RESULT($ACX_DEBUG_ENABLED)
])


dnl -------------------------------------------------------------------------
dnl ACX_PURIFY(yes|no)
dnl If called with yes CCLD is substituted with
dnl purify -log-file=purify_%v_%p.log -messages=first -chain-length=12 \
dnl [-g++] $(PURIFY_OPTS) $(CC)
dnl
dnl The developer should specify his special settings in PURIFY_OPTS in
dnl Makefile.am
AC_DEFUN([ACX_PURIFY], [
  AC_MSG_CHECKING(if linking with purify is enabled)
  if test "$1" = "yes" ; then
    acx_purify_opts="-log-file=purify_%v_%p.log -messages=first -chain-length=12"
    if test "$CC" = "gcc" -o "$CC" = "g++" ; then
      acx_purify_opts="$acx_purify_opts -g++"
    fi
    CCLD="purify $acx_purify_opts \$(PURIFY_OPTS) \$(CC)"
  else
    CCLD="\$(CC)"
  fi
  export CCLD
  AC_SUBST(CCLD)
  AC_MSG_RESULT($1)
])

