#$Id: Makefile,v 1.26 2009/07/26 10:56:48 dreamdaq Exp $
#
# Makefile per compilare il programma di DAQ
#
# A. Cardini, 8/5/2001
# Updatato il 29/5/2001
# Updatato il 4/6/2001
# Updatato il 24/1/2003, ACa & DRa
#
#
BINDIR		= $(HOME)/bin
ROOTINC         = `root-config --cflags`
#
F77             = g77
CC		= gcc
CPP		= g++
LINKER		= g++
# CFLAGS		= -Wall -O2 -g
CFLAGS		= -Wall -g
32BIT		= -m32
LFLAGS		=
ROOTLIBS        = `root-config --libs`
LIBS		= -lpthread -lrt 


APPS		= reader writer sampler checkfile myEventSelector myDaq myDump \
		myTest myScaler myFileChecker myOnlineChecker myDataWriter \
		provaVME myReadOut myReadOutOsc myReadOutDrs4 myReadOutAll myReadOutCorbo doSwapFADC7-8 fft TH03temp myGetTdc

all: $(APPS)

readout: myReadOut myReadOutOsc myReadOutDrs4 myReadOutAll myReadOutCorbo 

myFileChecker:  myFileChecker.o myDecode.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^

myFileChecker.o: myFileChecker.cpp myFIFO-IOp.h myDecode.h
	$(CPP) $(CFLAGS) $(32BIT) -c myFileChecker.cpp

myDecode.o: myDecode.cpp myFIFO-IOp.h myModules.h myDecode.h
	$(CPP) $(CFLAGS) $(32BIT) -c myDecode.cpp

checkfile: checkfile.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^

checkfile.o: checkfile.c myFIFO-IOp.h
	$(CPP) $(CFLAGS) $(32BIT) -c checkfile.c

myOscDataFile.o: myOscDataFile.cpp myOscDataFile.h
	$(CPP) $(CFLAGS) $(32BIT) -c myOscDataFile.cpp

myEventSelector: myEventSelector.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^	

TH03temp: temp.c
	$(CPP) $(CFLAGS) -o $@ $^ -lm

myTH03.o: myTH03.cpp myTH03.h
	$(CPP) $(CFLAGS) $(32BIT) -c myTH03.cpp

myEventSelector.o: myEventSelector.cpp myFIFO-IOp.h
	$(CPP) $(CFLAGS) $(32BIT) -c myEventSelector.cpp
#
doSwapFADC7-8: doSwapFADC7-8.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^	

doSwapFADC7-8.o: doSwapFADC7-8.cpp myFIFO-IOp.h
	$(CPP) $(CFLAGS) $(32BIT) -c doSwapFADC7-8.cpp
#
myGetTdc.o: myGetTdc.cpp myVme.h myCorbo.h myModules.h \
myFIFOWriter.h myFIFO-IO.h myL1176.h myRunNumber.h
	$(CPP) $(CFLAGS) $(32BIT) -c myGetTdc.cpp

provaVME.o: provaVME.cpp myVme.h myCorbo.h myModules.h myV488.h \
myFIFOWriter.h myFIFO-IO.h myKLOETDC.h mySIS3320.h myL1182.h myV262.h \
myV260.h myL1176.h myRunNumber.h myTekOsc.h myV792AC.h
	$(CPP) $(CFLAGS) $(32BIT) -c provaVME.cpp

myReadOut.o: myReadOut.cpp myVme.h myCorbo.h myModules.h myV488.h \
myFIFOWriter.h myFIFO-IO.h myKLOETDC.h mySIS3320.h myL1182.h myV262.h \
myV260.h myL1176.h myRunNumber.h myTH03.h myTekOsc.h myV792AC.h
	$(CPP) $(CFLAGS) $(32BIT) -c myReadOut.cpp

myReadOutOsc.o: myReadOut.cpp myVme.h myCorbo.h myModules.h myV488.h \
myFIFOWriter.h myFIFO-IO.h myKLOETDC.h mySIS3320.h myL1182.h myV262.h \
myV260.h myL1176.h myRunNumber.h myTH03.h myTekOsc.h myV792AC.h
	$(CPP) $(CFLAGS) $(32BIT) -DUSE_TEK_TDS7254B -o $@ -c myReadOut.cpp

myReadOutDrs4.o: myReadOut.cpp myVme.h myCorbo.h myModules.h myV488.h \
myFIFOWriter.h myFIFO-IO.h myKLOETDC.h mySIS3320.h myL1182.h myV262.h \
myV260.h myL1176.h myRunNumber.h myTH03.h myTekOsc.h myV792AC.h
	$(CPP) $(CFLAGS) $(32BIT) -DUSE_DRS_V1742 -o $@ -c myReadOut.cpp

