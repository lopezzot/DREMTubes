#############################################################################
#
# Version 2014 roberto.ferrari@pv.infn.it
#
#############################################################################

BINDIR          = $(HOME)/bin

CPP        = g++
LINKER     = g++
CFLAGS     = -O2 -g -Wall
ROOTVER    =  `root-config --version`
ROOTLIBS   =  `root-config --libs`
ROOTGLIBS  =  `root-config --glibs`
ROOTCFLAGS =  `root-config --cflags`

APPS       = pedestal_drs_file

all: $(APPS)
	@echo "****************************************************"
	@echo "        all done with g++ version `g++ -dumpversion`"
	@echo "****************************************************"

pedestal_drs_file: pedestal_drs_file.cxx
	$(CPP) -o $@ $^ `root-config --cflags` `root-config --glibs`

install:
	@mkdir -p $(BINDIR)
	/bin/cp $(APPS) $(BINDIR)/

clean:
	@rm -f *.o $(APPS)

#
# End of Makefile
#
