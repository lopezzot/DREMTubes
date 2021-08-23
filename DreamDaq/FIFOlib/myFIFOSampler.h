#include <stdint.h>

#include "myFIFO.h"

class myFIFOSampler: public myFIFO{

private:

  struct myFIFOData* internalData;

public:

  myFIFOSampler(key_t base_addr):
    myFIFO(base_addr,myFIFO::SAMPLER){internalData=myFIFO::getData();};
  
  //~myFIFOWriter();

  result read(uint32_t* buf, uint32_t* size);

  bool isWriterPresent(){return isPresent(myFIFO::WRITER);};

  void waitReader(){myFIFO::waitFor(myFIFO::READER);};

  result waitlock(){return myFIFO::waitlock(myFIFO::SAMPLER);};
   
  result unlock(){return myFIFO::unlock(myFIFO::SAMPLER);};

};
