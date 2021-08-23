/*************************************************************************

        myIO.c
        ------

        This file contains all related I/O stuff.

        Version 2014 25.11.2014 roberto.ferrari@pv.infn.it

*************************************************************************/
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <vector>

#include "myFIFO-IOp.h"
#include "myFIFO-IO.h"
#include "myFIFOWriter.h"

// Some definitions

#define IMAGIC 0xAABBCCDD
#define SMAGIC 0xDDCCBBAA

using namespace std;

// Global variables
int32_t myEventNumber = 0;
vector<updatable *> EvtList;
updatable * thisEv;
uint32_t nupd=0;

//RunHeader   myRH;
uint32_t mybuff[1000000];    // This is the buffer to temporary store all data
EventHeader* myEH = (EventHeader*)(void*)mybuff;
myFIFOWriter* fifo;
myFIFOWriter* pedFifo;

uint32_t buffsize;                 // Size of how much memory is used
uint32_t* subevsize;

char* daytime()
{
  static char _day_time_[30];
  time_t result = time(NULL);
  memcpy(_day_time_, asctime(localtime(&result)), 24);
  _day_time_[24] = 0;
  return _day_time_;
}

/************************************************************************/
int32_t myResetTypeCounters()
/************************************************************************/
{
  uint32_t i;
  
  for(i=0;i<EvtList.size();i++)
    delete EvtList[i];

  EvtList.clear();

  return 0;
}

 
/************************************************************************/
int32_t myMaxEvents()
/************************************************************************/
{
  // Return max events according to 
  // environnment variable MAXEVT
  
  char* envs = getenv("MAXEVT");
  return (envs == NULL) ? 10000 : atol(envs);
}

/************************************************************************/
double myPhysPedRatio()
/************************************************************************/
{
  // Return physics/pedestal ratio according to 
  // environnment variable PHYSPEDRATIO

  char* envs = getenv("PHYSPEDRATIO");
  double r = (envs == NULL) ? 10 : atof(envs);
  return (r > 0) ? r : 10;
}

/************************************************************************/
int32_t myMaxPedEvents()
/************************************************************************/
{
  // Return max pedestal events according to 
  // environnment variable MAXPEDEVT
  
  char* envs = getenv("MAXPEDEVT");
  return (envs == NULL) ? 1000 : atol(envs);
}

/************************************************************************/
int32_t myDownscaleFactor()
/************************************************************************/
{
  // Return downscale factor according to
  // environnment variable DWNSCALE
  
  int32_t dwnscale;

  if (getenv("DWNSCALE") == NULL) 
    dwnscale = 1;                     // Default is no downscale...
  else {
    dwnscale = atol(getenv("DWNSCALE"));
  }
  return dwnscale;
}

/************************************************************************/
int32_t myReloadTDCConfig()
/************************************************************************/
{
  // Return reload TDC Configuration factor according to
  // environnment variable RELOAD_TDC_CONFIG
  
  int32_t rtc;
  
  if (getenv("RELOAD_TDC_CONF") == NULL) 
    rtc = 1;                     // Default is reload
  else {
    rtc = atol(getenv("RELOAD_TDC_CONF"));
  }
  return rtc;
}

/************************************************************************/
int32_t myOpenRun(volatile bool* abort_run)
/************************************************************************/
{
  pedFifo = new myFIFOWriter(PED_BASE_KEY);

  if(pedFifo->isvalid()){
    cout << "Pedestal fifo is valid" << endl;
  }
  else{
    cout << "Pedestal fifo is not valid" << endl;
    *abort_run=true;
    return 1;
  }

  pedFifo->waitlock();


  fifo = new myFIFOWriter(PHYS_BASE_KEY);

  if(fifo->isvalid()){
    cout << "Event fifo is valid" << endl;
  }
  else{
    cout << "Event fifo is not valid" << endl;
    *abort_run=true;
    return 1;
  }

  fifo->waitlock();

  
  return 0 ; cout << "Waiting for readers ... " << flush;
  while(!(fifo->isReaderPresent()
	  && pedFifo->isReaderPresent())
	&& !(*abort_run))
    usleep(10);
  cout << "done" << endl;
  

  return 0;
}

/************************************************************************/
int32_t myCloseRun()
/************************************************************************/
{
  myResetTypeCounters();
  fifo->unlock();
  pedFifo->unlock();
  delete fifo;
  delete pedFifo;
  return 0;
}

static uint32_t n_full(0);

