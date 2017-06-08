# 
#  Makefile for the library.
# 
#  Copyright 2005 Paul Millar
# 
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
# 
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
# 
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# 

CC=gcc
AR=ar
CFLAGS = -Wall
LIBRARY = libfirebrand.a
PROGRAM = simpleTest

#
#  Choose SECURITY of "root", "user", or "lp"
#
SECURITY = user


OBJS=m-seq.o misc.o pin_utils.o generic.o chipinfo.o 27xxx.o 

all: $(LIBRARY) $(PROGRAM)

$(LIBRARY): $(OBJS)
	ar r $@ $(OBJS)

simpleTest: simpleTest.c $(LIBRARY)
	gcc -o $@ $< -lfirebrand -L.
ifeq ($(SECURITY),root)
	sudo chown root $@
	sudo chmod 4755 $@
else
ifeq ($(SECURITY),lp)
	sudo chgrp lp $@
	sudo chmod 2755 $@
endif
endif

clean:
	rm -f *.o *~ .#*

veryclean: clean
	rm -f $(PROGRAM) $(LIBRARY)


m-seq.o: m-seq.h
misc.o: misc.h pin_utils.h
pin_utils.o: pin_utils.h generic.h
generic.o: misc.h generic.h pin_utils.h m-seq.h
chipinfo.o: chipinfo.h 27xxx.h
27xxx.o: chipinfo.h generic.h pin_utils.h misc.h

