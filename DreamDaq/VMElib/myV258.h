#ifndef _MY_V258_H_
#define _MY_V258_H_

/*****************************************

  myV258.h
  ---------

  Definition of the CAEN discriminator V258 class.
  This class is a class derived from the class vme.

  Version 2014 25.11.2014 roberto.ferrari@pv.infn.it

*****************************************/

#include "myVme.h"

/*****************************************/
// The v258 class 
/*****************************************/
class v258 : public vme {
  
 public:
  
  // Constructor
  v258(uint32_t base, const char* dev);    
  
  //Generic Commands
  void selEnabledChannels(uint16_t mask);
  void enableChannel(uint8_t ch);
  void disableChannel(uint8_t ch);
  void setThreshold(uint8_t ch, uint8_t thre);
  void setThrAll(uint8_t thre);
  uint16_t getManCode();

  void reset();

 protected:

 private:
  uint16_t m_enabled;

};

#endif // _MY_V258_H_
