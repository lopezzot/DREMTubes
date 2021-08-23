
#include <stdint.h>

class updatable{
 public:
  updatable(){};
  uint32_t eventnumber;
  uint32_t dataoffset;
  uint32_t datasize;
  uint32_t * buffer;
  uint32_t partialsize;
  uint32_t * buffer2;
};
