## $Id: Makefile.am 287 2004-07-30 16:28:21Z kpoitschke $
##
## Copyright (c) 1991-2002 Kai Poitschke (kai@poitschke.de)
##  
## 
## This file is part of the libsqlora8 package which can be found
## at http://www.poitschke.de/libsqlora8/
##
##
##  Permission to use, copy, modify, and distribute this software for
##  any purpose with or without fee is hereby granted, provided that
##  the above copyright notice and this permission notice appear in all
##  copies.
##
##  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
##  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
##  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
##  IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
##  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
##  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
##  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
##  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
##  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
##  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
##  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
##  SUCH DAMAGE.
##

AUTOMAKE_OPTIONS       = foreign 
SUBDIRS = .

examples_DATA = ex1.c ex2.c ex3.c ex4.c ex5.c ex6.c ex7.c ex8.c ex9.c ex10.c\
               ex11.c ex12.c ex13.c ex13b.c ex14.c ex15.c ex16.c ex17.c ex18.c ex19.c\
               ex20.c error_exit.c examples.c Makefile

noinst_PROGRAMS = examples
noinst_HEADERS = examples.h

AM_CFLAGS = -I$(top_srcdir) -I$(top_srcdir)/examples @GLIB_CFLAGS@ -O -g

examples_SOURCES = $(examples_DATA)
examples_LDADD = ../sqlora.o @GLIB_LIBS@
CLEANFILES = *~

examplesdir = $(prefix)/share/doc/packages/$(PACKAGE)/examples

EXTRA_DIST = $(examples_DATA)