myReadOutAll.o: myReadOut.cpp myVme.h myCorbo.h myModules.h myV488.h \
myFIFOWriter.h myFIFO-IO.h myKLOETDC.h mySIS3320.h myL1182.h myV262.h \
myV260.h myL1176.h myRunNumber.h myTH03.h myTekOsc.h myV792AC.h
	$(CPP) $(CFLAGS) $(32BIT) -DUSE_TEK_TDS7254B -DUSE_DRS_V1742 -o $@ -c myReadOut.cpp

myReadOutCorbo.o: myReadOutCorbo.cpp myVme.h myCorbo.h myModules.h myV488.h \
myFIFOWriter.h myFIFO-IO.h myKLOETDC.h mySIS3320.h myL1182.h myV262.h \
myV260.h myL1176.h myRunNumber.h myOscDataFile.h
	$(CPP) $(CFLAGS) $(32BIT) -c myReadOutCorbo.cpp

newDaq.o: newDaq.cpp myVme.h myCorbo.h myV767A.h myL1182.h myV260.h myCBD8210.h myModules.h myIO.h myBar.h myThread.h
	$(CPP) $(CFLAGS) $(32BIT) -c newDaq.cpp

myDaq.o: myDaq.cpp myVme.h myV767A.h myL1182.h myV260.h myCBD8210.h myModules.h myIO.h myBar.h myThread.h
	$(CPP) $(CFLAGS) $(32BIT) -c myDaq.cpp
#
newDaq: newDaq.o myIO.o myBar.o 
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^
#
myGetTdc: myGetTdc.o myFIFO-IO.o myFIFOWriter.o myFIFO.o myRunNumber.o myOscDataFile.o sigHandler.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^ $(LIBS) -lgpibapi

provaVME: provaVME.o myFIFO-IO.o myFIFOWriter.o myFIFO.o myRunNumber.o myTH03.o myOscDataFile.o sigHandler.o dreamudp.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^ $(LIBS) -lgpibapi

myReadOut: myReadOut.o myFIFO-IO.o myFIFOWriter.o myFIFO.o myRunNumber.o myTH03.o myOscDataFile.o sigHandler.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^ $(LIBS) -lgpibapi

myReadOutOsc: myReadOutOsc.o myFIFO-IO.o myFIFOWriter.o myFIFO.o myRunNumber.o myTH03.o myOscDataFile.o sigHandler.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^ $(LIBS) -lgpibapi

myReadOutDrs4: myReadOutDrs4.o myFIFO-IO.o myFIFOWriter.o myFIFO.o myRunNumber.o myTH03.o myOscDataFile.o sigHandler.o DGTZconfig.o eventX742.o

	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^ $(LIBS) -lgpibapi -lCAENDigitizer

myReadOutAll: myReadOutAll.o myFIFO-IO.o myFIFOWriter.o myFIFO.o myRunNumber.o myTH03.o myOscDataFile.o sigHandler.o DGTZconfig.o eventX742.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^ $(LIBS) -lgpibapi -lCAENDigitizer

myReadOutCorbo: myReadOutCorbo.o myFIFO-IO.o myFIFOWriter.o myFIFO.o myRunNumber.o myOscDataFile.o sigHandler.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^ $(LIBS) -lgpibapi

#
myDaq: myDaq.o myIO.o myBar.o 
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^ $(LIBS)
#
myIO.o: myIO.c myIO.h myIOp.h myThread.h
	$(CC) $(CFLAGS) $(32BIT) -c myIO.c
#
myFIFO-IO.o: myFIFO-IO.cpp myFIFO-IO.h myFIFO-IOp.h myFIFOWriter.h myOscDataFile.h
	$(CPP) $(CFLAGS) $(32BIT) -c myFIFO-IO.cpp 
#
myDump: myDump.o
	$(CC) $(CFLAGS) $(32BIT) -o $@ $^
#
myDump.o: myDump.c myIOp.h
	$(CC) $(CFLAGS) $(32BIT) -c myDump.c
#
myBar.o: myBar.c myBarp.h myBar.h
	$(CC) $(CFLAGS) $(32BIT) -c myBar.c
#
myTest.o: myTest.cpp myVme.h myV513.h myV792AC.h myModules.h myV488.h
	$(CPP) $(CFLAGS) $(32BIT) -c myTest.cpp
#
myTest: myTest.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^
#
myScaler.o: myScaler.cpp myVme.h myV513.h myV792AC.h myModules.h myV488.h myModules.h myV260.h
	$(CPP) $(CFLAGS) $(32BIT) -c myScaler.cpp
#
myScaler: myScaler.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^
#
reader: reader.o myFIFOReader.o myFIFO.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^
#
myDataWriter: myDataWriter.o myFIFOReader.o myFIFO.o myRunNumber.o sigHandler.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^

