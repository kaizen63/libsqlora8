## $Id: Makefile.am 295 2004-08-26 09:55:58Z kpoitschke $
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
SUBDIRS = html

EXTRA_DIST = examples.doc

all: doc

DOXYDOC_OPTS=

doc: $(top_srcdir)/doc/Doxyfile $(top_srcdir)/sqlora.h
	@echo "/** @page index libsqlora8 - $(VERSION)" >main_page.doc
	@echo "<H2>ChangeLog</H2>" >> main_page.doc
	@echo "<pre>" >> main_page.doc
	@cat $(top_srcdir)/ChangeLog | sed -e 's/</\&lt;/g' -e 's/>/\&gt;/g' >> main_page.doc
	@echo "</pre>" >> main_page.doc
	@echo "*/" >> main_page.doc 
	@test x$(DOXYGEN) = xno || $(DOXYGEN) $(DOXYDOC_OPTS)

CLEANFILES=main_page.doc *~

dist-hook:
	$(MAKE) clean doc

