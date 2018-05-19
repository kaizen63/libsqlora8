
# libsqlora 8 - A simple C-Interface to access Oracle databases.

**NOTE: THIS LIBRARY IS DEPRECATED** 

libsqlora8 is simple C-library to access Oracle databases via the OCI interface. Makes simple programming tasks much easier as with plain OCI. 
If you are a C++ programmer and want to design strict OO programms, you better look at the OTL library developed by Sergei Kuchin.
The library was developed on SuSE GNU/Linux. The library should compile on every Unix system. Also some Windows users reported successfull installations. 
Version:

Current stable release is 2.3.3 (Updated: 19.12.2005).
Features:

Don't need to learn the complex Oracle OCI API.
Easy interface to Oracles OCI interface. You get the performance but you don't need to know all the details.
Reopen of cursors with different bind variables is possible (only one parse necessary).
Multiple database connections
Build-in trace facility.
Processing of array inserts.
Automatic array fetch.
Supports reading and writing LOBS.
Thread safe. Needs POSIX or the Oracle Threads package.
Builds as static and shared library. Note: Binding with Oracles static libraries is always difficult. If you know how to do it right, please send me a mail.
Download:

## Version
2.3.3	libsqlora8-2.3.3.tar.gz

## Installation:

Make sure your oracle environment is set up properly (e.g. ORACLE_HOME)
$> gunzip -c libsqlora8-2.3.0.tar.gz | tar xf -
$> cd libsqlora8-2.3.0
$> LD_LIBRARY_PATH=$ORACLE_HOME/lib
$> export LD_LIBRARY_PATH
$>./configure
$> make
$> make install
The library is build as a shared and static. When using this library add -lsqlora8 -L$ORACLE_HOME/lib -lclntsh to your LDFLAGS. Don't try to link static. It's horrible to find out the link sequence of the Oracle static libraries :(
Note: You have to set/append your LD_LIBRARY_PATH variable to/with $ORACLE_HOME/lib:${prefix}/lib

## Building your program:

Here is an example Makefile (for GNU/Linux), to show you how to compile/link your program with libsqlora8:
```
# Makefile to build foo
#
# Change this to the prefix you used during configure of the library.
LIBSQLORA8_PREFIX=/usr

CC=gcc
CPPFLAGS=-I$(LIBSQLORA8_PREFIX)/include -I$(LIBSQLORA8_PREFIX)/lib/libsqlora8/include

# Remove -D_REENTRANT if you didn't configure libsqlora8 with threads
CFLAGS=-g -D_REENTRANT

# Remove -lpthread if you do didn't configure libsqlora8 with threads
LDFLAGS=-Wl,-R$(LIBSQLORA8_PREFIX)/lib -L$(LIBSQLORA8_PREFIX)/lib -lsqlora8 \
       -Wl,-R$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/lib -lclntsh\
       -lpthread

all: foo

foo.o : foo.c

foo : foo.o


clean:
	$(RM) foo.o foo
```
## Autoconf:

If you are using automake/autoconf in your development, you can use the supplied macros to easily integrate it in your project. This macro sets up also the paths to the oracle libraries.
The files aclibsqlora8.m4 and acoracle.m4, which are both included in the package and copied to ${prefix}/share/aclocal

### Syntax:

AM_PATH_LIBSQLORA8([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]])
Example: 
Add these lines to your configure.in:
```
dnl
dnl Check if the required libsqlora8 is installed
dnl
AM_PATH_LIBSQLORA8(2.3.0, , [
    LIBS="$LIBS $LIBSQLORA8_LIBS"
    CFLAGS="$CPPFLAGS $LIBSQLORA8_CFLAGS"
], [ echo "*** Please install libsqlora 2.3.0 or higher"; exit 1 ])
````
## pkg-config:

If you are using pkg-config, you can check it via this macro in your configure.in
dnl
dnl Check if the required libsqlora8 is installed
dnl
PKG_CHECK_MODULES(LIBSQLORA8, [ libsqlora8 >= 2.3.0 ], [
    LIBS="$LIBS $LIBSQLORA8_LIBS"
    CFLAGS="$CFLAGS $LIBSQLORA8_CFLAGS"
],[ echo "*** Please install libsqlora8 2.3.0 or higher"; exit 1 ])

## Environment Variables:

SQLORA_TRACE_LEVEL: Defines the trace level (0..4).
SQLORA_TRACE_FILE: The filename where the trace is written (default: sqlora8.trc).
SQLORA_ARRAYSIZE: The prefetch size for the cursors (default is 100 records).
ORACLE_SID

## License:

This software is absolutely free. I grant everybody the right to modify the source, as long as it is mentioned in the distribution.
I will not maintain modified versions. If you have enhancements or bugfixes, please send it to me. I will include it, as long as it doesn't break backward compatibility.


### Successfully installed on these platforms:

* SuSE GNU/Linux 9.2/gcc-3.3.4/Oracle 10.1.0.3

Note: If your platform does not appear here, doesn't mean its not supported! It should compile one every *nix flavour. If not, give me a detailed report and we fix it together.

## Windows

Handa Hirotake from Japan was so kind to provide the instructions for compiling the library (version 2.2.7) on Windows. Please visit this English and Japanese (EUC-JP encoding) README or English and Japanese (Shift-JIS encoding) README file. He supplied also the necessary libsqlora8.def file.