myOnlineChecker: myOnlineChecker.o myFIFOSampler.o myFIFO.o myDecode.o
	$(CPP) $(CFLAGS)  -o $@ $^

sampler: sampler.o myFIFOSampler.o myFIFO.o dreammon.o myRawFile.o myRunNumber.o eventX742.o sigHandler.o
	$(CPP) $(CFLAGS)  -o $@ $^ $(LIBS) $(ROOTLIBS)

writer: writer.o myFIFOWriter.o myFIFO.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^

reader.o: reader.cpp myFIFOReader.h
	$(CPP) $(CFLAGS) $(32BIT) -c reader.cpp

myDataWriter.o: myDataWriter.cpp myFIFOReader.h myFIFO-IOp.h myRunNumber.h
	$(CPP) $(CFLAGS) $(32BIT) -c myDataWriter.cpp

myOnlineChecker.o: myOnlineChecker.cpp myFIFOSampler.h myDecode.h
	$(CPP) $(CFLAGS)  -c myOnlineChecker.cpp

sampler.o: sampler.cpp myFIFOSampler.h
	$(CPP) $(CFLAGS)  -c sampler.cpp

writer.o: writer.cpp myFIFOWriter.h myFIFO-IOp.h
	$(CPP) $(CFLAGS) $(32BIT) -c writer.cpp

myFIFOReader.o: myFIFOReader.cpp myFIFOReader.h myFIFO.h myFIFO-IOp.h 
	$(CPP) $(CFLAGS) $(32BIT) -c myFIFOReader.cpp

myFIFOWriter.o: myFIFOWriter.cpp myFIFOWriter.h myFIFO.h myFIFO-IOp.h
	$(CPP) $(CFLAGS) $(32BIT) -c myFIFOWriter.cpp

myFIFOSampler.o: myFIFOSampler.cpp myFIFOSampler.h myFIFO.h myFIFO-IOp.h
	$(CPP) $(CFLAGS)  -c myFIFOSampler.cpp

myFIFO.o: myFIFO.cpp myFIFO.h
	$(CPP) $(CFLAGS) $(32BIT) -c myFIFO.cpp

dreamtcpserv.o: dreamtcpserv.cpp dreamtcpserv.h mySockServ.h
	$(CPP) $(CFLAGS) $(32BIT) -c dreamtcpserv.cpp

dreamudp.o: dreamudp.cpp dreamudp.h mySockUdp.h
	$(CPP) $(CFLAGS) $(32BIT) -c dreamudp.cpp

traceSegv.o: traceSegv.cpp traceSegv.h
	$(CPP) $(CFLAGS) $(32BIT) -c traceSegv.cpp

dreammon.o: dreammon.c myhbook.h myRawFile.h dreammon.h myRunNumber.h myMapping.h
	$(CPP) $(CFLAGS)  $(ROOTINC) -c dreammon.c

#dreammon_new.o: dreammon_new.cxx myRawFile.h dreammon_new.hxx 
#	$(CPP) $(CFLAGS)  $(ROOTINC) -c dreammon_new.cxx

myRunNumber.o: myRunNumber.c myRunNumber.h
	$(CPP) $(CFLAGS) $(32BIT) -c myRunNumber.c

TestMyV1742.o: TestMyV1742.cpp myVme.h myV1742.h
	$(CPP) $(CFLAGS) $(32BIT) -c TestMyV1742.cpp

DGTZconfig.o: DGTZconfig.c
	$(CPP) $(CFLAGS) $(32BIT) -c DGTZconfig.c

eventX742.o: eventX742.c
	$(CPP) $(CFLAGS) $(32BIT) -c eventX742.c

TestMyV1742: TestMyV1742.o DGTZconfig.o eventX742.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^ $(LIBS) -lCAENDigitizer 

#libRawFile.a: myRawFile.o
#	ar r libRawFile.a myRawFile.o
#	-ranlib libRawFile.a

myRawFile.o: myRawFile.c myRawFile.h myRawFilep.h
	$(CPP) $(CFLAGS)  -c myRawFile.c

fortran_close.o: fortran_close.f
	$(F77) -c $^

fft: fft.o
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^

fft.o: fft.c
	$(CC) $(CFLAGS) $(32BIT) -c $^

sigHandler.o: sigHandler.cpp
	$(CPP) $(CFLAGS) $(32BIT) -c $^

prtime: prtime.cpp
	$(CPP) $(CFLAGS) $(32BIT) -o $@ $^

install:
	@mkdir -p $(BINDIR)
	/bin/cp -p $(APPS) $(BINDIR)/

clean:
	@rm -f *.o $(APPS)
#
# End of Makefile
#
