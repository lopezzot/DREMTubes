//$Id: myFIFO-IO.cpp,v 1.19 2009/07/21 09:14:45 dreamdaq Exp $
/*************************************************************************

        myIO.c
        ------

        This file contains all related I/O stuff.

        Version 0.1,      A.Cardini 3/6/2001
        Version 0.2,      D.Raspino 27/7/2005 Data From Slow Control

*************************************************************************/
extern "C" {
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
}
#include <iostream>
#include <vector>

#include "myFIFO-IOp.h"
#include "myFIFO-IO.h"
#include "myFIFOWriter.h"
#include "myOscDataFile.h"
#include "traceSegv.h"

// Some definitions

#define IMAGIC 0xAABBCCDD
#define SMAGIC 0xDDCCBBAA

using namespace std;


// Global variables
int myEventNumber = 0;
vector<updatable *> EvtList;
updatable * thisEv;
unsigned int nupd=0;

//RunHeader   myRH;
unsigned int mybuff[1000000];    // This is the buffer to temporary store all data
EventHeader  * myEH= (EventHeader *)(void *)mybuff;
myFIFOWriter * fifo;
myFIFOWriter * pedFifo;

unsigned int buffsize;                 // Size of how much memory is used
unsigned int * subevsize;

char * daytime()
{
  static char _day_time_[30];
  time_t result = time(NULL);
  memcpy(_day_time_, asctime(localtime(&result)), 24);
  _day_time_[24] = 0;
  return _day_time_;
}

/************************************************************************/
int myUpdateOscData(unsigned int samples, unsigned int run, 
		    unsigned int spill, unsigned int chmask,
		    unsigned int pts)
