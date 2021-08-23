//$Id: myFIFOSampler.h,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#include <iostream>
#include <sys/types.h>

#include "myFIFO.h"
 

using namespace std;

class myFIFOSampler: public myFIFO{

private:

  struct myFIFOData *internalData;

public:

  myFIFOSampler(key_t base_addr):
    myFIFO(base_addr,myFIFO::SAMPLER){internalData=myFIFO::getData();};
  
  //~myFIFOWriter();

  result read(unsigned int *buf,unsigned int *size);

  bool isWriterPresent(){return isPresent(myFIFO::WRITER);};

  void waitReader(){myFIFO::waitFor(myFIFO::READER);};

  result waitlock(){return myFIFO::waitlock(myFIFO::SAMPLER);};
   
  result unlock(){return myFIFO::unlock(myFIFO::SAMPLER);};

};
