
BINDIR          = $(HOME)/bin

CC         =  gcc
CPP        = g++
LINKER     = g++

GCC_VERSION  := $(shell $(CC) -dumpversion 2>&1)
GCC_MAJOR    := $(shell $(CC) -dumpversion 2>&1 | cut -d'.' -f1)
GCC_MINOR    := $(shell $(CC) -dumpversion 2>&1 | cut -d'.' -f2)
GCC_MMMINOR  := $(shell $(CC) -dumpversion 2>&1 | cut -d'.' -f3)

INCDIR     =  -I/usr/local/include  
CFLAGS       := -Wall -g $(INCDIR)
CPPFLAGS     := $(CFLAGS)
ifeq ($(GCC_VERSION),4.4.7)
        CPPFLAGS += -std=gnu++0x
endif
LFLAGS     =
ROOTLIBS   = `root-config --libs`

APPS            = demo 
all: $(APPS)


MyFlags = -D_LINUX -D_VXWORKS
LOCAL_CFLAGS = $(MyFlags)
PROGRAMMS = demo
all: $(PROGRAMS)

CFLAGSDRC      := -O2 -g -Wall $(LOCAL_CFLAGS)
CPPFLAGSDRC    := $(CFLAGSDRC)
ifeq ($(GCC_VERSION),4.4.7)
        CPPFLAGSDRC += -std=gnu++0x
endif
LIBS = -lz -lpthread
ROOTVER    =  `root-config --version`
ROOTLIBS   =  `root-config --libs` -lMinuit -lHistPainter -lThread
ROOTGLIBS  =  `root-config --glibs` -lThread
ROOTCFLAGS =  `root-config --cflags`

CFLAGS2 = -O2 -g -Wall  $(INCDIR) $(LOCAL_CFLAGS) 
CPPFLAGS2    := $(CFLAGS2)
ifeq ($(GCC_VERSION),4.4.7)
        CPPFLAGS2 += -std=gnu++0x
endif

#

demo: demo.o 
	$(CPP) -o $@ $^    $(LIBS) $(ROOTGLIBS) 
demo.o: demo.c
	$(CPP) $(CPPFLAGS2) -c demo.c $(ROOTCFLAGS)


clean:
	@rm -f *.o $(APPS)

#
# End of Makefile
#
