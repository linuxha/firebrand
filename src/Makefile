# 
#  Makefile for the src directory and subdirectories there under.
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

SUBDIRS = library CLI misc 

.PHONY: firebrand clean $(SUBDIRS:%=%__clean__) $(SUBDIRS:%=%__firebrand__)

firebrand: $(SUBDIRS:%=%__firebrand__)
clean: $(SUBDIRS:%=%__clean__)
veryclean:  $(SUBDIRS:%=%__veryclean__)

$(SUBDIRS:%=%__clean__):
	@$(MAKE) -C ${@:%__clean__=%} clean

$(SUBDIRS:%=%__veryclean__):
	@$(MAKE) -C ${@:%__veryclean__=%} veryclean

$(SUBDIRS:%=%__firebrand__):
	$(MAKE) -C ${@:%__firebrand__=%} all

CLI: library
