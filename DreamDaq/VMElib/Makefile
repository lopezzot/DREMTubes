
LIBDIR          = $(HOME)/lib

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

sources := DGTZconfig.cpp eventX742.cpp eventX742_corrections.cpp \
	X742CorrectionRoutines.cpp X742DecodeRoutines.cpp \
	myV1742.cpp myV262.cpp myV513.cpp myV775N.cpp myV775.cpp myV814.cpp myV258.cpp \
	myV2718.cpp myV792AC.cpp myV862AC.cpp myVme.cpp sigHandler.cpp

objects := $(patsubst %.cpp,%.o,$(sources))

localfiles := $(shell ls *.cpp *.h)

all: libDreamVme.so
	@echo "****************************************************"
	@echo "        all done with g++ version `g++ -dumpversion`"
	@echo "****************************************************"

libDreamVme.so: $(objects)
	$(CPP) $(CPPFLAGS) -shared -o $@ $^ -lc

$(objects): %.o: %.cpp %.h
	$(CPP) -c $(CPPFLAGS) $< -o $@

clean:
	rm -f libDreamVme.so $(objects)

headers:
	@echo "nothing to be done here"

install:
	@mkdir -p $(LIBDIR)
	/bin/cp -p libDreamVme.so $(LIBDIR)/

