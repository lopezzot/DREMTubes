
LIBDIR          := $(HOME)/lib

CPP             := g++

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

CFLAGS          := -W -Wall -ansi $(XARCH) -pipe -g -O2 -mtune=core2 -fPIC -ftree-vectorize -fomit-frame-pointer -I.
CPPFLAGS        := $(CFLAGS)
ifeq ($(GCC_VERSION),4.4.7)
        CPPFLAGS += -std=gnu++0x
else ifeq ($(GCC_VERSION),4.8.2)
        CPPFLAGS += -std=c++11
endif
ROOTLIBS        := `root-config --libs`
LIBS            := -lpthread -lrt

sources := $(shell ls *.cpp)

objects := $(patsubst %.cpp,%.o,$(sources))

all: libDreamFIFO.so
	@echo "****************************************************"
	@echo "        all done with g++ version `g++ -dumpversion`"
	@echo "****************************************************"

libDreamFIFO.so: $(objects)
	$(CPP) $(CPPFLAGS) -shared -o $@ $^ -lc

myFIFO-IO.o: myFIFO-IO.cpp myFIFO-IO.h myFIFO-IOp.h myFIFOWriter.h
	$(CPP) $(CPPFLAGS) -c myFIFO-IO.cpp 

myFIFOReader.o: myFIFOReader.cpp myFIFOReader.h myFIFO.h myFIFO-IOp.h
	$(CPP) $(CPPFLAGS) -c myFIFOReader.cpp

myFIFOWriter.o: myFIFOWriter.cpp myFIFOWriter.h myFIFO.h myFIFO-IOp.h
	$(CPP) $(CPPFLAGS) -c myFIFOWriter.cpp

myFIFOSampler.o: myFIFOSampler.cpp myFIFOSampler.h myFIFO.h myFIFO-IOp.h
	$(CPP) $(CPPFLAGS) -c myFIFOSampler.cpp

myFIFO.o: myFIFO.cpp myFIFO.h
	$(CPP) $(CPPFLAGS) -c myFIFO.cpp

myRawFile.o: myRawFile.cpp myRawFile.h
	$(CPP) $(CPPFLAGS) -c myRawFile.cpp

clean:
	rm -f libDreamFIFO.so $(objects)

headers:
	@echo "nothing to be done here"

install:
	@mkdir -p $(LIBDIR)
	/bin/cp -p libDreamFIFO.so $(LIBDIR)/

