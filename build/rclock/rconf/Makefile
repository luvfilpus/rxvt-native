# rclock/rconf/Makefile.in	-*- Makefile -*-
# autoconf/Make.common.in	 -*- Makefile -*-
# release date (man), LSM date, version number/name, current maintainer
DATE=26 MARCH 2003
LSMDATE=26MAR03
VERSION=2.7.10
VERNAME=rxvt-$(VERSION)#
MAINT=Geoff Wing#
MAINTEMAIL=<gcw@rxvt.org>#
WEBMAINT=Oezguer Kesim#
WEBMAINTEMAIL=<oec@rxvt.org>#
WEBPAGE=<http://www.rxvt.org/>#
FTPSITENAME=ftp.rxvt.org#
FTPSITEDIR=/pub/rxvt#
#-------------------------------------------------------------------------
RXVTNAME=rxvt

SHELL = /bin/sh

# This variable makes it possible to move the installation root to another
# directory. This is useful when you're creating a binary distribution
# If empty, normal root will be used.
# You can run eg. 'make install DESTDIR=/packages/rxvt-xx' to accomplish
# that.
# DESTDIR = /usr/local/X11/$(VERNAME)

# Installation target directories & other installation stuff
prefix = /usr
exec_prefix = /usr
bindir = /usr/bin
libdir = /usr/lib
includedir = ${prefix}/include
mandir = /usr/share/man/man1
manext = 1

# Tools & program stuff
CC = gcc
CPP = gcc -E
MV = /usr/bin/mv
RM = /usr/bin/rm
RMF = /usr/bin/rm -f
CP = /usr/bin/cp
LN = /usr/bin/ln
SED = /usr/bin/sed
AWK = gawk
ECHO = /usr/bin/echo
CMP = /usr/bin/cmp
TBL = /usr/bin/tbl
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = /usr/bin/install -c -m 755
INSTALL_DATA = /usr/bin/install -c -m 644

# Flags & libs
# add -DBINDIR=\""$(bindir)/"\" to CPPFLAGS, if we need to spawn a program

CFLAGS = -ggdb -O2 -pipe -Wimplicit-function-declaration -fdebug-prefix-map=/usr/src/rxvt-20050409-21/build=/usr/src/debug/rxvt-20050409-21 -fdebug-prefix-map=/usr/src/rxvt-20050409-21/src/rxvt=/usr/src/debug/rxvt-20050409-21 
CPPFLAGS =  -DHAVE_LIBXPM
LDFLAGS = 
DEFS = -DHAVE_CONFIG_H
LIBS = 
DINCLUDE = 
DLIB = /usr/src/rxvt-20050409-21/build/W11/wrap/rxvt_res.o

# X Include directory
XINC = -I/usr/src/rxvt-20050409-21/build/W11/X11  -I/usr/src/rxvt-20050409-21/build/W11

# extra libraries needed by X on some systems, X library location
# punt. build system is broken.
XLIB = /usr/src/rxvt-20050409-21/build/W11/lib/libXpm.a /usr/src/rxvt-20050409-21/build/W11/lib/libX11.a

LIBTOOL = $(SHELL) $(top_builddir)/libtool
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(DEBUG) $(DINCLUDE) $(XINC) -I$(basedir) -I$(srcdir) -I.
LINK = $(CC) $(CFLAGS) $(LDFLAGS)

# End of common section of the Makefile
#-------------------------------------------------------------------------

srcdir =	/usr/src/rxvt-20050409-21/src/rxvt/rclock/rconf
VPATH =		/usr/src/rxvt-20050409-21/src/rxvt/rclock/rconf
.PATH:		/usr/src/rxvt-20050409-21/src/rxvt/rclock/rconf

basedir = ../..
thisdir = rclock/rconf

first_rule: rconf
dummy:

subdirs = 

SRCS =	dialogs.c edit.c list.c rconf.c
OBJS =	dialogs.o edit.o list.o rconf.o
HDRS =	RConf_ad.h dialogs.h edit.h list.h rconf.h

#
# Distribution variables
#

DIST = $(HDRS) $(SRCS) Makefile.in RConf.ad rconf.gif rconf.html

# inference rules
.c.o:
	$(CC) -c $(CPPFLAGS) $(XINC) -I. -I$(basedir) -I$(srcdir) $(DEFS) $(CFLAGS) $<
#-------------------------------------------------------------------------
all:

rconf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(XLIB) -lXaw

tags: $(SRCS)
	ctags $(SRCS)

allbin: rconf

alldoc:	

clean:
	$(RMF) rconf core a.out *.o *.bak *~

realclean: clean
	$(RMF) tags

cleandir: realclean

distclean:
	(cd $(srcdir); $(RMF) rconf *~ *.o core a.out)
	if test $(srcdir) = .; then $(MAKE) realclean; fi
	@for I in $(subdirs); do (cd $$I; $(MAKE) $@) || exit 1; done
	(cd $(srcdir); $(RMF) Makefile)

install: allbin alldoc
	$(INSTALL_PROGRAM) rconf $(DESTDIR)$(bindir)/$(binprefix)rconf

uninstall:
	cd $(bindir); $(RMF) $(binprefix)rconf

distdirs:
	mkdir -p $(basedir)/../$(VERNAME)/$(thisdir)
	@for I in $(subdirs); do (cd $$I; $(MAKE) $@ || (echo "Failed to make distclean in $$I"; exit 0) ); done

distcopy: 
	$(CP) -p $(DIST) $(basedir)/../$(VERNAME)/$(thisdir)
	@for I in $(subdirs); do (cd $$I; $(MAKE) $@) || exit 1; done

# -----------------------------------------------------------------------
# DO NOT DELETE: ugly dependency list follows
list.o : list.h
dialogs.o : dialogs.h
edit.o : edit.h
rconf.o : list.h dialogs.h edit.h rconf.h RConf_ad.h
