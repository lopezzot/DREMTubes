ROOTCONFIG   := root-config

ARCH         := $(shell $(ROOTCONFIG) --arch)
PLATFORM     := $(shell $(ROOTCONFIG) --platform)

DAQEVENTDIR   = ../../DreamOffline/DaqEvent
INCLUDES      = -I.. -I$(DAQEVENTDIR)
CXX           =  g++
CXXFLAGS      = -g -Wall -fPIC $(INCLUDES)
LD            = g++
LDFLAGS       = -g
SOFLAGS       = -shared

ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)
HASTHREAD    := $(shell $(ROOTCONFIG) --has-thread)

CXXFLAGS     += $(ROOTCFLAGS)
LDFLAGS      += $(ROOTLDFLAGS)
LIBS          = $(ROOTLIBS) $(SYSLIBS)
GLIBS         = -L$(DAQEVENTDIR) -lDaqEvent $(ROOTGLIBS) $(SYSLIBS) 

OFILES = DreamDaqEventUnpacker.o DaqModuleUnpackers.o \
         DreamDaqModuleUnpacker.o DreamRawEventBrowser.o \
         dream_md5.o

DAQOBJ = ../myRawFile.o ../myOscDataFile.o 

.SUFFIXES: .cc .hh .h .c .o .so

%.o : %.cc
	$(CXX) $(CXXFLAGS) -MD -c $<
	@sed -i 's,\($*\.o\)[:]*\(.*\),$@: $$\(wildcard\2\)\n\1:\2,g' $*.d

%.o : %.c
	$(CXX) $(CXXFLAGS) -MD -c $<
	@sed -i 's,\($*\.o\)[:]*\(.*\),$@: $$\(wildcard\2\)\n\1:\2,g' $*.d

all: daqobj root2dream dream2root

daqobj:
	(cd ..; make myRawFile.o)
	(cd ..; make myOscDataFile.o)

root2dream: $(OFILES) root2dream.o
	$(LD) $(LDFLAGS) $^ $(DAQOBJ) $(GLIBS) -o $@

dream2root: $(OFILES) dream2root.o
	$(LD) $(LDFLAGS) $^ $(DAQOBJ) $(GLIBS) -o $@

clean:
	rm -f root2dream dream2root core.* *.d *~ *.o *.so

-include $(OFILES:.o=.d)
