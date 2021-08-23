#include <cmath>
#include <cstring>
#include <iostream>

#include "myFIFOWriter.h"
#include "myFIFO-IOp.h"

using namespace std;

myFIFO::result myFIFOWriter::write(uint32_t* buf, updatable* updateinfo){

  if(internalData->status!=myFIFO::LOCKEDFORWR
     && internalData->status!=myFIFO::UNLOCKED)
    return myFIFO::RDLOCKED;
  
  uint32_t offsetWR=internalData->offsetWR;
  uint32_t offsetRD=internalData->offsetRD;
  uint32_t* offsetSM=internalData->offsetSM;

  if(offsetWR==offsetRD)
    if(internalData->space==myFIFO::FULL)
      return myFIFO::FIFOFULL;	
  

  uint32_t fifosize=internalData->size;
  EventHeader* head =(EventHeader*)buf;
  uint32_t bytesize=head->evsiz;
  uint32_t datasize=bytesize/sizeof(uint32_t);
  uint32_t endoffset=offsetWR+datasize;
  uint32_t nsampler=internalData->nsampler;


  if(offsetWR>=offsetRD){
    if(endoffset<fifosize){
      memcpy(internalData->startWR+offsetWR,buf,bytesize);
      internalData->space=myFIFO::FREE;
 
      updateinfo->buffer=(internalData->startWR)+offsetWR+(updateinfo->dataoffset);
      updateinfo->partialsize=updateinfo->datasize;
      
      for(register uint32_t i=0;i<nsampler;i++){
	if(endoffset>offsetSM[i] && offsetWR<offsetSM[i]){
	  internalData->offsetSM[i]=endoffset;
	  internalData->monspace[i]=myFIFO::EMPTY;
	}else{
	  internalData->monspace[i]=myFIFO::FREE;
	}
      }
      
      internalData->offsetWR=endoffset;
    }else{
      if((endoffset-fifosize)<=offsetRD){

	if((offsetWR+(updateinfo->dataoffset))>=fifosize){
	  updateinfo->buffer=(internalData->startWR)+offsetWR+(updateinfo->dataoffset)-fifosize;
	  updateinfo->partialsize=updateinfo->datasize;
	}else if((offsetWR+(updateinfo->dataoffset)+(updateinfo->datasize))
		 <fifosize){
	  updateinfo->buffer=(internalData->startWR)+offsetWR+(updateinfo->dataoffset);
	  updateinfo->partialsize=updateinfo->datasize;
	}else{
	  updateinfo->buffer=(internalData->startWR)+offsetWR+(updateinfo->dataoffset);
	  updateinfo->buffer2=internalData->startWR;
	  updateinfo->partialsize=fifosize-offsetWR-(updateinfo->dataoffset);
	}

	memcpy(internalData->startWR+offsetWR,buf,
	       (fifosize-offsetWR)*sizeof(uint32_t));
	endoffset=endoffset-fifosize;
	
	memcpy(internalData->startWR,
	       &buf[datasize-endoffset],
	       endoffset*sizeof(uint32_t));
	
	if(endoffset==offsetRD)
	  internalData->space=myFIFO::FULL;
	else
	  internalData->space=myFIFO::FREE;
	
	for(register uint32_t i=0;i<nsampler;i++){
	  if(offsetWR<offsetSM[i] ||
	     (offsetWR>offsetSM[i] && endoffset>offsetSM[i])){
	    internalData->offsetSM[i]=endoffset;
	    internalData->monspace[i]=myFIFO::EMPTY;
	  }else{
	    internalData->monspace[i]=myFIFO::FREE;
	  }
	}

	internalData->offsetWR=endoffset;

	//spezzato

      }else{
        internalData->space=myFIFO::FULL;
	return myFIFO::FIFOFULL;
      }
    }
  }else{
    if(endoffset<=offsetRD){
      memcpy(internalData->startWR+offsetWR,buf,bytesize);

      updateinfo->buffer=(internalData->startWR)+offsetWR+(updateinfo->dataoffset);
      updateinfo->partialsize=updateinfo->datasize;

      if(endoffset==offsetRD)
	internalData->space=myFIFO::FULL;
      else
	internalData->space=myFIFO::FREE;

      for(register uint32_t i=0;i<nsampler;i++){
	if(endoffset>offsetSM[i] && offsetWR<offsetSM[i]){
	  internalData->offsetSM[i]=endoffset;
	  internalData->monspace[i]=myFIFO::EMPTY;
	}else{
	  internalData->monspace[i]=myFIFO::FREE;
	}
      }

      internalData->offsetWR=endoffset;
      
    }else{
      internalData->space=myFIFO::FULL;
      return myFIFO::FIFOFULL;
    }
  }

  return myFIFO::SUCCESS;
}

