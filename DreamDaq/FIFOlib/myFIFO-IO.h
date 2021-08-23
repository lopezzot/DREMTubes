#ifndef _MYFIFO_IO_
#define _MYFIFO_IO_
/*************************************************************************

        myFIFO-IO.h
        -----------

        This file contains all related I/O stuff prototypes
        and useful definitions...

        Version 2014 25.11.2014 roberto.ferrari@pv.infn.it

*************************************************************************/

#include <stddef.h>
#include <time.h>

// Function prototypes

extern "C"{
  int32_t myFIFOunlock();
  int32_t myFIFOlock();
  void myFIFObackup();
  void myFIFOrestore();
  double myFIFOOccupancy();
  double myPedFIFOOccupancy();
  int32_t myMaxEvents();
  double myPhysPedRatio();
  int32_t myMaxPedEvents();
  int32_t myDownscaleFactor();
  int32_t myReloadTDCConfig();
  int32_t myOpenRun(volatile bool* abort_run);
  int32_t myCloseRun();
  int32_t myWriteEvent(uint32_t spill, uint32_t isPhysEvent);
  int32_t myUpdateRunEventNumber();
  void myFIFOUpdateSpillNr();
  void myFIFOUpdateEvtNr(uint32_t nevtphys, uint32_t nevtped );
  void myFIFOGetEvtNr(uint32_t* nevtphys, uint32_t* nevtped );
  int32_t myNewEvent();
  int32_t myFormatSubEvent(uint32_t moduleId);
  int32_t myUpdateSubEvent(uint32_t datasize);
  uint32_t* myBuffer(uint32_t** size);
  int32_t myResetTypeCounters();
}
/************************************************************************/
#endif

