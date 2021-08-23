//$Id: myFIFOReader.cpp,v 1.2 2009/07/21 09:14:45 dreamdaq Exp $
#include <iostream>

#include "myFIFOReader.h"
#include "myFIFO-IOp.h"

using namespace std;

#define DO_NOT_WRITE false

myFIFO::result myFIFOReader::read(uint32_t *buf, uint32_t *size){
 
  if(internalData->status!=myFIFO::LOCKEDFORRD
     && internalData->status!=myFIFO::UNLOCKED)
    return myFIFO::WRLOCKED;

  uint32_t offsetWR=internalData->offsetWR;
  uint32_t offsetRD=internalData->offsetRD;
  
   	 
  if(offsetRD==offsetWR)
    if(internalData->space==myFIFO::EMPTY)  
      return myFIFO::FIFOEMPTY;


  uint32_t fifosize=internalData->size;
  EventHeader * head =(EventHeader *)&(internalData->startRD[offsetRD]);
  uint32_t datasize=head->evsiz;
  uint32_t endoffset=offsetRD+datasize/sizeof(uint32_t);
  
  if(offsetRD>=offsetWR){
    if(endoffset<fifosize){
      memcpy(buf,internalData->startRD+offsetRD,datasize);
      *size=datasize/sizeof(uint32_t);
      internalData->space=myFIFO::FREE;
      internalData->offsetRD=endoffset;
    }else{
      if((endoffset-fifosize)<=offsetWR){
	memcpy(buf,internalData->startRD+offsetRD,(fifosize-offsetRD)*sizeof(uint32_t));
	endoffset=endoffset-fifosize;
	memcpy(&buf[datasize/sizeof(uint32_t)-endoffset],internalData->startRD,endoffset*sizeof(uint32_t));
	*size=datasize/sizeof(uint32_t);
	
	if(endoffset==offsetWR)
	  internalData->space=myFIFO::EMPTY;
	else
	  internalData->space=myFIFO::FREE;
	
	internalData->offsetRD=endoffset;
      }else{
	return myFIFO::FAILED;
      }
    }
  }else{
    if(endoffset<=offsetWR){
      memcpy(buf,internalData->startRD+offsetRD,datasize);
      *size=datasize/sizeof(uint32_t);
      if(endoffset==offsetWR)
	internalData->space=myFIFO::EMPTY;
      else
	internalData->space=myFIFO::FREE;
      internalData->offsetRD=endoffset;
    }else
      return myFIFO::FAILED;
  }
  
  return myFIFO::SUCCESS;
}



myFIFO::result myFIFOReader::dumpData(FILE * file){

  uint32_t offsetWR=internalData->offsetWR;
  uint32_t offsetRD=internalData->offsetRD;
  uint32_t fifosize=internalData->size;
  size_t written=0;

  if(internalData->status==myFIFO::LOCKEDFORWR)
    return myFIFO::WRLOCKED;
	 
  if(offsetRD==offsetWR)
    if(internalData->space==myFIFO::EMPTY)  
      return myFIFO::FIFOEMPTY;

  waitlock();

  if(offsetRD>=offsetWR){
    written=fwrite(internalData->startRD+offsetRD,sizeof(uint32_t),
		   fifosize-offsetRD,file);
    written+=fwrite(internalData->startRD,sizeof(uint32_t),
		    offsetWR,file);
    
    if(written!=(fifosize-offsetRD+offsetWR)){
      unlock();
      return myFIFO::FAILED;
    }

    internalData->space=myFIFO::EMPTY;
    internalData->offsetRD=offsetWR;
  }else{
    written=fwrite(internalData->startRD+offsetRD,sizeof(uint32_t),
		   offsetWR-offsetRD,file);
    if(written!=(offsetWR-offsetRD)){
      unlock();
      return myFIFO::FAILED;
    }
    internalData->space=myFIFO::EMPTY;
    internalData->offsetRD=offsetWR;
  }

  unlock();
  return myFIFO::SUCCESS;
}


myFIFO::result myFIFOReader::dumpData(FILE * file, uint32_t bytes){
  uint32_t offsetWR=internalData->offsetWR;
  uint32_t offsetRD=internalData->offsetRD;
  uint32_t fifosize=internalData->size;
  uint32_t size=bytes/sizeof(uint32_t);
  uint32_t endoffset=offsetRD+size;
  size_t written=0;

  if(internalData->status==myFIFO::LOCKEDFORWR)
    return myFIFO::WRLOCKED;
  
  if(offsetRD==offsetWR)
    if(internalData->space==myFIFO::EMPTY)  
      return myFIFO::FIFOEMPTY;

  if(offsetRD>=offsetWR){
    if(endoffset<fifosize){
      written= (DO_NOT_WRITE) ? size : fwrite(internalData->startRD+offsetRD,
		     sizeof(uint32_t),size,file);
      if(written!=size){
	return myFIFO::FAILED;
      }

      internalData->space=myFIFO::FREE;
      internalData->offsetRD=endoffset;
    }else{
      written= (DO_NOT_WRITE) ? fifosize-offsetRD : fwrite(internalData->startRD+offsetRD,
		     sizeof(uint32_t),fifosize-offsetRD,file);
      
      if((endoffset-fifosize)<offsetWR){
	endoffset-=fifosize;
	written+= (DO_NOT_WRITE) ? endoffset : fwrite(internalData->startRD,
			sizeof(uint32_t),endoffset,file);
	if(written!=size){
	  return myFIFO::FAILED;
	}     
      }else{
	endoffset=offsetWR;
	written+= (DO_NOT_WRITE) ? endoffset : fwrite(internalData->startRD,
			sizeof(uint32_t),endoffset,file);
	if(written!=endoffset+fifosize-offsetRD){
	  return myFIFO::FAILED;
	}
      }
      internalData->space=myFIFO::EMPTY;
      internalData->offsetRD=endoffset;
    }
  }else{
    if(endoffset<offsetWR){
      written= (DO_NOT_WRITE) ? size : fwrite(internalData->startRD+offsetRD,
	     sizeof(uint32_t),size,file);
      
      if(written!=size){
	return myFIFO::FAILED;
      }
      
      internalData->space=myFIFO::FREE;
      internalData->offsetRD=endoffset;
    }else{
      endoffset=offsetWR;
      written= (DO_NOT_WRITE) ? endoffset-offsetRD : fwrite(internalData->startRD+offsetRD,
	     sizeof(uint32_t),endoffset-offsetRD,file);
      if(written!=endoffset-offsetRD){
	return myFIFO::FAILED;
      }
      internalData->space=myFIFO::EMPTY;
      internalData->offsetRD=endoffset;
    }
  }

  return myFIFO::SUCCESS;
}
