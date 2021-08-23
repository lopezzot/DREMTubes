#############################################################################
#
#    Makefile per compilare i seguenti files:
#
# 1) Libreria per la decodifica dei raw data libRawFile.a
# 2) Programma di dump per i file raw che utilizza la libreria di cui sopra
# 3) Programma per scrivere le ntuple dai raw data
#
# A. Cardini, 10/01/2002
#
#############################################################################

BINDIR          = $(HOME)/bin

CC         =  gcc
F77        =  g77
INCDIR     =  -I/usr/local/include  
CFLAGS     = -Wall -g $(INCDIR)
ROOTLIBS   = `root-config --libs`
CPP        = g++
LINKER     = g++
CFLAGS     = -Wall -g $(INCDIR)
LFLAGS     =

APPS            = myDaq2Ntu doOfflineHisto
all: $(APPS)


#DRC
#SUBDIRS = MCommon

MyFlags = -D_LINUX -D_VXWORKS
LOCAL_CFLAGS = $(MyFlags)
PROGRAMMS = myDaq2Ntu
all: $(PROGRAMS)
#OBJECTS = $(SUBDIRS:=/*.o)
#INCDIRS = -I. $(SUBDIRS:%=-I%)
#MRPROPERS = $(SUBDIRS:=.mrproper)
#CLEANERS  = $(SUBDIRS:=.cleaner)
#LIBRARIES = $(SUBDIRS:=.all)

CFLAGSDRC = -O2 -g -Wall $(LOCAL_CFLAGS)
LIBS = -lz -lpthread
ROOTVER    =  `root-config --version`
ROOTLIBS   =  `root-config --libs` -lMinuit -lHistPainter -lThread
ROOTGLIBS  =  `root-config --glibs` -lThread
ROOTCFLAGS =  `root-config --cflags`

CFLAGS2 = -O2 -g -Wall  $(INCDIR) $(LOCAL_CFLAGS) 

#

../myRawFile.o: 
	$(MAKE) -C ../. myRawFile.o

../dreammon.o: 
	$(MAKE) -C ../. dreammon.o

../eventX742.o: 
	$(MAKE) -C ../. eventX742.o

../myRunNumber.o: 
	$(MAKE) -C ../. myRunNumber.o

myDaq2Ntu: myDaq2Ntu.o ../myRawFile.o      
	$(CPP) -o $@ $^    $(LIBS) $(ROOTGLIBS)
myDaq2Ntu.o: myDaq2Ntu.c
	$(CPP) $(CFLAGS2) -c myDaq2Ntu.c $(ROOTCFLAGS)

doOfflineHisto: doOfflineHisto.o ../myRawFile.o ../dreammon.o ../myRunNumber.o ../eventX742.o
	$(CPP) -o $@ $^ $(ROOTLIBS)
doOfflineHisto.o: doOfflineHisto.c
	$(CPP) $(CFLAGS) -c doOfflineHisto.c


#$(LIBRARIES):
#	@echo " Calling make in $(@:.all=)"
#	@(cd $(@:.all=); $(MAKE) -f Makefile all)


install:
	@mkdir -p $(BINDIR)
	/bin/cp $(APPS) $(BINDIR)/

clean:
	@rm -f *.o $(APPS)

#
# End of Makefile
#