/************************************************************************/
{
  unsigned int i,j;
  unsigned int f;
  updatable * updateinfo;
  FILE * files[4];
  string names[4];
  unsigned int dataoff[4];
  unsigned int nch=0;

  if(samples==0)
    return 0;
  
  if(samples>nupd){
    cout << "Required samples " << samples << " larger than available " << nupd<< endl;
    return 1;
  }

  // cout << "Going to update " << samples << " events" << endl;

  for(i=0;i<4;i++){
    if((chmask>>i)&1){
      names[nch]=filename(run,spill,i+1,true);
      files[nch]=fopen(names[nch].c_str(),"r");
      
      if(files[nch]==NULL){
	cout << "Cannot open file " << names[nch] << endl;
	return 1;
      }
      //go where the data offset is
      fseek(files[nch],16,SEEK_SET);
      fread(&dataoff[nch],sizeof(unsigned int),1,files[nch]);
      swapbytes(&dataoff[nch],sizeof(unsigned int));
      
      //go where the #FRAME-1 is
      unsigned int frames;
      fseek(files[nch],72,SEEK_SET);
      fread(&frames,sizeof(unsigned int),1,files[nch]);
      swapbytes(&frames,sizeof(unsigned int));
      if((frames+1)<samples){
	cout << "Not enough frames in file " << names[nch] << endl;
	return 1;
      }

      //go where the data is
      fseek(files[nch],dataoff[nch],SEEK_SET);
      nch++;
    }
  }

  unsigned int chdatasizes=chdatasizebyte(pts,nch)/(sizeof(unsigned short)*nch);
  
  for(i=0;i<samples;i++){
    updateinfo = EvtList[i];
    if(updateinfo->datasize!=0){
      unsigned int writtenshort=0;
      unsigned int writtenshort2=0;
      bool trunc=false;
      unsigned int partials=0;
      unsigned short * buffers=0;
      unsigned short * buffer2s=0;
      unsigned int remw=0;
      
      if(updateinfo->partialsize>(sizeof(oscheader)/sizeof(unsigned int))){
	partials=updateinfo->partialsize*sizeof(unsigned int)/sizeof(unsigned short);
	partials-=sizeof(oscheader)/sizeof(unsigned short);
	buffers=(unsigned short *)&(updateinfo->buffer[sizeof(oscheader)/sizeof(unsigned int)]);
	buffer2s=(unsigned short *)(updateinfo->buffer2);
      }else{
	trunc=true;
	remw=(sizeof(oscheader)/sizeof(unsigned int))-updateinfo->partialsize;
	buffer2s=(unsigned short *)&(updateinfo->buffer2[remw]);
      }
      
      for(f=0;f<nch;f++){
	//updatable info is in word
	if(!trunc){
	  unsigned int shorttow=partials-writtenshort;
	  if(shorttow<chdatasizes){
	    //truncated
	    fread(&buffers[writtenshort],sizeof(unsigned short),shorttow,files[f]);
	    
	    writtenshort2+=fread(buffer2s,sizeof(unsigned short),chdatasizes-shorttow,files[f]);
	    trunc=true;
	  }else{
            unsigned int nitems;
	    writtenshort+=(nitems = fread(&buffers[writtenshort],sizeof(unsigned short),chdatasizes,files[f]));
            if(nitems!=chdatasizes)
                  printf("Fread error %s\n",strerror(errno));

	    if(shorttow==chdatasizes)
	      trunc=true;
	  }
	}else{
	  writtenshort2+=fread(&buffer2s[writtenshort2],sizeof(unsigned short),chdatasizes,files[f]);
	}
      }
      
      //swapping
      if(updateinfo->partialsize>(sizeof(oscheader)/sizeof(unsigned int))){
	
	for(j=((sizeof(oscheader)/sizeof(unsigned short)));j<(updateinfo->partialsize*sizeof(unsigned int)/sizeof(unsigned short));j++)
	  swapbytes(&(((short *)updateinfo->buffer)[j]),sizeof(unsigned short));
	
	
	for(j=0;j<((updateinfo->datasize-updateinfo->partialsize)*sizeof(unsigned int)/sizeof(unsigned short));j++)
	  swapbytes(&(((short *)updateinfo->buffer2)[j]),sizeof(unsigned short));
	
      }else{
	for(j=remw*sizeof(unsigned int)/sizeof(unsigned short);j<((updateinfo->datasize-updateinfo->partialsize)*sizeof(unsigned int)/sizeof(unsigned short));j++)
	  swapbytes(&(((short *)updateinfo->buffer2)[j]),sizeof(unsigned short));
      }
      
    }else{
      cout << "Skipping cleared"<< endl;
      for(f=0;f<nch;f++)
	fseek(files[f],chdatasizes*sizeof(unsigned short),SEEK_CUR);
    }
    
  }
  
  for(i=0;i<nch;i++)
    fclose(files[i]);

  nupd=0;

  return 0;
}

/************************************************************************/
int myResetTypeCounters()
/************************************************************************/
{
  unsigned int i;
  
  for(i=0;i<EvtList.size();i++)
    delete EvtList[i];

  EvtList.clear();

  return 0;
}

 
/************************************************************************/
int myMaxEvents()
/************************************************************************/
{
  // Return max events according to 
  // environnment variable MAXEVT
  
  char* envs = getenv("MAXEVT");
  return (envs == NULL) ? 10000 : atol(envs);
}

/************************************************************************/
int myPhysPedRatio()
/************************************************************************/
{
  // Return physics/pedestal ratio according to 
  // environnment variable PHYSPEDRATIO

  char* envs = getenv("PHYSPEDRATIO");
  return (envs == NULL) ? 10 : atol(envs);
}

/************************************************************************/
int myMaxPedEvents()
/************************************************************************/
{
  // Return max pedestal events according to 
  // environnment variable MAXPEDEVT
  
  char* envs = getenv("MAXPEDEVT");
  return (envs == NULL) ? 1000 : atol(envs);
}

