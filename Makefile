#
#  Argus Client Software. Tools to read, analyze and manage Argus data.
#  Copyright (c) 2000-2012 QoSient, LLC
#  All rights reserved.
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2, or (at your option)
#  any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */
# 
# 

NOOP = $(SHELL) -c true
NOECHO = @

CC = gcc

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

DEFS = -DHAVE_CONFIG_H
LIBS = 
WRAPLIBS = 

CFLAGS = -g
LDFLAGS = -g

prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = ${prefix}/share

srcdir = .
incdir = /usr/local/include
docdir = ${datarootdir}/doc/argus-clients-3.0

#### End of system configuration section. ####

SHELL = /bin/sh

DIRS = ./common ./include ./clients  ./examples
INSTDIRS = ./clients ./examples ./common ./include

DISTFILES = AUTHORS CHANGES COPYING CREDITS ChangeLog INSTALL MANIFEST Makefile.in README VERSION \
	include lib man support aclocal.m4 acsite.m4 config configure configure.ac .threads \
	bin clients examples common 

.c.o:
	$(CC) -c $(CPPFLAGS) $(DEFS) $(CFLAGS) $<

.PHONY: install installdirs

all: force
	@for i in  $(DIRS) ; do \
		if [ -d $$i ] ; then \
		echo "making in $$i"; \
		(cd $$i > /dev/null; $(MAKE));\
		fi; \
	done

.PHONY: all

clients: common
examples: common
include: common

