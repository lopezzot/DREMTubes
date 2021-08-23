//$Id: myFIFOReader.h,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#include <iostream>
#include <sys/types.h>

#include "myFIFO.h"
 

using namespace std;

class myFIFOReader: public myFIFO{

private:

  struct myFIFOData *internalData;

public:

  myFIFOReader(key_t base_addr):
    myFIFO(base_addr,myFIFO::READER){internalData=myFIFO::getData();};
  
  //~myFIFOWriter();

  result read(unsigned int *buf,unsigned int *size);

  result dumpData(FILE *file);
  
  result dumpData(FILE *file, unsigned int bytes);
 
  //result trylock(){return myFIFO::trylock(myFIFO::READER);};
  
  result waitlock(){return myFIFO::waitlock(myFIFO::READER);};
  
  result unlock(){return myFIFO::unlock(myFIFO::READER);};

  bool isWriterPresent(){return isPresent(myFIFO::WRITER);};

};
