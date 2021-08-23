#include <stdint.h>

#include "myFIFO.h"

class myFIFOReader: public myFIFO{

private:

  struct myFIFOData* internalData;

public:

  myFIFOReader(key_t base_addr):
    myFIFO(base_addr,myFIFO::READER){internalData=myFIFO::getData();};
  
  //~myFIFOWriter();

  result read(uint32_t* buf, uint32_t* size);

  result dumpData(FILE* file);
  
  result dumpData(FILE* file, uint32_t bytes);
 
  //result trylock(){return myFIFO::trylock(myFIFO::READER);};
  
  result waitlock(){return myFIFO::waitlock(myFIFO::READER);};
  
  result unlock(){return myFIFO::unlock(myFIFO::READER);};

  bool isWriterPresent(){return isPresent(myFIFO::WRITER);};

};
