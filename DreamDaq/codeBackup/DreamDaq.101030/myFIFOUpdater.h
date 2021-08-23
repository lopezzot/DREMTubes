//$Id: myFIFOUpdater.h,v 1.2 2007/06/01 17:13:14 cvsdream Exp $
#include <iostream>
#include <sys/types.h>

#include "myFIFOWriter.h"


using namespace std;

class myFIFOUpdater{

public:
  myFIFOUpdater(myFIFOWriter * fifo, unsigned int aId);
  ~myFIFOUpdater(){};
  
  unsigned int * next(unsigned int *evnumber, 
		      unsigned int * size,
		      bool * part);
  
private:
  unsigned int * fifostart;
  unsigned int fifosize;
  unsigned int initialUpOff;
  unsigned int currentUpOff;
  unsigned int wrOffset;
  unsigned int id;
  bool partial;
  unsigned int remaining;
};
