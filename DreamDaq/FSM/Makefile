
INCDIR          = $(HOME)/include
LIBDIR          = $(HOME)/lib
BINDIR          = $(HOME)/bin

CPP := g++

GCC_VERSION  := $(shell $(CC) -dumpversion 2>&1)
GCC_MAJOR    := $(shell $(CC) -dumpversion 2>&1 | cut -d'.' -f1)
GCC_MINOR    := $(shell $(CC) -dumpversion 2>&1 | cut -d'.' -f2)
GCC_MMMINOR  := $(shell $(CC) -dumpversion 2>&1 | cut -d'.' -f3)
KERN64BIT       := $(findstring x86_64,$(shell uname -r))

ifdef KERN64BIT
        XARCH := -m64
else
        XARCH := -m32
endif

CFLAGS          := -W -Wall -ansi $(XARCH) -pipe -g -O2 -mtune=core2 -fPIC -ftree-vectorize -fomit-frame-pointer -I.
CPPFLAGS        := $(CFLAGS)
ifeq ($(GCC_VERSION),4.4.7)
        CPPFLAGS += -std=gnu++0x
else ifeq ($(GCC_VERSION),4.8.2)
        CPPFLAGS += -std=c++11
endif
LFLAGS         :=
ROOTLIBS       := `root-config --libs`
LIBS           := -lpthread -lrt
xGPIBLIB        := -lgpibapi

sources := DGTZconfig.cpp eventX742.cpp \
	myV1742.cpp myV262.cpp myV775.cpp myV814.cpp myVme.cpp \
	myV258.cpp myV2718.cpp myV792AC.cpp myV862AC.cpp

headers := $(patsubst %.cpp,%.h,$(sources))
objects := $(patsubst %.cpp,%.o,$(sources))

dreamfsmsrc1 := dreamfsm.cxx dreamDaqClass.cxx
dreamfsmsrc2 := myFIFO-IO.cpp myFIFOWriter.cpp myFIFO.cpp

dreamfsmobj1 := $(patsubst %.cxx,%.o,$(dreamfsmsrc1))
dreamfsmobj2 := $(patsubst %.cpp,%.o,$(dreamfsmsrc2))

localfiles := $(shell ls *.cxx *.cpp *.h)

all: libDreamVme.so dreamfsm
	@echo "****************************************************"
	@echo "        all done with g++ version `g++ -dumpversion`"
	@echo "****************************************************"

copy:
	echo cp -p MakeLib makecopy newdirforlib
	echo cp -p $(sources) newdirforlib
	echo cp -p $(headers) newdirforlib
	echo cp -p $(dreamfsmsrc1) newdirforlib
	echo cp -p $(dreamfsmsrc2) newdirforlib
	echo cp -p WaveDump.h newdirforlib
	echo cp -p strEvX742.h newdirforlib
	echo cp -p noesone.h myModules.h newdirforlib
	echo cp -p dreamDaqClass.h newdirforlib
	echo cp -p myFIFO-IO.h myTekOsc.h newdirforlib
	echo cp -p myFIFO-IOp.h myFIFOWriter.h newdirforlib
	echo cp -p myFIFO.h newdirforlib
	echo cp -p updatable.h newdirforlib

remove:
	echo "cd .. && rm -f MakeLib makecopy $(localfiles)"
	cd .. && rm -f MakeLib makecopy $(localfiles)

libDreamVme.so : $(objects)
	$(CPP) $(CPPFLAGS) -shared -o $@ $^ -lc

$(objects): %.o: %.cpp %.h
	$(CPP) -c $(CPPFLAGS) $< -o $@

dreamfsm: $(dreamfsmobj1) $(dreamfsmobj2)
	$(CPP) $(CPPFLAGS) -o $@ $^ -L. -lDreamVme $(LIBS) $(GPIBLIB) -lCAENVME -lCAENDigitizer

$(dreamfsmobj1): %.o: %.cxx
	$(CPP) -c $(CPPFLAGS) $< -o $@

$(dreamfsmobj2): %.o: %.cpp
	$(CPP) -c $(CPPFLAGS) $< -o $@

clean:
	rm -f libDreamVme.so $(objects)
	rm -f dreamfsm $(dreamfsmobj1) $(dreamfsmobj2)

install:
	@mkdir -p $(INCDIR)
	@mkdir -p $(LIBDIR)
	@mkdir -p $(BINDIR)
	/bin/cp -p *.h $(INCDIR)/
	/bin/cp -p libDreamVme.so $(LIBDIR)/
	/bin/cp -p dreamfsm $(BINDIR)/

