# 
#  Makefile for CLI
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

CC = gcc
LIBDIR = ../library
CFLAGS = -I$(LIBDIR) -Wall
LFLAGS = -L$(LIBDIR)
CMDS = read write isBlank listChips chipInfo

OBJS = $(CMDS:%=cmd_%.o)
HDRS = $(CMDS:%=cmd_%.h)

TARGET = firebrand

#
#  Choose SECURITY of "root", "user", or "lp"
#
SECURITY = user

all: $(TARGET)

firebrand: main.c $(OBJS) generic-param.h $(HDRS) $(LIBDIR)/libfirebrand.a
	$(CC) -o $@ $(CFLAGS) $(LFLAGS) main.c $(OBJS) -lfirebrand
ifeq ($(SECURITY),root)
	sudo chown root $@
	sudo chmod 4755 $@
else
ifeq ($(SECURITY),lp)
	sudo chgrp lp $@
	sudo chmod 2755 $@
endif
endif

cmd_read.o: generic-param.h
cmd_write.o: generic-param.h
cmd_isBlank.o: generic-param.h

$(LIBDIR)/libfirebrand.a:
	make -C $(LIBDIR) all

clean:
	@rm -f *.o *~

veryclean: clean
	@rm -f $(TARGET)

.PHONY: $(CMDS)
