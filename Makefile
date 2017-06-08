# 
#  Main Makefile for the project.
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

SUBDIRS = src
NAME = firebrand
DEV = /dev/parport0

#  Information for releases
VER = 0.1b
DISTDIR = /tmp/$(NAME)-$(VER)
SRCDIR = $(shell pwd)

.PHONY: $(NAME) $(SUBDIRS) clean release device

$(NAME): $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@ $(NAME)

localclean:
	rm -f *~ *.bak

clean: localclean
	@$(MAKE) -C src clean

veryclean: localclean
	@$(MAKE) -C src veryclean

device: $(DEV)
$(DEV):
	sudo mknod $@ c 99 0
	sudo chown root:lp $@
	sudo chmod 660 $@

release: veryclean
	[ -e $(DISTDIR) ] && rm -rf $(DISTDIR); mkdir $(DISTDIR)
	cp -r * $(DISTDIR)
	find $(DISTDIR) -name CVS |xargs rm -rf 
	find $(DISTDIR) -name .cvsignore -exec rm -rf \{\} \;
	find $(DISTDIR) -iname \*.pdf -exec rm -rf \{\} \;
	(cd $(DISTDIR)/.. && tar czf $(SRCDIR)/../$(NAME)-$(VER).tar.gz $(NAME)-$(VER) )
	rm -rf $(DISTDIR)

