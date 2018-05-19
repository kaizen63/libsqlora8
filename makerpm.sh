#!/bin/sh
# This script cares for the rpm crap.

# if [ `id -u` -ne 0 ]
# then
#    echo This command must be run as root.
#    exit 1
# fi

if [ $# -ne 5 ]
then
    echo "Usage: $0 [path to rpm] [package] [package version] [awk] [prefix]"
    echo 'Possible reason for failure: rpm not found?'
    echo This script should preferentially be called from the Makefile only.
    exit 1
fi

RPM=$1
PACKAGE=$2
VERSION=$3
AWK=$4
PREFIX=$5
HOMEDIR=`/bin/pwd`

# export RPM=/bin/rpm-2.5.5

RPMVERSION=`${RPM} --version | ${AWK} '{ if ($3 < 3 ) print("2"); else print("3"); }'`
if [ ${RPMVERSION} -eq 2 ]
then
  echo rpm version 2.x recognized
  RPMSRCDIR=`$RPM --showrc | grep ^sourcedir | ${AWK} '{ print($3); }'`
  SPECDIR=`$RPM --showrc | grep ^specdir | $AWK '{ print($3); }'`
  BUILDDIR=`$RPM --showrc | grep ^builddir | $AWK '{ print($3); }'`
else
  echo rpm version 3.x recognized -- have to make educated guesses
  RPMTOPDIR=/usr/src/packages/
  RPMSRCDIR=${RPMTOPDIR}/SOURCES
  SPECDIR=${RPMTOPDIR}/SPECS
  BUILDDIR=${RPMTOPDIR}/BUILD

fi
SPECFILE=${SPECDIR}/${PACKAGE}.spec

if [ ! -x ${RPM} ]
then
    echo rpm should be in $RPM but not found
    exit 1
fi

cp ${HOMEDIR}/${PACKAGE}.spec ${SPECFILE}
cp ${HOMEDIR}/${PACKAGE}-${VERSION}.tar.gz ${RPMSRCDIR}/${PACKAGE}-${VERSION}.tar.gz

mkdir -p ${BUILDDIR}
cd ${BUILDDIR}
tar xzf ${HOMEDIR}/${PACKAGE}-${VERSION}.tar.gz

echo Building RPMs
${RPM} -ba ${SPECFILE}
