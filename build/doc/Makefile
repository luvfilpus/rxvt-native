# doc/Makefile.in	-*- Makefile -*-
# $Id: Makefile.in,v 1.20 2000/05/03 06:40:54 gcw Exp $
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

srcdir =	/usr/src/rxvt-20050409-21/src/rxvt/doc
VPATH =		/usr/src/rxvt-20050409-21/src/rxvt/doc
.PATH:		/usr/src/rxvt-20050409-21/src/rxvt/doc

basedir = ..
thisdir = doc

first_rule: all
dummy:

#
# Distribution variables
#

YODL_RXVT = yodl/rxvt-colours.yo yodl/rxvt-environment.yo\
	    yodl/rxvt-files.yo yodl/rxvt-fonts.yo yodl/rxvt-login.yo\
	    yodl/rxvt-mousereporting.yo yodl/rxvt-options.yo\
	    yodl/rxvt-resources.yo yodl/rxvt-scrollbar.yo\
	    yodl/rxvt-textselection.yo
YODL_RREF = yodl/rxvtRef-csi.yo yodl/rxvtRef-definitions.yo\
	    yodl/rxvtRef-graphics.yo yodl/rxvtRef-keycodes.yo\
	    yodl/rxvtRef-menubar.yo yodl/rxvtRef-mouse.yo\
	    yodl/rxvtRef-privatemodes.yo yodl/rxvtRef-sequences.yo\
	    yodl/rxvtRef-values.yo yodl/rxvtRef-xpm.yo yodl/rxvtRef-xterm.yo

DIST_ETC  = etc/rxvt.termcap etc/rxvt.terminfo etc/XTerm.ad
DIST_YODL = yodl/masonyodl.yo yodl/versioninfo.yo yodl/versioninfo.yo.in\
	    $(YODL_RXVT) yodl/rxvt.yo $(YODL_RREF) yodl/rxvtRef.yo
DIST_MENU = menu/menu menu/example.menu menu/rxvt.menu menu/rxvt.zh-menu\
	    menu/terminal.menu menu/jedmenu.sl
DIST	  = Makefile.in README.greek README.menu README.xvt BUGS FAQ TODO\
	    changes.txt xterm.seq LSM.in rxvt.1 rxvt.tbl rxvtRef.txt\
	    rxvtRef.html rxvtRef-frame.html rxvtRef-toc.html rxvt.html 

#-------------------------------------------------------------------------

all:	rxvt.1

rxvt.1: rxvt.tbl Makefile
	@if test x$(TBL) = x; then : ; else echo "$(TBL) $(srcdir)/rxvt.tbl | grep -v '^.lf' > rxvt.1"; $(TBL) $(srcdir)/rxvt.tbl | grep -v '^.lf' > rxvt.1 ; fi

SEDREPLACE = -e 's%@RXVT_VERSION@%$(VERSION)%g;'\
	     -e 's%@RXVT_LSMDATE@%$(LSMDATE)%g;'\
	     -e 's%@RXVT_DATE@%$(DATE)%g;'\
	     -e 's%@RXVT_MAINTEMAIL@%$(MAINTEMAIL)%g;'\
	     -e 's%@RXVT_MAINT@%$(MAINT)%g;'\
	     -e 's%@RXVT_WEBPAGE@%$(WEBPAGE)%g;'\
	     -e 's%@RXVT_WEBMAINTEMAIL@%$(WEBMAINTEMAIL)%g;'\
	     -e 's%@RXVT_WEBMAINT@%$(WEBMAINT)%g;'\
	     -e 's%@RXVT_FTPSITENAME@%$(FTPSITENAME)%g;'\
	     -e 's%@RXVT_FTPSITEDIR@%$(FTPSITEDIR)%g;'

tags allbin:

alldoc:	$(basedir)/$(VERNAME).lsm yodl/versioninfo.yo rxvt.1 rxvt.html rxvtRef.html rxvtRef.txt

yodl/versioninfo.yo: yodl/versioninfo.yo.in ../src/version.h
	$(SED) $(SEDREPLACE) < $(srcdir)/yodl/versioninfo.yo.in > $@

$(basedir)/$(VERNAME).lsm: LSM.in $(srcdir)/../autoconf/Make.common.in $(srcdir)/../src/version.h
	$(SED) $(SEDREPLACE) < $(srcdir)/LSM.in > $@

rxvt.tbl: yodl/rxvt.yo $(YODL_RXVT) yodl/masonyodl.yo yodl/versioninfo.yo
	(cd $(srcdir)/yodl; yodl2man -I. -I.. -o../rxvt.tbl rxvt.yo)

rxvt.html: yodl/rxvt.yo $(YODL_RXVT) yodl/masonyodl.yo
	(cd $(srcdir)/yodl; yodl2html -I. -I.. -o../rxvt.html rxvt.yo)

rxvtRef.html: $(YODL_RREF) yodl/rxvtRef.yo yodl/masonyodl.yo
	(cd $(srcdir)/yodl; yodl2html -I. -I.. -o../rxvtRef.html rxvtRef.yo)

rxvtRef.txt: $(YODL_RREF) yodl/rxvtRef.yo yodl/masonyodl.yo
	(cd $(srcdir)/yodl; yodl2txt -I. -I.. -o../rxvtRef.txt rxvtRef.yo)

clean:
	$(RMF) rxvt.man *~ yodl/*~

realclean: clean
	$(RMF) tags rxvt.1 rxvt.html rxvtRef.html rxvtRef.txt

cleandir: realclean

distclean:

install:
	$(INSTALL_DATA) rxvt.1 $(DESTDIR)$(mandir)/$(RXVTNAME).$(manext)

uninstall:
	-(cd $(mandir); $(RMF) $(RXVTNAME).$(manext) )

distdirs:
	mkdir -p $(basedir)/../$(VERNAME)/$(thisdir)
	mkdir -p $(basedir)/../$(VERNAME)/$(thisdir)/etc
	mkdir -p $(basedir)/../$(VERNAME)/$(thisdir)/menu
	mkdir -p $(basedir)/../$(VERNAME)/$(thisdir)/yodl

distcopy: realclean distdepend
	$(CP) -p $(basedir)/$(VERNAME).lsm $(basedir)/../$(VERNAME);
	$(CP) -p $(DIST)      $(basedir)/../$(VERNAME)/$(thisdir)
	$(CP) -p $(DIST_ETC)  $(basedir)/../$(VERNAME)/$(thisdir)/etc
	$(CP) -p $(DIST_MENU) $(basedir)/../$(VERNAME)/$(thisdir)/menu
	$(CP) -p $(DIST_YODL) $(basedir)/../$(VERNAME)/$(thisdir)/yodl

distdepend: alldoc

# ------------------------------------------------------------------------
# DO NOT DELETE: ugly dependency list follows
