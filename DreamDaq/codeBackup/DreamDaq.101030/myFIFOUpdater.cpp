//$Id: myFIFOUpdater.cpp,v 1.2 2007/06/01 17:13:14 cvsdream Exp $

#include "myFIFOUpdater.h"
#include "myFIFO-IOp.h"
#include <iostream>

using namespace std;

myFIFOUpdater::myFIFOUpdater(myFIFOWriter * fifo, unsigned int aId){
  fifostart=fifo->getStartPointer();
  fifosize=fifo->getSize();
  initialUpOff=fifo->getOldWrOffset();
  currentUpOff=initialUpOff;
  wrOffset=fifo->getWrOffset();
  id=aId;
  partial=false;
  remaining=0;
}
  
unsigned int * myFIFOUpdater::next(unsigned int * evnumber,unsigned int *size,
				   bool * part){
  
  unsigned int i=0;
  unsigned int start=currentUpOff;
  SubEventHeader * sub=0;

  if(partial){
    *size=remaining;
    partial = false;
    return fifostart;
  }else{
    if(start==wrOffset)
      return 0;

    if(fifostart[start]!=EVMARK)
      return 0;
    
    if(fifosize-start<sizeof(EventHeader))
      

    EventHeader * evhead = (EventHeader *)(fifostart+currentUpOff);
    

    i+=sizeof(EventHeader)/sizeof(unsigned int);
    
    while(i<(evhead->evsiz/sizeof(unsigned int))){
      
      sub=(SubEventHeader *)(fifostart+currentUpOff+i);
      
      if(sub->id==id)
	break;
      
      i+=(sub->size/sizeof(unsigned int));
    }
    
    currentUpOff+=(evhead->evsiz/sizeof(unsigned int));
    if(currentUpOff>=fifosize)
      currentUpOff -=fifosize;
    
    if(i<(evhead->evsiz/sizeof(unsigned int))){
      *evnumber=evhead->evnum;
      *size=(sub->size-sizeof(SubEventHeader))/sizeof(unsigned int);
      unsigned int offset =i+(sizeof(SubEventHeader)/sizeof(unsigned int));
      
      if(offset+(*size)>=fifosize){
	cout << "partial" << endl;
	partial = true;
	remaining=*size-fifosize+1;
	*size=*size-remaining;
      }
      
      return ((unsigned int *)evhead)+offset;
      
    }else
      return 0;
  }
}
