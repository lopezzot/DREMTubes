#############################################################################
#
# Version 2014 roberto.ferrari@pv.infn.it
#
#############################################################################

BINDIR          := $(HOME)/bin
VMELIBDIR       := ../VMElib
FIFOLIBDIR      := ../FIFOlib
INCDIR          := ..

F77             := g77
CC              := gcc
CPP             := g++
LINKER          := g++

GCC_VERSION     := $(shell $(CC) -dumpversion 2>&1)
GCC_MAJOR       := $(shell $(CC) -dumpversion 2>&1 | cut -d'.' -f1)
GCC_MINOR       := $(shell $(CC) -dumpversion 2>&1 | cut -d'.' -f2)
GCC_MMMINOR     := $(shell $(CC) -dumpversion 2>&1 | cut -d'.' -f3)
KERN64BIT       := $(findstring x86_64,$(shell uname -r))

ifdef KERN64BIT
        XARCH := -m64
else
        XARCH := -m32
endif

CFLAGS          := -W -Wall -ansi $(XARCH) -pipe -g -O2 -mtune=core2 -fPIC -ftree-vectorize -fomit-frame-pointer -I$(VMELIBDIR) -I$(FIFOLIBDIR) -I$(INCDIR)
CPPFLAGS        := $(CFLAGS)
ifeq ($(GCC_VERSION),4.4.7)
        CPPFLAGS += -std=gnu++0x
else ifeq ($(GCC_VERSION),4.8.2)
        CPPFLAGS += -std=c++11
endif
CFLAGS2         := -W -Wall -ansi $(XARCH) -pipe -g -O2 -mtune=core2 -fPIC -ftree-vectorize -fomit-frame-pointer  -I$(VMELIBDIR) -I$(FIFOLIBDIR) -I$(INCDIR) $(LOCAL_CFLAGS)
CPPFLAGS2       := $(CFLAGS2)
ifeq ($(GCC_VERSION),4.4.7)
        CPPFLAGS2 += -std=gnu++0x
else ifeq ($(GCC_VERSION),4.8.2)
        CPPFLAGS2 += -std=c++11
endif

APPS            := myDaq2Ntu csv_dream merge2017 ntuplizeMADA find_offset run_offset merge2018
all: $(APPS)

LOCAL_CFLAGS    := -D_LINUX -D_VXWORKS

LIBS            := -L$(VMELIBDIR) -lDreamVme -L$(FIFOLIBDIR) -lDreamFIFO -lz -lpthread -lrt -lCAENDigitizer -lCAENComm -lCAENVME
ROOTVER         := `root-config --version`
ROOTLIBS        := `root-config --libs` -lMinuit -lHistPainter -lThread
ROOTGLIBS       := `root-config --glibs` -lThread
ROOTCFLAGS      := `root-config --cflags`

myDaq2Ntu: myDaq2Ntu.o
	$(CPP) -o $@ $^ $(LIBS) $(ROOTGLIBS)

myDaq2Ntu.o: myDaq2Ntu.cpp ../mapping.h
	$(CPP) $(CPPFLAGS2) -o $@ -c myDaq2Ntu.cpp $(ROOTCFLAGS)


csv_dream: csv_dream.o
	$(CPP) -o $@ $^ $(LIBS) $(ROOTGLIBS)

csv_dream.o: csv_dream.cpp
	$(CPP) $(CPPFLAGS2) -o $@ -c csv_dream.cpp $(ROOTCFLAGS)

merge2018: merge2018.o
	$(CPP) -o $@ $^ $(LIBS) $(ROOTGLIBS)

merge2018.o: merge2018.cpp
	$(CPP) $(CPPFLAGS2) -o $@ -c merge2017.cpp $(ROOTCFLAGS)

merge2017: merge2017.o
	$(CPP) -o $@ $^ $(LIBS) $(ROOTGLIBS)

merge2017.o: merge2017.cpp
	$(CPP) $(CPPFLAGS2) -o $@ -c merge2017.cpp $(ROOTCFLAGS)

ntuplizeMADA: ntuplizeMADA.o
	$(CPP) -o $@ $^ $(LIBS) $(ROOTGLIBS)

ntuplizeMADA.o: ntuplizeMADA.cpp
	$(CPP) $(CPPFLAGS2) -o $@ -c ntuplizeMADA.cpp $(ROOTCFLAGS)

find_offset: find_offset.o
	$(CPP) -o $@ $^ $(LIBS) $(ROOTGLIBS)

find_offset.o: find_offset.cpp
	$(CPP) $(CPPFLAGS2) -o $@ -c find_offset.cpp $(ROOTCFLAGS)

run_offset: run_offset.o
	$(CPP) -o $@ $^ $(LIBS) $(ROOTGLIBS)

run_offset.o: run_offset.cpp
	$(CPP) $(CPPFLAGS2) -o $@ -c run_offset.cpp $(ROOTCFLAGS)



install:
	@mkdir -p $(BINDIR)
	/bin/cp -p $(APPS) $(BINDIR)/

clean:
	@rm -f *.o $(APPS)

#
# End of Makefile
#