install:  force
	$(MAKE) installdirs
	[ -d $(DESTDIR)/usr/local ] || \
		(mkdir -p $(DESTDIR)/usr/local; chmod 755 $(DESTDIR)/usr/local)
	[ -d $(DESTDIR)${exec_prefix}/bin ] || \
		(mkdir -p $(DESTDIR)${exec_prefix}/bin; chmod 755 $(DESTDIR)${exec_prefix}/bin)
	[ -d $(DESTDIR)$(exec_prefix)/argus ] || \
		(mkdir -p $(DESTDIR)$(exec_prefix)/argus; chmod 755 $(DESTDIR)$(exec_prefix)/argus)
	[ -d $(DESTDIR)$(exec_prefix)/argus/archive ] || \
		(mkdir -p $(DESTDIR)$(exec_prefix)/argus/archive; chmod 755 $(DESTDIR)$(exec_prefix)/argus/archive)
	@for i in  $(INSTDIRS) ; do \
		if [ -d $$i ] ; then \
		echo "making in $$i"; \
		(cd $$i > /dev/null; $(MAKE) install); \
		fi; \
	done
	$(INSTALL) -m 0644 $(srcdir)/support/Config/rarc $(DESTDIR)$(prefix)/argus/rarc
	$(INSTALL) -m 0644 $(srcdir)/support/Config/delegated-ipv4-latest $(DESTDIR)$(prefix)/argus/delegated-ipv4-latest
	$(INSTALL) -m 0644 $(srcdir)/support/Config/wireshark.manuf.txt $(DESTDIR)$(prefix)/argus/wireshark.manuf.txt
	$(INSTALL) -m 0755 $(srcdir)/bin/argusclientbug $(DESTDIR)${exec_prefix}/bin/argusclientbug
	[ -d $(DESTDIR)${datarootdir}/man ] || \
		(mkdir -p $(DESTDIR)${datarootdir}/man; chmod 755 $(DESTDIR)${datarootdir}/man)
	[ -d $(DESTDIR)${datarootdir}/man/man1 ] || \
		(mkdir -p $(DESTDIR)${datarootdir}/man/man1; chmod 755 $(DESTDIR)${datarootdir}/man/man1)
	$(INSTALL) -m 0644 $(srcdir)/man/man1/ra.1 \
		$(DESTDIR)${datarootdir}/man/man1/ra.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/rabins.1 \
		$(DESTDIR)${datarootdir}/man/man1/rabins.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/racluster.1 \
		$(DESTDIR)${datarootdir}/man/man1/racluster.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/raconvert.1 \
		$(DESTDIR)${datarootdir}/man/man1/raconvert.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/racount.1 \
		$(DESTDIR)${datarootdir}/man/man1/racount.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/radump.1 \
		$(DESTDIR)${datarootdir}/man/man1/radump.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/raevent.1 \
		$(DESTDIR)${datarootdir}/man/man1/raevent.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/rafilteraddr.1 \
		$(DESTDIR)${datarootdir}/man/man1/rafilteraddr.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/ragraph.1 \
		$(DESTDIR)${datarootdir}/man/man1/ragraph.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/ragrep.1 \
		$(DESTDIR)${datarootdir}/man/man1/ragrep.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/rahisto.1 \
		$(DESTDIR)${datarootdir}/man/man1/rahisto.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/ralabel.1 \
		$(DESTDIR)${datarootdir}/man/man1/ralabel.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/ranonymize.1 \
		$(DESTDIR)${datarootdir}/man/man1/ranonymize.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/rapath.1 \
		$(DESTDIR)${datarootdir}/man/man1/rapath.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/rapolicy.1 \
		$(DESTDIR)${datarootdir}/man/man1/rapolicy.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/rasort.1 \
		$(DESTDIR)${datarootdir}/man/man1/rasort.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/rasplit.1 \
		$(DESTDIR)${datarootdir}/man/man1/rasplit.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/rasql.1 \
		$(DESTDIR)${datarootdir}/man/man1/rasql.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/rasqlinsert.1 \
		$(DESTDIR)${datarootdir}/man/man1/rasqlinsert.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/rasqltimeindex.1 \
		$(DESTDIR)${datarootdir}/man/man1/rasqltimeindex.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/rastream.1 \
		$(DESTDIR)${datarootdir}/man/man1/rastream.1
	$(INSTALL) -m 0644 $(srcdir)/man/man1/rastrip.1 \
		$(DESTDIR)${datarootdir}/man/man1/rastrip.1
	[ -d $(DESTDIR)${datarootdir}/man/man5 ] || \
		(mkdir -p $(DESTDIR)${datarootdir}/man/man5; chmod 755 $(DESTDIR)${datarootdir}/man/man5)
	[ -d $(DESTDIR)${datarootdir}/man/man8 ] || \
		(mkdir -p $(DESTDIR)${datarootdir}/man/man8; chmod 755 $(DESTDIR)${datarootdir}/man/man8)
	$(INSTALL) -m 0644 $(srcdir)/man/man5/rarc.5 \
		$(DESTDIR)${datarootdir}/man/man5/rarc.5
	$(INSTALL) -m 0644 $(srcdir)/man/man5/racluster.5 \
		$(DESTDIR)${datarootdir}/man/man5/racluster.5
	$(INSTALL) -m 0644 $(srcdir)/man/man5/ralabel.conf.5 \
		$(DESTDIR)${datarootdir}/man/man5/ralabel.conf.5
	$(INSTALL) -m 0644 $(srcdir)/man/man5/radium.conf.5 \
		$(DESTDIR)${datarootdir}/man/man5/radium.conf.5
	$(INSTALL) -m 0644 $(srcdir)/man/man5/ranonymize.5 \
		$(DESTDIR)${datarootdir}/man/man5/ranonymize.5
	$(INSTALL) -m 0644 $(srcdir)/man/man8/radium.8 \
		$(DESTDIR)${datarootdir}/man/man8/radium.8

	[ -d $(DESTDIR)/usr/local ] || \
		(mkdir -p $(DESTDIR)/usr/local; chmod 755 $(DESTDIR)/usr/local)
	[ -d $(DESTDIR)$(docdir) ] || \
		(mkdir -p $(DESTDIR)$(docdir); chmod 755 $(DESTDIR)$(docdir))
	$(INSTALL) -m 0644 $(srcdir)/README $(DESTDIR)$(docdir)
	$(INSTALL) -m 0644 $(srcdir)/COPYING $(DESTDIR)$(docdir)

