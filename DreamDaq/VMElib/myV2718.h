#ifndef _MY_V2718_H_
#define _MY_V2718_H_

#include <stdint.h>

#ifndef LINUX
#define LINUX
#endif

#include "myVme.h"

/*****************************************/
// The v2718 class
/*****************************************/
class v2718 : public vme {

 public:

  v2718(uint32_t base, const char* dev);
  ~v2718() {}

  uint32_t setPulserA( uint64_t period,  uint64_t width);
  uint32_t setPulserB( uint64_t period,  uint64_t width);
  uint32_t resetPulserA();
  uint32_t resetPulserB();

 protected:

 private:
};

#endif // _MY_V2718_H_
