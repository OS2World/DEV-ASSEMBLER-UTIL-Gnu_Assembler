# This file was generated automatically by configure.  Do not edit.
.SUFFIXES: .obj
CC= gcc
VPATH = .
links = host.h targ-cpu.c targ-cpu.h targ-env.h obj-format.h obj-format.c atof-targ.c
host_alias = i386-*-bsd
host_cpu = i386
host_vendor = *
host_os = bsd
host_canonical = i386-*-bsd
target_alias = i386-*-bsd
target_cpu = i386
target_vendor = *
target_os = bsd
target_canonical = i386-*-bsd
# Makefile for GNU Assembler
#   Copyright (C) 1987-1992 Free Software Foundation, Inc.

#This file is part of GNU GAS.

#GNU GAS is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2, or (at your option)
#any later version.

#GNU GAS is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with GNU GAS; see the file COPYING.  If not, write to
#the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

# The targets for external use include:
# all, doc, proto, install, uninstall, includes, TAGS,
# clean, cleanconfig, realclean, stage1, stage2, stage3, stage4.

# Variables that exist for you to override.
# See below for how to change them for certain systems.

srcdir = .

prefix = /usr/local

program_transform_name =
exec_prefix = $(prefix)
bindir = d:\gcc2\bin
libdir = $(exec_prefix)/lib
tooldir = $(exec_prefix)/i386-*-bsd

datadir = $(prefix)/lib
mandir = $(prefix)/man
man1dir = $(mandir)/man1
man2dir = $(mandir)/man2
man3dir = $(mandir)/man3
man4dir = $(mandir)/man4
man5dir = $(mandir)/man5
man6dir = $(mandir)/man6
man7dir = $(mandir)/man7
man8dir = $(mandir)/man8
man9dir = $(mandir)/man9
infodir = $(prefix)/info
includedir = $(prefix)/include
docdir = $(datadir)/doc

VERSION=2.2


AR = ar
AR_FLAGS = qv
BISON = bison -y
MAKEINFO = makeinfo
TEXI2DVI = texi2dvi
RANLIB = ranlib
CFLAGS = -O2 -m486
LDFLAGS = -static

MAKEOVERRIDES=

AS_FOR_TARGET = $${here}/as.new

CC_FOR_TARGET = gcc

NM_FOR_TARGET =

OBJDUMP= objdump
OBJDUMP_FOR_TARGET =

FLAGS_TO_PASS =

CHECKFLAGS=

# Lists of files for various purposes.
OS2SRC = o2obj.c

REAL_SOURCES = \
	$(srcdir)/app.c \
	$(srcdir)/as.c \
	$(srcdir)/atof-generic.c \
	$(srcdir)/bignum-copy.c \
	$(srcdir)/cond.c \
	$(srcdir)/expr.c \
	$(srcdir)/flonum-konst.c \
	$(srcdir)/flonum-copy.c \
	$(srcdir)/flonum-mult.c \
	$(srcdir)/frags.c \
	$(srcdir)/hash.c \
	$(srcdir)/hex-value.c \
	$(srcdir)/input-file.c \
	$(srcdir)/input-scrub.c \
	$(srcdir)/messages.c \
	$(srcdir)/output-file.c \
	$(srcdir)/read.c \
	$(srcdir)/subsegs.c \
	$(srcdir)/symbols.c \
	$(srcdir)/write.c \
	$(srcdir)/listing.c \
	$(srcdir)/xmalloc.c \
	$(srcdir)/obstack.c

# in an expedient order
LINKED_SOURCES = \
	targ-cpu.c \
	obj-format.c \
	atof-targ.c

SOURCES = $(LINKED_SOURCES) $(REAL_SOURCES)

REAL_HEADERS = \
	$(srcdir)/as.h \
	$(srcdir)/bignum.h \
	$(srcdir)/expr.h \
	$(srcdir)/flonum.h \
	$(srcdir)/frags.h \
	$(srcdir)/hash.h \
	$(srcdir)/input-file.h \
	$(srcdir)/listing.h \
	$(srcdir)/tc.h \
	$(srcdir)/obj.h \
	$(srcdir)/read.h \
	$(srcdir)/struc-symbol.h \
	$(srcdir)/subsegs.h \
	$(srcdir)/symbols.h \
	$(srcdir)/write.h

LINKED_HEADERS = \
	a.out.gnu.h \
	a.out.h \
	host.h \
	targ-env.h \
	targ-cpu.h \
	obj-format.h \
	atof-targ.h

HEADERS = $(LINKED_HEADERS) $(REAL_HEADERS)

OS2OBJS = fo2obj.obj


OBJS = \
	targ-cpu.obj \
	obj-format.obj \
	atof-targ.obj \
	app.obj \
	as.obj \
	atof-generic.obj \
	bignum-copy.obj \
	cond.obj \
	expr.obj \
	flonum-konst.obj \
	flonum-copy.obj \
	flonum-mult.obj \
	frags.obj \
	hash.obj \
	hex-value.obj \
	input-file.obj \
	input-scrub.obj \
	messages.obj \
	output-file.obj \
	read.obj \
	subsegs.obj \
	symbols.obj \
	write.obj \
	listing.obj \
	xmalloc.obj \
	obstack.obj