uninstall:
	rm -f $(DESTDIR)${exec_prefix}/bin/ra
	rm -f $(DESTDIR)${exec_prefix}/bin/rabins
	rm -f $(DESTDIR)${exec_prefix}/bin/racluster
	rm -f $(DESTDIR)${exec_prefix}/bin/racount
	rm -f $(DESTDIR)${exec_prefix}/bin/radump
	rm -f $(DESTDIR)${exec_prefix}/bin/raevent
	rm -f $(DESTDIR)${exec_prefix}/bin/ragraph
	rm -f $(DESTDIR)${exec_prefix}/bin/ragrep
	rm -f $(DESTDIR)${exec_prefix}/bin/rahisto
	rm -f $(DESTDIR)${exec_prefix}/bin/rasort
	rm -f $(DESTDIR)${exec_prefix}/bin/rasplit
	rm -f $(DESTDIR)${exec_prefix}/bin/rastream
	rm -f $(DESTDIR)${exec_prefix}/bin/rastrip
	rm -f $(DESTDIR)${exec_prefix}/bin/ratop
	rm -f $(DESTDIR)${exec_prefix}/bin/radium
	rm -f $(DESTDIR)${datarootdir}/man/man1/ra.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/rabins.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/racluster.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/racount.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/raevent.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/ragraph.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/ragrep.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/rahisto.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/rapath.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/rasort.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/rasplit.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/rastream.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/rastrip.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/rasql.1
	rm -f $(DESTDIR)${datarootdir}/man/man1/rasqlinsert.1
	rm -f $(DESTDIR)${datarootdir}/man/man5/rarc.5
	rm -f $(DESTDIR)${datarootdir}/man/man5/racluster.5
	rm -f $(DESTDIR)${datarootdir}/man/man5/ralabel.conf.5
	rm -f $(DESTDIR)${datarootdir}/man/man5/radium.conf.5
	rm -f $(DESTDIR)${datarootdir}/man/man8/radium.8
	rm -rf $(DESTDIR)$(docdir)
	rm -f $(DESTDIR)$(prefix)/argus/COPYING
	rm -f $(DESTDIR)$(prefix)/argus/README
	rm -f $(DESTDIR)$(prefix)/argus/rarc

installdirs:
	${srcdir}/config/mkinstalldirs $(bindir) $(infodir)

Makefile: Makefile.in config.status
	$(SHELL) config.status

config.status: configure
	$(srcdir)/configure --no-create

TAGS: $(SRCS)
	etags $(SRCS)

.PHONY: clean mostlyclean distclean realclean dist

clean: force
	rm -f config.log
	@for i in $(DIRS) ; do \
		if [ -d $$i ] ; then \
		(cd $$i > /dev/null; $(MAKE) clean ); \
		fi; \
	done

mostlyclean: clean

distclean: clean
	rm -f config.*
	rm -f TAGS
	rm -f lib/*.a
	rm -f bin/ra*
	rm -f Makefile
	@for i in $(DIRS) ; do \
		if [ -d $$i ] ; then \
		(cd $$i > /dev/null; $(MAKE) distclean); \
		fi; \
	done

clobber realclean: force
	rm -f TAGS
	rm -f lib/*.a
	rm -f include/argus_config.h
	rm -f include/tm.h
	rm -f bin/ra*
	rm -rf log
	@for i in $(DIRS) ; do \
		if [ -d $$i ] ; then \
		(cd $$i > /dev/null; $(MAKE) distclean); \
		fi; \
	done
	rm -f ./Makefile config.*

dist: $(DISTFILES) distclean
	echo argus-clients-`cat VERSION` > .fname
	rm -rf `cat .fname`
	mkdir `cat .fname`
	tar cf - $(DISTFILES) | (cd `cat .fname`; tar xpf -)
	ls -lR `cat .fname` | fgrep CVS: | sed 's/:$///' > exfile
	tar -X exfile -chozf `cat .fname`.tar.gz `cat .fname`
	rm -rf `cat .fname` .fname exfile

force:  /tmp
depend: $(GENSRC) force
	@for i in $(DIRS) ; do \
		if [ -d $$i ] ; then \
		(cd $$i > /dev/null; $(MAKE) depend ); \
		fi; \
	done

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:
