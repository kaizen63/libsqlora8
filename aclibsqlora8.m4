dnl $Id: aclibsqlora8.m4 283 2004-05-28 03:04:15Z kpoitschke $
dnl
dnl
dnl Copyright (c) 1991-2002 Kai Poitschke (kai@poitschke.de)
dnl  
dnl 
dnl This file is part of the libsqlora8 package which can be found
dnl at http://www.poitschke.de/libsqlora8/
dnl
dnl
dnl  Permission to use, copy, modify, and distribute this software for
dnl  any purpose with or without fee is hereby granted, provided that
dnl  the above copyright notice and this permission notice appear in all
dnl  copies.
dnl
dnl  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
dnl  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
dnl  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
dnl  IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
dnl  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
dnl  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
dnl  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
dnl  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
dnl  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
dnl  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
dnl  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
dnl  SUCH DAMAGE.
dnl
dnl
dnl
dnl Defines the macros:
dnl AM_PATH_LIBSQLORA8
dnl 
dnl ACKP_LIB_SQLORA8 for backward compatibility (calls AM_PATH_LIBSQLORA8)
dnl
dnl
dnl
dnl -------------------------------------------------------------------------
dnl AM_PATH_LIBSQLORA8([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]])
dnl Test for LIBSQLORA8, and define LIBSQLORA8_CFLAGS and LIBSQLORA8_LIBS
dnl
dnl
AC_DEFUN([AM_PATH_LIBSQLORA8],
[dnl 
dnl Get the cflags and libraries from the libsqlora8-config script
dnl
dnl
AC_ARG_WITH(libsqlora8-prefix,
    AC_HELP_STRING([--with-libsqlora8-prefix=PFX], [Prefix where libsqlora8 is installed (optional)]),
            libsqlora8_config_prefix="$withval", libsqlora8_config_prefix="")
dnl
AC_ARG_WITH(libsqlora8-exec-prefix,
    AC_HELP_STRING([--with-libsqlora8-exec-prefix=PFX], [Exec prefix where libsqlora8 is installed (optional)]),
            libsqlora8_config_exec_prefix="$withval", libsqlora8_config_exec_prefix="")
dnl
AC_ARG_ENABLE(libsqlora8test, 
    AC_HELP_STRING([--disable-libsqlora8test], [Do not try to compile and run a test LIBSQLORA8 program]),
		    , enable_libsqlora8test=yes)

AC_ARG_WITH(libsqlora8-config,
    AC_HELP_STRING([--with-libsqlora8-config=path], [Path to libsqlora8-config executable (optional)]),
            LIBSQLORA8_CONFIG="$withval")

AC_ARG_WITH(pkg-config,
    AC_HELP_STRING([--with-pkg-config=path], [Path to pkg-config executable (optional)]),
            PKG_CONFIG="$withval")
dnl

  no_libsqlora8=""

  dnl check if better pkg-config is installed
  if test x$libsqlora8_config_exec_prefix != x ; then
    if test -x $libsqlora8_config_exec_prefix/bin/pkg-config ; then
      if test x${PKG_CONFIG+set} != xset ; then
        PKG_CONFIG=$libsqlora8_config_exec_prefix/bin/pkg-config
      fi
    fi
  fi

  if test x$libsqlora8_config_prefix != x ; then
    if test -x $libsqlora8_config_prefix/bin/pkg-config ; then
      if test x${PKG_CONFIG+set} != xset ; then
        PKG_CONFIG=$libsqlora8_config_prefix/bin/pkg-config
      fi
    fi
  fi

  if test -x $exec_prefix/bin/pkg-config ; then
    if test x${PKG_CONFIG+set} != xset ; then
      PKG_CONFIG=$exec_prefix/bin/pkg-config
    fi
  fi

  if test -x $prefix/bin/pkg-config ; then
    if test x${PKG_CONFIG+set} != xset ; then
      PKG_CONFIG=$prefix/bin/pkg-config
    fi
  fi

  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

  if test x$PKG_CONFIG != xno ; then
    if $PKG_CONFIG --atleast-pkgconfig-version 0.8 ; then
      LIBSQLORA8_CONFIG=no # switch off old libsqlora8-config, if we detect pkg-config
    else
      echo *** pkg-config too old; version 0.8 or better required.
      PKG_CONFIG=no
    fi
  else
    PKG_CONFIG=no
  fi

  # if we did not find PKG_CONFIG, fallback to our old libsqlora8-config
  if test x$PKG_CONFIG = xno ; then
    if test x$libsqlora8_config_exec_prefix != x ; then
       libsqlora8_config_args="$libsqlora8_config_args --exec-prefix=$libsqlora8_config_exec_prefix"
       if test x${LIBSQLORA8_CONFIG+set} != xset ; then
          LIBSQLORA8_CONFIG=$libsqlora8_config_exec_prefix/bin/libsqlora8-config
       fi
    fi

    if test x$libsqlora8_config_prefix != x ; then
       libsqlora8_config_args="$libsqlora8_config_args --prefix=$libsqlora8_config_prefix"
       if test x${LIBSQLORA8_CONFIG+set} != xset ; then
          LIBSQLORA8_CONFIG=$libsqlora8_config_prefix/bin/libsqlora8-config
       fi
    fi
    dnl check if config file is there
    AC_PATH_PROG(LIBSQLORA8_CONFIG, libsqlora8-config, no)
    if test x$LIBSQLORA8_CONFIG = xno ; then
      no_libsqlora8=yes
    fi
  fi

  min_libsqlora8_version=ifelse([$1], ,2.1.0,$1)

  AC_ARG_WITH(libsqlora8-min-version,
    AC_HELP_STRING([--with-libsqlora8-min-version=version], [Overwrite min version passed by configure ($1)]),
            min_libsqlora8_version="$withval")

  AC_MSG_CHECKING(for libsqlora8 - version >= $min_libsqlora8_version)

  
  if test x$no_libsqlora8 = x;  then
    if test x$LIBSQLORA8_CONFIG != xno ; then
      # use libsqlora8-config
      LIBSQLORA8_CFLAGS=`$LIBSQLORA8_CONFIG $libsqlora8_config_args --cflags`
      LIBSQLORA8_LIBS=`$LIBSQLORA8_CONFIG $libsqlora8_config_args --libs`
      libsqlora8_config_major_version=`$LIBSQLORA8_CONFIG $libsqlora8_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
      libsqlora8_config_minor_version=`$LIBSQLORA8_CONFIG $libsqlora8_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
      libsqlora8_config_micro_version=`$LIBSQLORA8_CONFIG $libsqlora8_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    else
      # use pkg-config
      LIBSQLORA8_CFLAGS=`$PKG_CONFIG --cflags libsqlora8`
      LIBSQLORA8_LIBS=`$PKG_CONFIG --libs libsqlora8`
      libsqlora8_config_major_version=`$PKG_CONFIG --modversion libsqlora8 | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
      libsqlora8_config_minor_version=`$PKG_CONFIG --modversion libsqlora8 | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
      libsqlora8_config_micro_version=`$PKG_CONFIG --modversion libsqlora8 | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    fi

    SQLORA8_CFLAGS=$LIBSQLORA8_CFLAGS
    SQLORA8_CPPFLAGS=$LIBSQLORA8_CFLAGS
    SQLORA8_LIBS=$LIBSQLORA8_LIBS

    if test "x$enable_libsqlora8test" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$LIBSQLORA8_CFLAGS"
      LIBS="$LIBSQLORA8_LIBS"
dnl
dnl Now check if the installed LIBSQLORA8 is sufficiently new. (Also sanity
dnl checks the results of libsqlora8-config to some extent
dnl
      rm -f conf.libsqlora8test
      AC_TRY_RUN([
#include "sqlora.h"
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.libsqlora8test");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = malloc(strlen("$min_libsqlora8_version")+1);
  strcpy(tmp_version,"$min_libsqlora8_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_libsqlora8_version");
     exit(1);
   }

  if ((sqlora8_major_version != $libsqlora8_config_major_version) ||
      (sqlora8_minor_version != $libsqlora8_config_minor_version) ||
      (sqlora8_micro_version != $libsqlora8_config_micro_version))
    {
      if (0 == strcmp("$PKG_CONFIG", "no"))
          printf(   "\n*** 'libsqlora8-config --version' returned ");
      else
          printf(   "\n*** 'pkg-config --modversion libsqlora8' returned ");

      printf(" %d.%d.%d, but libsqlora8 (%d.%d.%d)\n", 
             $libsqlora8_config_major_version, $libsqlora8_config_minor_version, $libsqlora8_config_micro_version,
             sqlora8_major_version, sqlora8_minor_version, sqlora8_micro_version);


      printf ("*** was found! If ");

      if (0 == strcmp("$PKG_CONFIG", "no"))
          printf("libsqlora8-config");
      else
          printf("pkg-config");

      printf (" was correct, then it is best\n");

      printf ("*** to remove the old version of LIBSQLORA8. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");

      if (0 == strcmp("$PKG_CONFIG", "no"))
        {
          printf("*** If libsqlora8-config was wrong, set the environment variable LIBSQLORA8_CONFIG\n");
          printf("*** to point to the correct copy of libsqlora8-config, and remove the file config.cache\n");
        }
      else
        {
          printf("*** If pkg-config was wrong, set the environment variable PKG_CONFIG\n");
          printf("*** to point to the correct copy of pkg-config, and remove the file config.cache\n");
        }
      printf("*** before re-running configure\n");
    } 
  else if ((sqlora8_major_version != SQLORA8_MAJOR_VERSION) ||
	       (sqlora8_minor_version != SQLORA8_MINOR_VERSION) ||
           (sqlora8_micro_version != SQLORA8_MICRO_VERSION))
    {
      printf("*** libsqlora8 header files (version %d.%d.%d) do not match\n",
	     SQLORA8_MAJOR_VERSION, SQLORA8_MINOR_VERSION, SQLORA8_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     sqlora8_major_version, sqlora8_minor_version, sqlora8_micro_version);
    }
  else
    {
      if ((sqlora8_major_version > major) ||
        ((sqlora8_major_version == major) && (sqlora8_minor_version > minor)) ||
        ((sqlora8_major_version == major) && (sqlora8_minor_version == minor) && (sqlora8_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of libsqlora8 (%d.%d.%d) was found.\n",
               sqlora8_major_version, sqlora8_minor_version, sqlora8_micro_version);
        printf("*** You need a version of libsqlora8 newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        if (getenv("AXN_HOME"))
          printf("*** libsqlora8 is always available from ftp://mercury.axense.com/pub/axense/packages/lib/.\n");
        else
          printf("*** libsqlora8 is always available from http://www.poitschke.de/libsqlora8/.\n");

        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the libsqlora8-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of libsqlora8, but you can also set the LIBSQLORA8_CONFIG environment to point to the\n");
        printf("*** correct copy of libsqlora8-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_libsqlora8=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_libsqlora8" = x ; then
     AC_MSG_RESULT(yes (version $libsqlora8_config_major_version.$libsqlora8_config_minor_version.$libsqlora8_config_micro_version))
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)

     if test x$PKG_CONFIG = xno -a x$LIBSQLORA8_CONFIG = xno ; then
       echo "*** The libsqlora8-config script installed by LIBSQLORA8 could not be found"
       echo "*** If LIBSQLORA8 was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the LIBSQLORA8_CONFIG environment variable to the"
       echo "*** full path to libsqlora8-config."
       echo "***"
       echo "*** Maybe you would like to switch to pkg-config, which is also supported"
       echo "*** and preferred if available."
       echo "*** You can get it here: http://www.freedesktop.org/software/pkgconfig/"
    
     else
       if test -f conf.libsqlora8test ; then
        :
       else
          echo "*** Could not run LIBSQLORA8 test program, checking why..."
          ac_save_CFLAGS="$CFLAGS"
          ac_save_LIBS="$LIBS"
          CFLAGS="$LIBSQLORA8_CFLAGS"
          LIBS="$LIBSQLORA8_LIBS"

          AC_TRY_LINK([
#include "sqlora.h"
#include <stdio.h>
],      [ return ((sqlora8_major_version) || (sqlora8_minor_version) || (sqlora8_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding LIBSQLORA8 or finding the wrong"
          echo "*** version of LIBSQLORA8. If it is not finding LIBSQLORA8, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
        ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means LIBSQLORA8 was incorrectly installed"
          echo "*** or that you have moved LIBSQLORA8 since it was installed. In the latter case, you"
          echo "*** may want to edit the libsqlora8-config script: $LIBSQLORA8_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     LIBSQLORA8_CFLAGS=""
     LIBSQLORA8_LIBS=""
     SQLORA8_CFLAGS=""
     SQLORA8_CPPFLAGS=""
     SQLORA8_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(LIBSQLORA8_CFLAGS)
  AC_SUBST(LIBSQLORA8_LIBS)
dnl
dnl backward comptibility
dnl
  AC_SUBST(SQLORA8_CPPFLAGS)
  AC_SUBST(SQLORA8_CFLAGS)
  AC_SUBST(SQLORA8_LIBS)
  rm -f conf.libsqlora8test
])


dnl
dnl backward compatibility macro
dnl ACKP_LIB_SQLORA8([MINIMUM-VERSION, [PREFIX [,ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]]])
AC_DEFUN([ACKP_LIB_SQLORA8], [
  AM_PATH_LIBSQLORA8($1, $3, $4)
]
)

dnl $Id: aclibsqlora8.m4 283 2004-05-28 03:04:15Z kpoitschke $