#### host, target, and site specific Makefile frags come in here.

all: gas386.exe o2obj.exe


# This is the variable actually used when we compile.
ALL_CFLAGS = $(INTERNAL_CFLAGS) $(CROSS) $(CFLAGS) $(HDEFINES) $(TDEFINES) \
	$(BFDDEF)

# How to link with both our special library facilities
# and the system's installed libraries.

LIBS =

# Specify the directories to be searched for header files.
# Both . and srcdir are used, in that order,
# so that tm.h and config.h will be found in the compilation
# subdirectory rather than in the source directory.
INCLUDES = -I. -I$(srcdir)/Include -I$(srcdir)/config
#SUBDIR_INCLUDES = -I.. -I$(srcdir) -I$(srcdir)/config

# Always use -I$(srcdir)/config when compiling.
.c.obj:
	$(CC) -c $(ALL_CFLAGS) $(CPPFLAGS) $(INCLUDES) $<

# This tells GNU make version 3 not to export all the variables
# defined in this file into the environment.
.NOEXPORT:

# Files to be copied away after each stage in building.
STAGESTUFF = *.obj as.new

install:
	copy gas386.exe $(bindir)\as.exe
	copy o2obj.exe $(bindir)\o2obj.exe

gas386.exe: $(OBJS) $(LIBS) $(OS2OBJS)
	$(CC) $(ALL_CFLAGS) $(LDFLAGS) -o gas386.exe \
	$(OBJS) $(OS2OBJS) $(LIBS) $(LOADLIBES)

o2obj.exe: o2obj.obj
	$(CC) $(CFLAGS) $(LDFLAGS) -o o2obj.exe o2obj.obj

fo2obj.obj: o2obj.c
	$(CC) $(CFLAGS) -DFO2OBJ -c o2obj.c -o fo2obj.obj
o2obj.obj: o2obj.c
	$(CC) $(CFLAGS) -c o2obj.c



config.h: config-stamp
config-stamp: Makefile
#       -rm -f config.new config-stamp
#       echo define TARGET_CPU       "$(target_cpu)"        > config.new
#       echo define TARGET_ALIAS     "$(target_alias)"     >> config.new
#       echo define TARGET_CANONICAL "$(target_canonical)" >> config.new
#       echo define GAS_VERSION      "$(VERSION)"          >> config.new
#       sed -e s/define/#define/  config.new >> config.h
#       type config.h
	touch config-stamp

# Compiling object files from source files.

app.obj : app.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h
as.obj : as.c as.h host.h targ-env.h obj-format.h output-file.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h subsegs.h \
  tc.h obj.h config.h
atof-generic.obj : atof-generic.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h
bignum-copy.obj : bignum-copy.c as.h host.h \
  targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h
cond.obj : cond.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h

debug.obj : debug.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
  subsegs.h
expr.obj : expr.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h

flonum-konst.obj : flonum-konst.c flonum.h bignum.h
flonum-copy.obj : flonum-copy.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h
flonum-mult.obj : flonum-mult.c flonum.h bignum.h
frags.obj : frags.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
  subsegs.h
hash.obj : hash.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h
hex-value.obj : hex-value.c
input-file.obj : input-file.c as.h host.h \
   targ-env.h obj-format.h targ-cpu.h \
   struc-symbol.h write.h flonum.h bignum.h expr.h \
  frags.h hash.h read.h symbols.h tc.h obj.h input-file.h
input-scrub.obj : input-scrub.c \
  as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
  input-file.h
listing.obj : listing.c as.h host.h targ-env.h flonum.h bignum.h \
  listing.h obj-format.h targ-cpu.h struc-symbol.h write.h expr.h \
  frags.h hash.h read.h symbols.h tc.h obj.h input-file.h
messages.obj : messages.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h
output-file.obj : output-file.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
  output-file.h
read.obj : read.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h
subsegs.obj : subsegs.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
  subsegs.h
symbols.obj : symbols.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
   subsegs.h
write.obj : write.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
  subsegs.h  output-file.h
xmalloc.obj : xmalloc.c
atof-targ.obj : atof-targ.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h \
  symbols.h tc.h obj.h
obj-format.obj : obj-format.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h \
  subsegs.h symbols.h tc.h obj.h
targ-cpu.obj : targ-cpu.c config.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h \
  symbols.h tc.h obj.h $(TARG_CPU_DEPENDENTS)

# Remake the info files.

doc: $(srcdir)/as.info

$(srcdir)/as.info: $(srcdir)/doc/as.texinfo
	@(cd doc; $(MAKE) $(FLAGS_TO_PASS) as.info; mv as.info $srcdir)

clean-here:
	-rm -f $(STAGESTUFF) core *.exe config-stamp config.new

clean:  clean-here

# Like clean but also delete the links made to configure gas.
distclean: clean-here