/************************************************************************/
int32_t myWriteEvent(uint32_t spill, uint32_t isPhysEvent)
/************************************************************************/
{
  struct timeval tv;
  struct timezone tz;

  // Event time with microsecond resolution...
  gettimeofday(&tv, &tz);
  
  
  // Write Event Header and data
  
  myEH->evmark   =  EVMARK;
  myEH->evhsiz   =  sizeof(EventHeader);
  myEH->evsiz    =  buffsize* sizeof(uint32_t);
  myEH->evnum    =  myEventNumber;
  myEH->spill    =  spill;
  myEH->tsec     =  tv.tv_sec;                       // Event time in seconds
  myEH->tusec    =  tv.tv_usec;                      // ...and in microseconds
  
  myFIFOWriter * tmpfifo;
  
  thisEv->eventnumber=myEventNumber;

  if(isPhysEvent){
    tmpfifo=fifo;
  }else{
    tmpfifo=pedFifo;
  }

  myEventNumber++;

  myFIFO::result result;

  do{
    result = tmpfifo->write(mybuff,thisEv);
    
    if(result==myFIFO::FIFOFULL){
      if ((n_full & 0xFF) == 0){
        if(isPhysEvent)
	  cout<< "Data FIFO is FULL" << endl;
        else
	  cout<< "Pedestal FIFO is FULL" << endl;
      }
      n_full ++;

      tmpfifo->unlock();
      usleep(10);
      tmpfifo->waitlock();
    }else if(result==myFIFO::RDLOCKED){
      cout<< "FIFO blocked by reader" << endl;
    }
  }
  while(result!=myFIFO::SUCCESS);
  
  return 0;
}

/************************************************************************/
int32_t myNewEvent()
/************************************************************************/
{
  if (nupd<EvtList.size())
    thisEv = EvtList[nupd];
  else{
    thisEv = new updatable();
    EvtList.push_back(thisEv);
  }
  
  thisEv->datasize = 0;
  nupd++;
  buffsize = sizeof(EventHeader)/sizeof(uint32_t); // Reset total event length
  
  return 0;
}

/************************************************************************/
int32_t myFormatSubEvent(uint32_t moduleId)
/************************************************************************/
{
  SubEventHeader * mySEH;

  mySEH=(SubEventHeader *)&mybuff[buffsize];

  mySEH->semk = SUBEVMARK;
  mySEH->sevhsiz = sizeof(SubEventHeader);
  mySEH->id   = moduleId;
  mySEH->size = 0;
  subevsize = &(mySEH->size);

  buffsize+=sizeof(SubEventHeader)/sizeof(uint32_t);

  return 0;
}

int32_t myUpdateSubEvent(uint32_t size)
{
  if(size>0){
    *subevsize = sizeof(SubEventHeader) + size*sizeof(uint32_t);
    buffsize +=size;
  }else{
    buffsize -=sizeof(SubEventHeader)/sizeof(uint32_t);
  }
 
  return 0;
}

/************************************************************************/
int32_t myFIFOunlock()
/************************************************************************/
{
  fifo->unlock();
  pedFifo->unlock();
  return 0;
}

/************************************************************************/
int32_t myFIFOlock()
/************************************************************************/
{
  fifo->waitlock();
  fifo->updateSamplingPoint();
  pedFifo->waitlock();
  pedFifo->updateSamplingPoint();
  return 0;
}

/************************************************************************/
double myFIFOOccupancy()
/************************************************************************/
{
  return fifo->bufferOccupancy();
}

/************************************************************************/
double myPedFIFOOccupancy()
/************************************************************************/
{
  return pedFifo->bufferOccupancy();
}

/************************************************************************/
uint32_t* myBuffer(uint32_t** size)
/************************************************************************/
{
  *size=&buffsize;

  return mybuff;
}

/************************************************************************/
void myFIFOUpdateSpillNr()
/************************************************************************/
{
   fifo->updateSpillNr();
   pedFifo->updateSpillNr();
}

/************************************************************************/
void myFIFOUpdateEvtNr( uint32_t nevtphys, uint32_t nevtped )
/************************************************************************/
{
   fifo->updateNevt( nevtphys );
   pedFifo->updateNevt( nevtped );
}

/************************************************************************/
void myFIFOGetEvtNr( uint32_t* nevtphys, uint32_t* nevtped )
/************************************************************************/
{
   *nevtphys = fifo->getNevt();
   *nevtped = pedFifo->getNevt();
}

/************************************************************************/
void myFIFObackup()
/************************************************************************/
{
   fifo->backup();
   pedFifo->backup();
}

/************************************************************************/
void myFIFOrestore()
/************************************************************************/
{
   fifo->restore();
   pedFifo->restore();
}

