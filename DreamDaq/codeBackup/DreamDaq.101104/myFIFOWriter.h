//$Id: myFIFOWriter.h,v 1.5 2007/06/02 15:44:35 cvsdream Exp $
#include <iostream>
#include <sys/types.h>

#include "myFIFO.h"
#include "updatable.h"

using namespace std;

class myFIFOWriter: public myFIFO{

private:

  struct myFIFOData *internalData;

public:

  myFIFOWriter(key_t base_addr):
    myFIFO(base_addr,myFIFO::WRITER){internalData=myFIFO::getData();};
  
  //~myFIFOWriter();

  result write(unsigned int *buf);

  result write(unsigned int *buf,updatable * updateinfo);

  //result trylock(){return myFIFO::trylock(myFIFO::WRITER);};
  
  result waitlock(){return myFIFO::waitlock(myFIFO::WRITER);};
  
  result unlock(){return myFIFO::unlock(myFIFO::WRITER);};

  void waitReader(){myFIFO::waitFor(myFIFO::READER);};
  
  bool isReaderPresent(){return myFIFO::isPresent(myFIFO::READER);};

  void updateSamplingPoint();

  double bufferOccupancy();

  void updateOldWrPointer();

  unsigned int getOldWrOffset();

  unsigned int getSize();

  unsigned int getWrOffset();

  unsigned int * getStartPointer();

};
