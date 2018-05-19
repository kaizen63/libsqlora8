dnl $Id: acoracle.m4 336 2005-12-19 12:31:10Z kpoitschke $
dnl
dnl acoracle.m4
dnl
dnl Author: Kai Poitschke <kai.poitschke@computer.org>
dnl
dnl Defines the macros:
dnl  ACX_LD_RUNPATH_SWITCH 
dnl  AM_PATH_ORACLE
dnl
dnl  ACKP_ORACLE for backward compatibility (calls AM_PATH_ORACLE)

dnl -------------------------------------------------------------------------
dnl ACX_LD_RUNPATH_SWITCH([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
dnl Sets the variable LD_RUNPATH_SWITCH
dnl We try:
dnl -Wl,-R 
dnl -Wl,-rpath 
dnl -Wl,+s,+b
dnl If nothing works, the LD_RUNPATH_SWITCH is set to "".
dnl
AC_DEFUN([ACX_LD_RUNPATH_SWITCH], [

  if test "x$LD_RUNPATH_SWITCH" = "x"; then
    AC_CACHE_CHECK([if compiler supports -Wl,-R], ackp_cv_cc_dashr,[
    ackp_save_libs="${LIBS}"
    LIBS="-R/usr/lib ${LIBS}"
    AC_TRY_LINK([], [], ackp_cv_cc_dashr=yes, ackp_cv_cc_dashr=no)
    LIBS="${ackp_save_libs}"])
  

    if test $ackp_cv_cc_dashr = "yes"; then
        LD_RUNPATH_SWITCH="-Wl,-R"
    else
        AC_CACHE_CHECK([if compiler supports -Wl,-rpath,], ackp_cv_cc_rpath,[
                ackp_save_libs="${LIBS}"
                LIBS="-Wl,-rpath,/usr/lib ${LIBS}"
                AC_TRY_LINK([], [], ackp_cv_cc_rpath=yes, ackp_cv_cc_rpath=no)
                LIBS="${ackp_save_libs}"])
        if test $ackp_cv_cc_rpath = "yes"; then
            LD_RUNPATH_SWITCH="-Wl,-rpath,"
        else
            AC_CACHE_CHECK([if compiler supports -Wl,+s,+b,], ackp_cv_cc_plusb,[
                ackp_save_libs="${LIBS}"
                LIBS="-Wl,+s,+b,/usr/lib ${LIBS}"
                AC_TRY_LINK([], [], ackp_cv_cc_plusb=yes, ackp_cv_cc_plusb=no)
                LIBS="${ackp_save_libs}"])

            if test $ackp_cv_cc_plusb = "yes" ; then
                LD_RUNPATH_SWITCH="-Wl,+s,+b,"
            else
                LD_RUNPATH_SWITCH=""
            fi
        fi
    fi
  fi
  if test "x$LD_RUNPATH_SWITCH" != "x"; then
      # action-if-found
      ifelse([$1], , :, [$1])
  else
      # action-if-not-found
      ifelse([$2], , :, [$2])
  fi
])


dnl -------------------------------------------------------------------------
dnl ACX_ORACLE_VERSION([MINIMUM-VERSION [,ORACLE_HOME] [,ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]]])
dnl Tries to find out which oracle version is installed
dnl Returns the version in $ORACLE_VERSION
dnl Substitutes ORACLE_VERSION
dnl
AC_DEFUN([ACX_ORACLE_VERSION], [

AC_ARG_ENABLE(oraversion, 
  AC_HELP_STRING([--disable-oraversion], [Do not check for the minimum oracle version]),
  [], [enable_oraversion=yes])

AC_ARG_WITH(oraversion, AC_HELP_STRING([--with-oraversion=major.minor], 
                                       [Tells configure the oracle client version. Use this when it is not possible to detect the version automatically (client only installations).]
                                      ),
            [ackp_cv_user_oracle_version=$withval], 
            [ackp_cv_user_oracle_version=""])

  ora_min_version=$1

  if test "x$ackp_cv_user_oracle_version" = "x" ; then

    AC_CACHE_CHECK([Oracle version >= $ora_min_version], ackp_cv_oracle_version, [
    ackp_cv_oracle_version=""

    acx_orahome=ifelse([$2], , $ORACLE_HOME, $2)

    export acx_orahome

    if test -f "$acx_orahome/bin/svrmgrl" ; then
      ackp_cv_oracle_version=`$acx_orahome/bin/svrmgrl command=exit |\
                  egrep '(PL/SQL|JServer) (Release|Version)' |\
                  sed 's/  */:/g' | cut -d: -f3 | cut -c 1-7`
    else
      if test -f "$acx_orahome/bin/sqlplus" ; then
        # Oracle 8i knows sqlplus -?
        # and Oracle 9i knows only sqlplus -v 
        # Cannot use the return code, have to parse the output
        n=`$acx_orahome/bin/sqlplus -? | grep -i -c Usage`
        if test $n -eq 0 ; then
          sqlplus_opt="-?"
        else
          sqlplus_opt="-v"
        fi
        ackp_cv_oracle_version=`$acx_orahome/bin/sqlplus $sqlplus_opt |\
                      egrep '(Release|Version)' |\
                      sed 's/  */:/g' | cut -d: -f4 | cut -c 1-7`
      else
         echo "Cannot detect your oracle version. Configure with: --with-oraversion=major.minor"
      fi # endif test sqlplus
    fi  # endif test svrmgrl

    unset acx_orahome
    unset sqlplus_opt
    ])
  else
    echo "Using user specified oracle version: $ackp_cv_user_oracle_version"
    ackp_cv_oracle_version=$ackp_cv_user_oracle_version
  fi # endif test ackp_oracle_version

  ORACLE_VERSION=$ackp_cv_oracle_version
  AC_SUBST(ORACLE_VERSION)

  if test "x$ORACLE_VERSION" != "x"; then
      if test "x$enable_oraversion" = "xyes" ; then
          # split the minumum
          ora_min_major=`echo $ora_min_version. | cut -d. -f1`
          test -z "$ora_min_major" && ora_min_major=0
          ora_min_minor=`echo $ora_min_version. | cut -d. -f2`
          test -z "$ora_min_minor" && ora_min_minor=0
          ora_min_micro=`echo $ora_min_version. | cut -d. -f3`
          test -z "$ora_min_micro" && ora_min_micro=0
          ora_min_patchl=`echo $ora_min_version. | cut -d. -f4`
          test -z "$ora_min_patchl" && ora_min_patchl=0

          #split the detected version
          ora_major=`echo $ORACLE_VERSION | cut -d. -f1`
          test -z "$ora_major" && ora_major=0
          ora_minor=`echo $ORACLE_VERSION | cut -d. -f2`
          test -z "$ora_minor" && ora_minor=0
          ora_micro=`echo $ORACLE_VERSION | cut -d. -f3`
          test -z "$ora_micro" && ora_micro=0
          ora_patchl=`echo $ORACLE_VERSION | cut -d. -f4`
          test -z "$ora_patchl" && ora_patchl=0

          if test \( \( $ora_major -gt $ora_min_major \) -o \
                \( \( $ora_major -eq $ora_min_major \) -a \( $ora_minor -gt $ora_min_minor \) \) -o \
                \( \( $ora_major -eq $ora_min_major \) -a \( $ora_minor -eq $ora_min_minor \) -a \( $ora_micro -gt $ora_min_micro \) \) -o \
                \( \( $ora_major -eq $ora_min_major \) -a \( $ora_minor -eq $ora_min_minor \) -a \( $ora_micro -eq $ora_min_micro \) -a \( $ora_patchl -ge $ora_min_patchl \) \) \) ; then
              # this is the minumum required version
              # action-if-found
              ifelse([$3], , :, [$3]) 
          else
              echo "*** Sorry your Oracle version is not sufficient"
              # action-if-not-found
              ifelse([$4], , :, [$4])
          fi
      else
         # ignore version check. Do action-if-found
         ifelse([$3],,:,[$3])
      fi  
  else
      echo "*** Could not detect your oracle version"
      # action-if-not-found
      ifelse([$4], , :, [$4])
  fi

])

dnl-------------------------------------------------------------------------
dnl AM_PATH_ORACLE([MINIMUM-VERSION [, ORACLE_HOME [,ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]]])
dnl Sets all variables to support oracle
dnl Sets $ORACLE_SHLIBS, $ORACLE_STLIBS, $ORACLE_LDFLAGS
dnl $ORACLE_CPPFLAGS, $ORACLE_HOME, $ORACLE_VERSION (vial ACKP_ORACLE_VERSION)
dnl and $ORACLE_LIBDIR
dnl
dnl Requires macro ACX_LD_RUNPATH_SWITH
dnl
AC_DEFUN([AM_PATH_ORACLE], [
AC_ARG_WITH(oraclehome,
  AC_HELP_STRING([--with-oraclehome=DIR], [DIR is Oracle's installation directory, defaults to $ORACLE_HOME.]),
[ORACLEINST_TOP=$withval
 ackp_cv_user_oracle_home=yes
], 
[ORACLEINST_TOP=ifelse([$2], , $ORACLE_HOME, [$2], yes, $ORACLE_HOME, [$1])])

  NLS_LANG="American_America.WE8ISO8859P1"
  export NLS_LANG

  if test "$ORACLEINST_TOP" != ""
  then

    ACX_ORACLE_VERSION([$1], $ORACLEINST_TOP, [

    # Check for ORACLEINST_TOP/lib. If this does not exist, ORACLE_HOME
    # is set wrong, ora --with-oraclehome has a value
    if ! test -d $ORACLEINST_TOP/lib -o -d $ORACLEINST_TOP/lib64 -o -d $ORACLEINST_TOP/lib32
    then
       echo ""
       echo "Cannot find Oracle library dir  $ORACLEINST_TOP/lib[32|64]" 
       if test "x$ackp_cv_user_oracle_home" = "x"
       then
          echo "Your env. variable ORACLE_HOME points to the wrong directory"
       else
          echo "Wrong directory in --with-oraclehome=<dir>"
       fi
       echo ""
       exit 1
    fi

    # Oracle include files
    if test -f "$ORACLEINST_TOP/rdbms/public/ocidfn.h"
    then
      # V8.0.5
      ORACLE_CPPFLAGS="-I$ORACLEINST_TOP/rdbms/public"
    elif test -f "$ORACLEINST_TOP/rdbms/demo/ocidfn.h" ; then
      # V7.[0123]
      ORACLE_CPPFLAGS="-I$ORACLEINST_TOP/rdbms/demo"
    fi

    if test -d "$ORACLEINST_TOP/network/public"
    then
      # V8
      ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/network/public"
    fi

    if test -d "$ORACLEINST_TOP/plsql/public"
    then
      # V8
      ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/plsql/public"
    fi

    if test -d "$ORACLEINST_TOP/rdbms/public"
    then
      # V8
      ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/rdbms/public"
    fi

    # Oracle libs - nightmare :-)
    ORACLE_LIBDIR=$ORACLEINST_TOP/lib  # The default

    if test -r $ORACLEINST_TOP/lib32 -a x$ackp_64bit = xno
    then
       # for 64bit platforms with 32bit compatibility
       ORACLE_LIBDIR=$ORACLEINST_TOP/lib32
    else
       case $host in
          *-*mingw*)
		ORACLE_LIBDIR=$ORACLEINST_TOP/BIN
		;;
	  *)
		if test -d $ORACLEINST_TOP/lib64 -a x$ackp_64bit = xyes ; then
		    ORACLE_LIBDIR=$ORACLEINST_TOP/lib64
		else
	            ORACLE_LIBDIR=$ORACLEINST_TOP/lib
		fi
		;;
	esac
    fi

    ACX_LD_RUNPATH_SWITCH([
      ORACLE_LDFLAGS="-L$ORACLE_LIBDIR ${LD_RUNPATH_SWITCH}$ORACLE_LIBDIR"
    ], [ ORACLE_LDFLAGS="-L$ORACLE_LIBDIR" ])


    if test -f "$ORACLE_LIBDIR/sysliblist"
    then
      ORA_SYSLIB="`cat $ORACLE_LIBDIR/sysliblist`"
    else
      ORA_SYSLIB="-lm"
    fi

    # Oracle Static libs and PRO*C flags
    case $ORACLE_VERSION in
      7.0*|7.1*)
        ORACLE_STLIBS="-lsql -locic $ORACLE_LIBDIR/osntab.o \
            -lsqlnet -lora -lsqlnet -lnlsrtl -lcv6 -lcore -lnlsrtl -lcv6 \
            -lcore $ORA_SYSLIB "
        if test "`uname -s 2>/dev/null`" = "AIX"; then
            ORACLE_STLIBS="$ORACLE_STLIBS -bI:$ORACLE_HOME/lib/mili.exp"
        fi
	    ORACLE_PCC="$ORACLEINST_TOP/bin/proc"
        ORACLE_PCCINCLUDE="include=./ include=$ORACLEINST_TOP/proc/lib"
	    ORACLE_PCCFLAGS="define=SQLCA_STORAGE_CLASS=static define=SQLCA_INIT\
                         ireclen=132 oreclen=132 select_error=no ltype=none \
                         hold_cursor=yes  maxopencursors=100\
                         release_cursor=no sqlcheck=\$(ORA_SQLCHECK) \
                        \$(ORA_USERID)"
        ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/proc/lib -DSQLCA_STORAGE_CLASS=static -DSQLCA_INIT"
        ;;
      7.2*)
        ORACLE_STLIBS="-locic $ORACLE_LIBDIR/osntab.o \
            -lsqlnet -lora -lsqlnet -lora -lnlsrtl3 -lc3v6 -lcore3 -lnlsrtl3 \
            -lcore3 $ORA_SYSLIB -lcore3 $ORA_SYSLIB"
	    ORACLE_PCC="$ORACLEINST_TOP/bin/proc16"
        ORACLE_PCCINCLUDE="include=./ include=$ORACLEINST_TOP/sqllib/public"
	    ORACLE_PCCFLAGS="define=SQLCA_STORAGE_CLASS=static define=SQLCA_INIT\
                         ireclen=132 oreclen=132 select_error=no ltype=none \
                         hold_cursor=yes maxopencursors=100\
                         release_cursor=no sqlcheck=\$(ORA_SQLCHECK) \
                        \$(ORA_USERID)"

        ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/sqllib/public -DSQLCA_STORAGE_CLASS=static -DSQLCA_INIT"
        ;;
      7.3*)
        ORACLE_STLIBS="-lclient -lsqlnet -lncr -lsqlnet -lclient -lcommon \
            -lgeneric -lsqlnet -lncr -lsqlnet -lclient -lcommon -lgeneric \
            -lepc -lnlsrtl3 -lc3v6 -lcore3 -lnlsrtl3 -lcore3 -lnlsrtl3 \
            $ORA_SYSLIB -lcore3 $ORA_SYSLIB"
	    ORACLE_PCC="$ORACLEINST_TOP/bin/proc16"
        ORACLE_PCCINCLUDE="include=./ include=$ORACLEINST_TOP/precomp/public"
	    ORACLE_PCCFLAGS="define=SQLCA_STORAGE_CLASS=static define=SQLCA_INIT\
                         ireclen=132 oreclen=132 select_error=no ltype=none \
                         hold_cursor=yes parse=none maxopencursors=100\
                         release_cursor=no sqlcheck=\$(ORA_SQLCHECK) \
                        \$(ORA_USERID)"

        ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/precomp/public -DSQLCA_STORAGE_CLASS=static -DSQLCA_INIT"
        ;;
      8.0*)
        ORACLE_STLIBS="-lclient -lsqlnet -lncr -lsqlnet -lclient -lcommon \
            -lgeneric -lsqlnet -lncr -lsqlnet -lclient -lcommon -lgeneric \
            -lepc -lnlsrtl3 -lc3v6 -lcore4 -lnlsrtl3 -lcore4 -lnlsrtl3 \
            $ORA_SYSLIB -lcore3 $ORA_SYSLIB"
	    ORACLE_PCC="$ORACLEINST_TOP/bin/proc"
        ORACLE_PCCINCLUDE="include=./ include=$ORACLEINST_TOP/precomp/public"
	    ORACLE_PCCFLAGS="define=SQLCA_STORAGE_CLASS=static define=SQLCA_INIT\
                         ireclen=132 oreclen=132 select_error=no ltype=none \
                         hold_cursor=yes parse=none maxopencursors=100\
                         release_cursor=no sqlcheck=\$(ORA_SQLCHECK) \
                        \$(ORA_USERID) code=ansi_c lines=yes"
        ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/precomp/public" 
        ;;

      8.1* | 9.0* | * )
        # Don't know if the STLIBS are right :(
	case $host in
	   *-*mingw*)
	      ORACLE_STLIBS="-loci $ORA_SYSLIB"
	      ORACLE_PCCINCLUDE="include=./ include=$ORACLEINST_TOP/OCI/include"
              ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/OCI/include -D_int64=__int64"
	      ;;
           *)
              ORACLE_STLIBS="-lclntsh `cat $ORACLE_LIBDIR/ldflags` $ORA_SYSLIB"
              ORACLE_PCCINCLUDE="include=./ include=$ORACLEINST_TOP/precomp/public"
              ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/precomp/public"
	      ;;
        esac
	ORACLE_PCC="$ORACLEINST_TOP/bin/proc"
	ORACLE_PCCFLAGS="define=SQLCA_STORAGE_CLASS=static define=SQLCA_INIT\
                         ireclen=132 oreclen=132 select_error=no ltype=none \
                         hold_cursor=yes parse=none maxopencursors=100\
                         release_cursor=no sqlcheck=\$(ORA_SQLCHECK) \
                        \$(ORA_USERID) code=ansi_c lines=yes"
        ;;

    esac
  
    ##################################
    # Oracle shared libs
    ##################################
    case $ORACLE_VERSION in
      7.0*)
        # shared libs not supported
        ORACLE_SHLIBS="$ORACLE_STLIBS"
        ;;
      7.1*)
        if test -f $ORACLE_LIBDIR/liboracle.s?
        then
          ORACLE_SHLIBS="-loracle $ORA_SYSLIB"
        else
          ORACLE_SHLIBS="$ORACLE_STLIBS"
        fi
        ;;
      7.2*|7.3*)
        if test -f $ORACLE_LIBDIR/libclntsh.s?
        then
          ORACLE_SHLIBS="-lclntsh $ORA_SYSLIB"
        else
          ORACLE_SHLIBS="$ORACLE_STLIBS"
        fi
        ;;
      8.0*)
        if test -f $ORACLE_LIBDIR/libclntsh.s? -o \
                -f $ORACLE_LIBDIR/libclntsh.a # AIX
        then
          if test "$CC" = "gcc" -a "`uname -sv`" = "AIX 4"; then
            # for Oracle 8 on AIX 4
            ORA_SYSLIB="$ORA_SYSLIB -nostdlib /lib/crt0_r.o /usr/lib/libpthreads.a /usr/lib/libc_r.a -lgcc"
          fi
          ORACLE_SHLIBS="-lclntsh -lpsa -lcore4 -lnlsrtl3 -lclntsh $ORA_SYSLIB"
        else
          ORACLE_SHLIBS="$ORACLE_STLIBS"
        fi
        AC_DEFINE(HAVE_OCI8, [1], [Defined if Oracle 8 OCI was found] )
        ;;

      8.1* | 9.0* | *)
        if test -f $ORACLE_LIBDIR/libclntsh.s? -o \
                -f $ORACLE_LIBDIR/libclntsh.a # AIX
        then
          ORACLE_SHLIBS="-lclntsh $ORA_SYSLIB"
        else
          ORACLE_SHLIBS="$ORACLE_STLIBS"
        fi
        AC_DEFINE(HAVE_OCI8, [1], [Defined if Oracle 8 OCI was found] )
        ;;
    esac

    # only using shared libs right now
	ORACLE_HOME=$ORACLEINST_TOP

    if test -n "$ORACLE_SHLIBS"; then
       ackp_save_libs=$LIBS
       ackp_save_ldflags=$LDFLAGS
       ackp_save_cppflags=$CPPFLAGS
       LIBS="$ORACLE_SHLIBS $LIBS"
       LDFLAGS="$ORACLE_LDFLAGS $LDFLAGS"
       CPPFLAGS="$ORACLE_CPPFLAGS $CPPFLAGS"

       AC_CACHE_CHECK([linking with oracle shared libs ($ORACLE_SHLIBS) works], 
                      ackp_cv_oralink_shared_works, [
          AC_TRY_LINK([
#include "oci.h" 
],[
  OCIEnv *envhp;
  OCIEnvInit(&envhp, 0, 0, 0);
],
                    [ackp_cv_oralink_shared_works=yes], 
                    [ackp_cv_oralink_shared_works=no]
                     )
          ])
      # This must be possible, otherwise something is really fucked, or
      # this macro doesn't cover your Oracle release.
      if test "$ackp_cv_oralink_shared_works" = "no" ; then
         echo
         echo "Cannot link with oracle !!!"
         echo "LDFLAGS=$LDFLAGS"
         echo "LIBS=$LIBS"
         echo
         exit 1
      fi
       LIBS=$ackp_save_libs
       LDFLAGS=$ackp_save_ldflags
       CPPFLAGS=$ackp_save_cppflags
    fi

    # This should be possible at least up to Oracle 8.0.5
    # Didn't figure out 8i yet.
    if test -n "$ORACLE_STLIBS"; then
       ackp_save_libs=$LIBS
       ackp_save_ldflags=$LDFLAGS
       ackp_save_cppflags=$CPPFLAGS
       LIBS="$ORACLE_STLIBS $LIBS"
       LDFLAGS="$ORACLE_LDFLAGS $LDFLAGS"
       CPPFLAGS="$ORACLE_CPPFLAGS $CPPFLAGS"

       AC_CACHE_CHECK([linking with oracle static libs ($ORACLE_STLIBS) works], 
         ackp_cv_oralink_static_works, [
          AC_TRY_LINK([
#include "oci.h" 
],[
  OCIEnv *envhp;
  OCIEnvInit(&envhp, 0, 0, 0);
],
            [ackp_cv_oralink_static_works=yes], 
            [ackp_cv_oralink_static_works=no]
            )
          ])
       LIBS=$ackp_save_libs
       LDFLAGS=$ackp_save_ldflags
       CPPFLAGS=$ackp_save_cppflags
    fi
  AC_SUBST(ORACLE_SHLIBS)
  AC_SUBST(ORACLE_STLIBS)
  AC_SUBST(ORACLE_LDFLAGS)
  AC_SUBST(ORACLE_CPPFLAGS)
  AC_SUBST(ORACLE_HOME)
  AC_SUBST(ORACLE_LIBDIR)
  AC_SUBST(ORACLE_PCC)
  AC_SUBST(ORACLE_PCCFLAGS)
  AC_SUBST(ORACLE_PCCINCLUDE)

  #execute action-if-found
  ifelse([$3],,:,[$3])

], [
dnl didnt find the right Oracle Version execute action-if-not-found
ifelse([$4],, :, [$4])
], )
  else
    echo "*** Restart with --with-oraclehome=<path-to-oracle-home> or set \$ORACLE_HOME"
   ifelse([$4],, :, [$4])
  fi
])


dnl backward compatibiliy macro
dnl ACKP_ORACLE([MINIMUM-VERSION [, ORACLE_HOME [,ACTION-IF-FOUND [,ACTION-IF-NO
AC_DEFUN([ACKP_ORACLE], [
  AM_PATH_ORACLE($1, $2, $3, $4)
]
)

dnl $Id: acoracle.m4 336 2005-12-19 12:31:10Z kpoitschke $
