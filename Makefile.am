## $Id: Makefile.am 302 2005-01-19 06:36:06Z kpoitschke $
##
## Copyright (c) 1991-2004 Kai Poitschke (kai@poitschke.de)
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

AUTOMAKE_OPTIONS       = foreign dist-zip

SUBDIRS =  . examples doc windows contrib

ACLOCAL_AMFLAGS  = -I . -I $(prefix)/share/aclocal

EXTRA_DIST              = ChangeLog           \
                          NEWS                \
                          NEWS-2.2            \
                          Install             \
                          libsqlora8.spec.in  \
                          makerpm.sh          \
                          Makefile.start      \
                          aclibsqlora8.m4     \
                          acoracle.m4         \
                          libsqlora8.pc.in    \
                          libsqlora8-uninstalled.pc.in



configincludedir        = $(pkglibdir)/include

lib_LTLIBRARIES         = libsqlora8.la

libsqlora8_la_SOURCES   = sqlora.c sqlora_compat.c

noinst_PROGRAMS         = testlora  #testblob
#AM_CFLAGS               = -pedantic
testlora_SOURCES        = testlora.c 
#testblob_SOURCES        = testblob.c 
testlora_LDADD          = sqlora.o @GLIB_LIBS@
#testblob_LDADD          = sqlora.o @GLIB_LIBS@

include_HEADERS         = sqlora.h

configinclude_DATA      = @PACKAGE@-config.h

bin_SCRIPTS             = @PACKAGE@-config

m4datadir               = $(datadir)/aclocal
m4data_DATA             = aclibsqlora8.m4 acoracle.m4

docdir                  = $(prefix)/share/doc/packages/$(PACKAGE)
doc_DATA                = ChangeLog NEWS NEWS-2.2

pkgconfigdir            = $(prefix)/lib/pkgconfig
pkgconfig_DATA          = libsqlora8.pc

libsqlora8_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-release $(LT_RELEASE) \
	-export-dynamic

#testlora_LDFLAGS = -lefence

CLEANFILES = *~ libsqlora8.html


libsqlora8-config: libsqlora8-config.in

libsqlora8-config.h: libsqlora8-config.h.in

libsqlora8.spec: libsqlora8.spec.in

rpm: dist libsqlora8.spec
	$(top_srcdir)/makerpm.sh ${RPM} ${PACKAGE} ${VERSION} ${AWK} ${prefix}

LINT=splint
LINTFLAGS = -weak +posixlib

lint:
	$(LINT) -I. -DPROTOTYPES $(CPPFLAGS) $(LINTFLAGS) sqlora.c | tee lint.out


# remove acsqlora8.m4 of old installations
install-data-hook:
	if test -r $(m4datadir)/acsqlora8.m4 ; then mv -f $(m4datadir)/acsqlora8.m4 $(m4datadir)/acsqlora8.m4.old ; fi

# $Id: Makefile.am 302 2005-01-19 06:36:06Z kpoitschke $
