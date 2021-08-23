//$Id: myFIFOSampler.cpp,v 1.2 2006/10/30 02:08:33 cvsdream Exp $
#include <iostream>

#include "myFIFOSampler.h"
#include "myFIFO-IOp.h"

using namespace std;

myFIFO::result myFIFOSampler::read(unsigned int *buf, unsigned int *size){
 
  if(internalData->status!=myFIFO::LOCKEDFORSM
     && internalData->status!=myFIFO::UNLOCKED){
    return myFIFO::WRLOCKED;
  }

  if(internalData->status==myFIFO::LOCKEDFORRD){
    return myFIFO::RDLOCKED;
  }

  unsigned int offsetWR=internalData->offsetWR;
  unsigned int offsetSM=internalData->offsetSM[sam_ind];
  
  if(offsetSM==offsetWR)
    if(internalData->monspace[sam_ind]==myFIFO::EMPTY) {
      return myFIFO::FIFOEMPTY;
    }

  unsigned int fifosize=internalData->size;
  unsigned int *start =internalData->startSM[sam_ind];
  EventHeader * head =(EventHeader *) &(start[offsetSM]);
// std::cout << hex << sam_ind << " start " << start << " offsetSM " << offsetSM << " offsetWR " << offsetWR << " head " << head << dec << endl;
  
/*  unsigned int evmark;           // Beginning of event marker - 0xcafecafe
  unsigned int evhsiz;           // Event Header size in bytes
  unsigned int evsiz;            // Event size in bytes (including header)
  unsigned int evnum;            // Event number
  unsigned int spill;            // Spill number
  unsigned int tsec;             // Seconds in the day from gettimeofday
  unsigned int tusec;            // Microseconds in the day from gettimeofday
*/

  unsigned int bytesize=head->evsiz;
  unsigned int datasize=bytesize/sizeof(unsigned int);
  unsigned int endoffset=offsetSM+datasize;

// std::cout << hex << " 0 (offsetSM >= offsetWR) start " << start << " offsetSM " << offsetSM << " offsetWR " << offsetWR << " endoffset " << endoffset << " head " << head << dec << endl;
// std::cout << hex << " bytesize " << bytesize << " datasize " << datasize << " endoffset " << endoffset << dec << endl;
// std::cout << hex << " evmark " << head->evmark << " evsiz " << head->evsiz << " evnum " << head->evnum << dec << endl;
  /*if(offsetWR!=offsetSM && datasize==0){
    exit(1);
    }*/

  *size=0;

  if(internalData->space==myFIFO::FULL){
    return myFIFO::RDLOCKED;
  }

  if(offsetSM>=offsetWR){
// std::cout << hex << " A (offsetSM >= offsetWR) start " << start << " offsetSM " << offsetSM << " offsetWR " << offsetWR << " endoffset " << endoffset << " head " << head << dec << endl;
    if(endoffset<fifosize){
      memcpy(buf,internalData->startSM[sam_ind]+offsetSM,bytesize);
      *size=datasize;
      internalData->monspace[sam_ind]=myFIFO::FREE;
      internalData->offsetSM[sam_ind]=endoffset;
    }else{
      if((endoffset-fifosize)<=offsetWR){
	memcpy(buf,internalData->startSM[sam_ind]+offsetSM,(fifosize-offsetSM)*sizeof(unsigned int));
	endoffset=endoffset-fifosize;
	memcpy(&buf[datasize-endoffset],internalData->startSM[sam_ind],endoffset*sizeof(unsigned int));
	*size=datasize;
	
	if(endoffset==offsetWR)
	  internalData->monspace[sam_ind]=myFIFO::EMPTY;
	else
	  internalData->monspace[sam_ind]=myFIFO::FREE;
	
	internalData->offsetSM[sam_ind]=endoffset;
      }else{
cout << "FIFO buh  " << std::hex << start[offsetSM] << std::dec << " " << datasize << " " << endoffset << " " <<  fifosize << " " << offsetWR << " " << offsetSM << endl;
	return myFIFO::FAILED;
      }
    }
  }else{
// std::cout << hex << " B (offsetSM < offsetWR) start " << start << " offsetSM " << offsetSM << " offsetWR " << offsetWR << " endoffset " << endoffset << " head " << head << dec << endl;
    if(endoffset<=offsetWR){
      memcpy(buf,internalData->startSM[sam_ind]+offsetSM,bytesize);
      *size=datasize;
      if(endoffset==offsetWR)
	internalData->monspace[sam_ind]=myFIFO::EMPTY;
      else
	internalData->monspace[sam_ind]=myFIFO::FREE;
      internalData->offsetSM[sam_ind]=endoffset;
    }else {
// std::cout << hex << " C (offsetSM < offsetWR) start " << start << " offsetSM " << offsetSM << " offsetWR " << offsetWR << " endoffset " << endoffset << " head " << head << dec << endl;
cout << "FIFO bah  " << std::hex << start[offsetSM] << std::dec << " " << datasize << " " << endoffset << " " <<  fifosize << " " << offsetWR << " " << offsetSM << endl;
      return myFIFO::FAILED;
    }
  }
  
  return myFIFO::SUCCESS;
}