/************************************************************************/
int myDownscaleFactor()
/************************************************************************/
{
  // Return downscale factor according to
  // environnment variable DWNSCALE
  
  int dwnscale;

  if (getenv("DWNSCALE") == NULL) 
    dwnscale = 1;                     // Default is no downscale...
  else {
    dwnscale = atol(getenv("DWNSCALE"));
  }
  return dwnscale;
}

/************************************************************************/
int myReloadTDCConfig()
/************************************************************************/
{
  // Return reload TDC Configuration factor according to
  // environnment variable RELOAD_TDC_CONFIG
  
  int rtc;
  
  if (getenv("RELOAD_TDC_CONF") == NULL) 
    rtc = 1;                     // Default is reload
  else {
    rtc = atol(getenv("RELOAD_TDC_CONF"));
  }
  return rtc;
}

/************************************************************************/
int myOpenRun(volatile bool * abort_run)
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

  
  return 0 ; cout << "Waiting for readers ... "<<flush;
  while(!(fifo->isReaderPresent()
	  && pedFifo->isReaderPresent())
	&& !(*abort_run))
    usleep(1000);
  cout << "done" << endl;
  

  return 0;
}

/************************************************************************/
int myCloseRun()
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
int myWriteEvent(unsigned int spill,unsigned int isPhysEvent)
/************************************************************************/
{
  struct timeval tv;
  struct timezone tz;

  // Event time with microsecond resolution...
  gettimeofday(&tv, &tz);
  
  
  // Write Event Header and data
  
  myEH->evmark   =  EVMARK;
  myEH->evhsiz   =  sizeof(EventHeader);
  myEH->evsiz    =  buffsize * sizeof(unsigned int);
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
      usleep(1000);
      tmpfifo->waitlock();
    }else if(result==myFIFO::RDLOCKED){
      cout<< "FIFO blocked by reader" << endl;
    }
  }
  while(result!=myFIFO::SUCCESS);
  
  return 0;
}

/************************************************************************/
int myNewEvent(bool cleared)
/************************************************************************/
{
  
  if(nupd<EvtList.size())
    thisEv=EvtList[nupd];
  else{
    thisEv = new updatable();
    EvtList.push_back(thisEv);
  }
  
  thisEv->datasize=0;
  nupd++;
  
  if(!cleared){
    buffsize = sizeof(EventHeader)/sizeof(unsigned int); // Reset total event length
  }else{
    thisEv->eventnumber=myEventNumber;
    myEventNumber++;
  }
  
  return 0;
}

/************************************************************************/
int myFormatOscSubEvent(unsigned int moduleId)
/************************************************************************/
{
  myFormatSubEvent(moduleId);
  thisEv->dataoffset=buffsize;
  return 0;
}

/************************************************************************/
int myFormatSubEvent(unsigned int moduleId)
/************************************************************************/
{
  SubEventHeader * mySEH;

  mySEH=(SubEventHeader *)&mybuff[buffsize];

  mySEH->semk = SUBEVMARK;
  mySEH->sevhsiz = sizeof(SubEventHeader);
  mySEH->id   = moduleId;
  mySEH->size = 0;
  subevsize = &(mySEH->size);

  buffsize+=sizeof(SubEventHeader)/sizeof(unsigned int);

  return 0;
}

int myUpdateOscSubEvent(unsigned int size)
{
  thisEv->datasize=size;
  myUpdateSubEvent(size);
  return 0;
}

int myUpdateSubEvent(unsigned int size)
{
  if(size>0){
    *subevsize = sizeof(SubEventHeader) + size*sizeof(unsigned int);
    buffsize +=size;
  }else{
    buffsize -=sizeof(SubEventHeader)/sizeof(unsigned int);
  }
 
  return 0;
}


/************************************************************************/
int myFIFOunlock()
/************************************************************************/
{
  fifo->unlock();
  pedFifo->unlock();
  return 0;
}

/************************************************************************/
int myFIFOlock()
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
unsigned int * myBuffer(unsigned int **size)
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