myFIFO::result myFIFOWriter::write(uint32_t* buf){

  if(internalData->status!=myFIFO::LOCKEDFORWR
     && internalData->status!=myFIFO::UNLOCKED)
    return myFIFO::RDLOCKED;
  
  uint32_t offsetWR=internalData->offsetWR;
  uint32_t offsetRD=internalData->offsetRD;
  uint32_t* offsetSM=internalData->offsetSM;

  if(offsetWR==offsetRD)
    if(internalData->space==myFIFO::FULL)
      return myFIFO::FIFOFULL;	
  
  uint32_t fifosize=internalData->size;
  EventHeader* head = (EventHeader*)buf;
  uint32_t bytesize=head->evsiz;
  uint32_t datasize=bytesize/sizeof(uint32_t);
  uint32_t endoffset=offsetWR+datasize;
  uint32_t nsampler=internalData->nsampler;

  if(offsetWR>=offsetRD){
    if(endoffset<fifosize){
      memcpy(internalData->startWR+offsetWR,buf,bytesize);
      internalData->space=myFIFO::FREE;
      
      for(register uint32_t i=0;i<nsampler;i++){
	if(endoffset>offsetSM[i] && offsetWR<offsetSM[i]){
	  internalData->offsetSM[i]=endoffset;
	  internalData->monspace[i]=myFIFO::EMPTY;
	}else{
	  internalData->monspace[i]=myFIFO::FREE;
	}
      }
      
      internalData->offsetWR=endoffset;
    }else{
      if((endoffset-fifosize)<=offsetRD){
	memcpy(internalData->startWR+offsetWR,buf,
	       (fifosize-offsetWR)*sizeof(uint32_t));
	endoffset=endoffset-fifosize;
	
	memcpy(internalData->startWR,
	       &buf[datasize-endoffset],
	       endoffset*sizeof(uint32_t));
	
	if(endoffset==offsetRD)
	  internalData->space=myFIFO::FULL;
	else
	  internalData->space=myFIFO::FREE;
	
	for(register uint32_t i=0;i<nsampler;i++){
	  if(offsetWR<offsetSM[i] ||
	     (offsetWR>offsetSM[i] && endoffset>offsetSM[i])){
	    internalData->offsetSM[i]=endoffset;
	    internalData->monspace[i]=myFIFO::EMPTY;
	  }else{
	    internalData->monspace[i]=myFIFO::FREE;
	  }
	}

	internalData->offsetWR=endoffset;
      }else{
        internalData->space=myFIFO::FULL;
	return myFIFO::FIFOFULL;
      }
    }
  }else{
    if(endoffset<=offsetRD){
      memcpy(internalData->startWR+offsetWR,buf,bytesize);

      if(endoffset==offsetRD)
	internalData->space=myFIFO::FULL;
      else
	internalData->space=myFIFO::FREE;

      for(register uint32_t i=0;i<nsampler;i++){
	if(endoffset>offsetSM[i] && offsetWR<offsetSM[i]){
	  internalData->offsetSM[i]=endoffset;
	  internalData->monspace[i]=myFIFO::EMPTY;
	}else{
	  internalData->monspace[i]=myFIFO::FREE;
	}
      }

      internalData->offsetWR=endoffset;
    }else{
      internalData->space=myFIFO::FULL;
      return myFIFO::FIFOFULL;
    }
  }
  
  return myFIFO::SUCCESS;
}

double myFIFOWriter::bufferOccupancy(){

  uint32_t offsetWR=internalData->offsetWR;
  uint32_t offsetRD=internalData->offsetRD;
  uint32_t fifosize=internalData->size;

  if(offsetRD==offsetWR){
    if(internalData->space==myFIFO::FULL)
      return 1.0;
    else if(internalData->space==myFIFO::EMPTY)
      return 0.0;
  }

  double diff=1.0*offsetWR-1.0*offsetRD;
  diff/=1.0*fifosize;
  if (diff<0) diff+=1.;

  diff=rint(diff*10000.)/10000.;

  return diff;
}

void myFIFOWriter::updateSamplingPoint(){
  for(register uint32_t i=0;i<(internalData->nsampler);i++){
    internalData->monspace[i]=myFIFO::EMPTY;
    internalData->offsetSM[i]=internalData->offsetWR;
  }
}

void myFIFOWriter::updateOldWrPointer(){
  internalData->oldoffsetWR=internalData->offsetWR;
}

uint32_t myFIFOWriter::getOldWrOffset(){
  return internalData->oldoffsetWR;
}

uint32_t myFIFOWriter::getWrOffset(){
  return internalData->offsetWR;
}

uint32_t myFIFOWriter::getSize(){
  return internalData->size;
}

uint32_t* myFIFOWriter::getStartPointer(){
  return internalData->startWR;
}
