#include <sys/types.h>
#include <iostream>

#include "myFIFO.h"
#include "updatable.h"

using namespace std;

class myFIFOWriter: public myFIFO{

private:

  struct myFIFOData* internalData;

public:

  myFIFOWriter(key_t base_addr):
    myFIFO(base_addr,myFIFO::WRITER){internalData=myFIFO::getData();};
  
  result write(uint32_t* buf);

  result write(uint32_t* buf, updatable* updateinfo);

  //result trylock(){return myFIFO::trylock(myFIFO::WRITER);};
  
  result waitlock(){return myFIFO::waitlock(myFIFO::WRITER);};
  
  result unlock(){return myFIFO::unlock(myFIFO::WRITER);};

  void waitReader(){myFIFO::waitFor(myFIFO::READER);};
  
  bool isReaderPresent(){return myFIFO::isPresent(myFIFO::READER);};

  void updateSamplingPoint();

  double bufferOccupancy();

  void updateOldWrPointer();

  uint32_t getOldWrOffset();

  uint32_t getSize();

  uint32_t getWrOffset();

  uint32_t* getStartPointer();

};
