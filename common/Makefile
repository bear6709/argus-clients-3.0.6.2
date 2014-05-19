#
#  Argus Software
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
#  Various configurable paths (remember to edit Makefile.in, not Makefile)
#
# $Id: //depot/argus-3.0.6/clients/common/Makefile.in#1 $
# $DateTime: 2012/04/17 12:22:02 $
# $Change: 2368 $
#
#

# Top level hierarchy

prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = ${prefix}/share

# Pathname of directory to install the include files
INCLDEST = ${prefix}/include
# Pathname of directory to install the library
LIBDEST =  ${exec_prefix}/lib
# Pathname of directory to install the man page
MANDEST = ${datarootdir}/man

# VPATH
srcdir = .


#
# You shouldn't need to edit anything below.
#

CC = gcc
CCOPT = -O3
INCLS = -I. -I../include  
DEFS = -DHAVE_CONFIG_H -DARGUS_SYSLOG

# Standard CFLAGS
CFLAGS = $(CCOPT) $(INCLS) $(DEFS)

INSTALL = /usr/bin/install -c
RANLIB = ranlib

#
# Flex and bison allow you to specify the prefixes of the global symbols
# used by the generated parser.  This allows programs to use lex/yacc
# and link against libpcap.  If you don't have flex or bison, get them.
#
LEX = flex -Pargus_
YACC = bison -y -p argus_

# Explicitly define compilation rule since SunOS 4's make doesn't like gcc.
# Also, gcc does not remove the .o before forking 'as', which can be a
# problem if you don't own the file but can write to the directory.
.c.o:
	@rm -f $@
	$(CC) $(CFLAGS) -c $(srcdir)/$*.c

# We would like to say "OBJ = $(SRC:.c=.o)" but Ultrix's make cannot
# hack the extra indirection


VSRC =  version.c
LSRC =  scanner.l
YSRC =  grammar.y
GENSRC = $(LSRC:.l=.c) $(YSRC:.y=.c)
GENHDR = tokdefs.h

TAGHDR = bpf/net/bpf.h

TAGFILES = $(SRC) $(HDR) $(TAGHDR)

LIBS = ../lib/argus_parse.a ../lib/argus_common.a ../lib/argus_client.a ../lib/argus_event.a

OBJ =	$(COMMONOBJ) $(PARSEOBJ) $(CLIENTOBJ) $(EVENTOBJ)
CLEANFILES = $(LIBS) $(OBJ) $(GENSRC) $(GENHDR) $(VSRC) lex.yy.c 


COMMONSRC = argus_code.c argus_filter.c argus_util.c argus_auth.c argus_parser.c $(GENSRC) $(VSRC)
COMMONOBJ = argus_code.o argus_filter.o argus_util.o argus_auth.o argus_parser.o scanner.o grammar.o version.o

PARSESRC  = argus_main.c
PARSEOBJ  = argus_main.o

CLIENTSRC = argus_client.c argus_import.c argus_label.c argus_grep.c
CLIENTOBJ = argus_client.o argus_import.o argus_label.o argus_grep.o

EVENTSRC  = argus_event.c
EVENTOBJ  = argus_event.o


SRC = $(COMMONSRC) $(PARSESRC) $(CLIENTSRC) $(EVENTSRC)

all: $(LIBS)

../lib/argus_common.a: $(COMMONOBJ)
	rm -f $@; ar qc $@ $(COMMONOBJ)
	-$(RANLIB) $@

../lib/argus_parse.a: $(PARSEOBJ)
	rm -f $@; ar qc $@ $(PARSEOBJ)
	-$(RANLIB) $@

../lib/argus_client.a: $(CLIENTOBJ)
	rm -f $@; ar qc $@ $(CLIENTOBJ)
	-$(RANLIB) $@

../lib/argus_event.a: $(EVENTOBJ)
	rm -f $@; ar qc $@ $(EVENTOBJ)
	-$(RANLIB) $@

scanner.c: scanner.l
	@rm -f $@
	$(LEX) -t $< > $$$$.$@; mv $$$$.$@ $@

scanner.o: scanner.c tokdefs.h
tokdefs.h: grammar.c
grammar.c: grammar.y
	@rm -f grammar.c tokdefs.h
	$(YACC) -d $<
	mv y.tab.c grammar.c
	mv y.tab.h tokdefs.h

grammar.o: grammar.c
	$(CC) $(CFLAGS) -Dyylval=argus_lval -c grammar.c


version.o: version.c
version.c: $(srcdir)/../VERSION
	@rm -f $@
	sed -e 's/.*/char version[] = "&";/' $(srcdir)/../VERSION > $@

install: force all
	[ -d $(DESTDIR)$(LIBDEST) ] || \
		(mkdir -p $(DESTDIR)$(LIBDEST); chmod 755 $(DESTDIR)$(LIBDEST))
	$(INSTALL) $(srcdir)/../lib/argus_client.a  $(DESTDIR)$(LIBDEST)/argus_client.a
	$(RANLIB) $(DESTDIR)$(LIBDEST)/argus_client.a
	$(INSTALL) $(srcdir)/../lib/argus_common.a $(DESTDIR)$(LIBDEST)/argus_common.a
	$(RANLIB) $(DESTDIR)$(LIBDEST)/argus_common.a
	$(INSTALL) $(srcdir)/../lib/argus_parse.a  $(DESTDIR)$(LIBDEST)/argus_parse.a
	$(RANLIB) $(DESTDIR)$(LIBDEST)/argus_parse.a
	[ -d $(DESTDIR)$(LIBDEST)/pkgconfig ] || \
		(mkdir -p $(DESTDIR)$(LIBDEST)/pkgconfig; chmod 755 $(DESTDIR)$(LIBDEST)/pkgconfig)
	$(INSTALL) $(srcdir)/../lib/argus-clients.pc  $(DESTDIR)$(LIBDEST)/pkgconfig/argus-clients.pc

clean:
	rm -f $(CLEANFILES)

distclean:
	rm -f $(CLEANFILES) Makefile $(srcdir)/../lib/argus-clients.pc config.cache config.log config.status \
	    gnuc.h os-proto.h bpf_filter.c net

tags: $(TAGFILES)
	ctags -wtd $(TAGFILES)

tar:	force
	@cwd=`pwd` ; dir=`basename $$cwd` ; name=libpcap-`cat VERSION` ; \
	    list="" ; tar="tar chFFf" ; \
	    for i in `cat FILES` ; do list="$$list $$name/$$i" ; done; \
	    echo \
	    "rm -f ../$$name; ln -s $$dir ../$$name" ; \
	     rm -f ../$$name; ln -s $$dir ../$$name ; \
	    echo \
	    "(cd .. ; $$tar - [lots of files]) | compress > /tmp/$$name.tar.Z" ; \
	     (cd .. ; $$tar - $$list) | compress > /tmp/$$name.tar.Z ; \
	    echo \
	    "rm -f ../$$name" ; \
	     rm -f ../$$name

force:	/tmp
depend:	$(SRC) force
	../bin/mkdep -c $(CC) $(DEFS) $(INCLS) $(SRC)
