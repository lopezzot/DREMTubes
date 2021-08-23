//$Id: myFIFO-IO.h,v 1.11 2009/07/21 09:14:45 dreamdaq Exp $
#ifndef _MYIO_
#define _MYIO_
/*************************************************************************

        myIO.h
        ------

        This file contains all related I/O stuff prototypes
        and useful definitions...

        Version 0.1,      A.Cardini 3/6/2001

*************************************************************************/

#include <stddef.h>
#include <time.h>

// Function prototypes

extern "C"{
  int myFIFOunlock();
  int myFIFOlock();
  void myFIFObackup();
  void myFIFOrestore();
  double myFIFOOccupancy();
  double myPedFIFOOccupancy();
  int myMaxEvents();
  int myPhysPedRatio();
  int myMaxPedEvents();
  int myDownscaleFactor();
  int myReloadTDCConfig();
  int myOpenRun(volatile bool * abort_run);
  int myCloseRun();
  int myWriteEvent(unsigned int spill,unsigned int isPhysEvent);
  int myUpdateRunEventNumber();
  void myFIFOUpdateSpillNr();
  int myNewEvent(bool cleared);
  int myFormatSubEvent(unsigned int moduleId);
  int myUpdateSubEvent(unsigned int datasize);
  int myFormatOscSubEvent(unsigned int moduleId);
  int myUpdateOscSubEvent(unsigned int datasize);
  unsigned int * myBuffer(unsigned int **size);
  int myResetTypeCounters();
  int myUpdateOscData(unsigned int samples, unsigned int run,
		      unsigned int spill, unsigned int chmask,
		      unsigned int pts);
}
/************************************************************************/
#endif

