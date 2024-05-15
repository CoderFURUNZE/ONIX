# Copyright (C) 2001-2013  The Bochs Project
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


.SUFFIXES: .cc


top_builddir = ..

SHELL = /bin/sh



CC = gcc
CXX = g++
CFLAGS = -march=x86-64 -mtune=generic -O2 -pipe -fno-plt -fexceptions         -Wp,-D_FORTIFY_SOURCE=3 -Wformat -Werror=format-security         -fstack-clash-protection -fcf-protection         -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -g -ffile-prefix-map=/home/furunze/ONIX/onix/bochs/src=/usr/src/debug/bochs-gdb -flto=auto -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES   $(X_CFLAGS) -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
CXXFLAGS = -march=x86-64 -mtune=generic -O2 -pipe -fno-plt -fexceptions         -Wp,-D_FORTIFY_SOURCE=3 -Wformat -Werror=format-security         -fstack-clash-protection -fcf-protection         -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -Wp,-D_GLIBCXX_ASSERTIONS -g -ffile-prefix-map=/home/furunze/ONIX/onix/bochs/src=/usr/src/debug/bochs-gdb -flto=auto -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES   $(X_CFLAGS) -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT

LDFLAGS = -Wl,-O1 -Wl,--sort-common -Wl,--as-needed -Wl,-z,relro -Wl,-z,now          -Wl,-z,pack-relative-relocs -flto=auto
LIBS = 
RANLIB = ranlib
LIBTOOL=$(SHELL) $(top_builddir)/libtool


.cc.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@


all: make_cmos_image

make_cmos_image: make_cmos_image.o
	$(LIBTOOL) --mode=link --tag CXX $(CXX) -o $@ $(CXXFLAGS_CONSOLE) $(LDFLAGS) make_cmos_image.o

clean:
	rm -f  *.o
	rm -f  make_cmos_image

dist-clean: clean
	rm -f  Makefile
