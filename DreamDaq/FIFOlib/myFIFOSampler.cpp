#include <cstring>
#include <iostream>

#include "myFIFOSampler.h"
#include "myFIFO-IOp.h"

using namespace std;

static uint32_t old_spill_nr(1000000);

myFIFO::result myFIFOSampler::read(uint32_t* buf, uint32_t* size){
 
  if(internalData->status!=myFIFO::LOCKEDFORSM
     && internalData->status!=myFIFO::UNLOCKED){
    return myFIFO::WRLOCKED;
  }

  if(internalData->status==myFIFO::LOCKEDFORRD){
    return myFIFO::RDLOCKED;
  }

  uint32_t offsetWR=internalData->offsetWR;
  uint32_t offsetSM=internalData->offsetSM[sam_ind];
  
  if(offsetSM==offsetWR)
    if(internalData->monspace[sam_ind]==myFIFO::EMPTY) {
      return myFIFO::FIFOEMPTY;
    }

  uint32_t fifosize=internalData->size;
  uint32_t* start =internalData->startSM[sam_ind];
  EventHeader* head =(EventHeader*)&(start[offsetSM]);
  
/*  uint32_t evmark;           // Beginning of event marker - 0xcafecafe
  uint32_t evhsiz;           // Event Header size in bytes
  uint32_t evsiz;            // Event size in bytes (including header)
  uint32_t evnum;            // Event number
  uint32_t spill;            // Spill number
  uint32_t tsec;             // Seconds in the day from gettimeofday
  uint32_t tusec;            // Microseconds in the day from gettimeofday
*/

  uint32_t bytesize=head->evsiz;
  uint32_t datasize=bytesize/sizeof(uint32_t);
  uint32_t endoffset=offsetSM+datasize;
  uint32_t spill=head->spill;
  bool printdebug=false; // (old_spill_nr != spill);
  old_spill_nr = spill;
if (printdebug) {
cout << hex << sam_ind << " start " << start << " offsetSM " << offsetSM << " offsetWR " << offsetWR << " head " << head;

cout << "\n 0 (offsetSM >= offsetWR) start " << start << " offsetSM " << offsetSM << " offsetWR " << offsetWR << " endoffset " << endoffset << " head " << head;
cout << "\n bytesize " << bytesize << " datasize " << datasize << " endoffset " << endoffset;
cout << "\n evmark " << head->evmark << " evsiz " << head->evsiz << " evnum " << head->evnum << dec << " spill " << head->spill << endl;
}
  /*if(offsetWR!=offsetSM && datasize==0){
    exit(1);
    }*/

  *size=0;

  if(internalData->space==myFIFO::FULL){
    return myFIFO::RDLOCKED;
  }

  if(offsetSM>=offsetWR){
if (printdebug) cout << hex << " A (offsetSM >= offsetWR) start " << start << " offsetSM " << offsetSM << " offsetWR " << offsetWR << " endoffset " << endoffset << " head " << head << dec << endl;
    if(endoffset<fifosize){
      memcpy(buf,internalData->startSM[sam_ind]+offsetSM,bytesize);
      *size=datasize;
      internalData->monspace[sam_ind]=myFIFO::FREE;
      internalData->offsetSM[sam_ind]=endoffset;
    }else{
      if((endoffset-fifosize)<=offsetWR){
	memcpy(buf,internalData->startSM[sam_ind]+offsetSM,(fifosize-offsetSM)*sizeof(uint32_t));
	endoffset=endoffset-fifosize;
	memcpy(&buf[datasize-endoffset],internalData->startSM[sam_ind],endoffset*sizeof(uint32_t));
	*size=datasize;
	
	if(endoffset==offsetWR)
	  internalData->monspace[sam_ind]=myFIFO::EMPTY;
	else
	  internalData->monspace[sam_ind]=myFIFO::FREE;
	
	internalData->offsetSM[sam_ind]=endoffset;
      }else{
cout << "FIFO buh  " << hex << start[offsetSM] << dec << " " << datasize << " " << endoffset << " " <<  fifosize << " " << offsetWR << " " << offsetSM << endl;
	return myFIFO::FAILED;
      }
    }
  }else{
if (printdebug) cout << hex << " B (offsetSM < offsetWR) start " << start << " offsetSM " << offsetSM << " offsetWR " << offsetWR << " endoffset " << endoffset << " head " << head << dec << endl;
    if(endoffset<=offsetWR){
      memcpy(buf,internalData->startSM[sam_ind]+offsetSM,bytesize);
      *size=datasize;
      if(endoffset==offsetWR)
	internalData->monspace[sam_ind]=myFIFO::EMPTY;
      else
	internalData->monspace[sam_ind]=myFIFO::FREE;
      internalData->offsetSM[sam_ind]=endoffset;
    }else {
if (printdebug) cout << hex << " C (offsetSM < offsetWR) start " << start << " offsetSM " << offsetSM << " offsetWR " << offsetWR << " endoffset " << endoffset << " head " << head;
cout << "\nFIFO bah  " << start[offsetSM] << dec << " " << datasize << " " << endoffset << " " <<  fifosize << " " << offsetWR << " " << offsetSM << endl;

      if (offsetSM == 0) internalData->offsetSM[sam_ind] = offsetWR;
      return myFIFO::FIFOEMPTY;
      // return myFIFO::FAILED;
    }
  }
  
  return myFIFO::SUCCESS;
}


