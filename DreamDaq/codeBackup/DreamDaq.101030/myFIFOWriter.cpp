//$Id: myFIFOWriter.cpp,v 1.9 2009/07/21 09:14:45 dreamdaq Exp $

extern "C" {
#include <math.h>
}

#include <iostream>

#include "myFIFOWriter.h"
#include "myFIFO-IOp.h"

using namespace std;

myFIFO::result myFIFOWriter::write(unsigned int *buf,updatable * updateinfo){

  if(internalData->status!=myFIFO::LOCKEDFORWR
     && internalData->status!=myFIFO::UNLOCKED)
    return myFIFO::RDLOCKED;
  
  unsigned int offsetWR=internalData->offsetWR;
  unsigned int offsetRD=internalData->offsetRD;
  unsigned int * offsetSM=internalData->offsetSM;

  //cout << "offsetSM " << offsetSM << endl;

  if(offsetWR==offsetRD)
    if(internalData->space==myFIFO::FULL)
      return myFIFO::FIFOFULL;	
  

  unsigned int fifosize=internalData->size;
  EventHeader * head =(EventHeader *)buf;
  unsigned int bytesize=head->evsiz;
  unsigned int datasize=bytesize/sizeof(unsigned int);
  unsigned int endoffset=offsetWR+datasize;
  unsigned int nsampler=internalData->nsampler;


  if(offsetWR>=offsetRD){
    if(endoffset<fifosize){
      memcpy(internalData->startWR+offsetWR,buf,bytesize);
      internalData->space=myFIFO::FREE;
 
      updateinfo->buffer=(internalData->startWR)+offsetWR+(updateinfo->dataoffset);
      updateinfo->partialsize=updateinfo->datasize;
      
      for(register unsigned int i=0;i<nsampler;i++){
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
	       (fifosize-offsetWR)*sizeof(unsigned int));
	endoffset=endoffset-fifosize;
	
	memcpy(internalData->startWR,
	       &buf[datasize-endoffset],
	       endoffset*sizeof(unsigned int));
	
	if(endoffset==offsetRD)
	  internalData->space=myFIFO::FULL;
	else
	  internalData->space=myFIFO::FREE;
	
	for(register unsigned int i=0;i<nsampler;i++){
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

      for(register unsigned int i=0;i<nsampler;i++){
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
  
  //cout << "endoffset " <<endoffset << endl;
  
  return myFIFO::SUCCESS;
}

myFIFO::result myFIFOWriter::write(unsigned int *buf){

  if(internalData->status!=myFIFO::LOCKEDFORWR
     && internalData->status!=myFIFO::UNLOCKED)
    return myFIFO::RDLOCKED;
  
  unsigned int offsetWR=internalData->offsetWR;
  unsigned int offsetRD=internalData->offsetRD;
  unsigned int * offsetSM=internalData->offsetSM;

  //cout << "offsetSM " << offsetSM << endl;

  if(offsetWR==offsetRD)
    if(internalData->space==myFIFO::FULL)
      return myFIFO::FIFOFULL;	
  

  unsigned int fifosize=internalData->size;
  EventHeader * head =(EventHeader *)buf;
  unsigned int bytesize=head->evsiz;
  unsigned int datasize=bytesize/sizeof(unsigned int);
  unsigned int endoffset=offsetWR+datasize;
  unsigned int nsampler=internalData->nsampler;


  if(offsetWR>=offsetRD){
    if(endoffset<fifosize){
      memcpy(internalData->startWR+offsetWR,buf,bytesize);
      internalData->space=myFIFO::FREE;
      
      for(register unsigned int i=0;i<nsampler;i++){
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
	       (fifosize-offsetWR)*sizeof(unsigned int));
	endoffset=endoffset-fifosize;
	
	memcpy(internalData->startWR,
	       &buf[datasize-endoffset],
	       endoffset*sizeof(unsigned int));
	
	if(endoffset==offsetRD)
	  internalData->space=myFIFO::FULL;
	else
	  internalData->space=myFIFO::FREE;
	
	for(register unsigned int i=0;i<nsampler;i++){
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

      for(register unsigned int i=0;i<nsampler;i++){
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
  
  //cout << "endoffset " <<endoffset << endl;
  
  return myFIFO::SUCCESS;
}

double myFIFOWriter::bufferOccupancy(){

  unsigned int offsetWR=internalData->offsetWR;
  unsigned int offsetRD=internalData->offsetRD;
  unsigned int fifosize=internalData->size;

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
  for(register unsigned int i=0;i<(internalData->nsampler);i++){
    internalData->monspace[i]=myFIFO::EMPTY;
    internalData->offsetSM[i]=internalData->offsetWR;
  }
}

void myFIFOWriter::updateOldWrPointer(){
  internalData->oldoffsetWR=internalData->offsetWR;
}

unsigned int myFIFOWriter::getOldWrOffset(){
  return internalData->oldoffsetWR;
}

unsigned int myFIFOWriter::getWrOffset(){
  return internalData->offsetWR;
}

unsigned int myFIFOWriter::getSize(){
  return internalData->size;
}

unsigned int * myFIFOWriter::getStartPointer(){
  return internalData->startWR;
}
