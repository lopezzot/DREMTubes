#ifndef _MY_V814_H_
#define _MY_V814_H_

/*****************************************

  myV814.h
  ---------

  Definition of the CAEN discriminator V814 class.
  This class is a class derived from the class vme.

*****************************************/

#include <iostream>

#include "myVme.h"

#define V814_EN (0x4A)
#define V814_W0 (0x40)
#define V814_W1 (0x42)
#define V814_BASE_TH (0x00)
#define V814_VER (0xFE)
#define V814_MAN (0xFC)
#define V814_FIX (0xFA)

/*****************************************/
// The v814 class 
/*****************************************/

class v814 : public vme {
 public:

  // Constructor
  v814(uint32_t base, const char* dev);    

  //Generic Commands

  void selEnabledChannels(uint16_t mask);
  void enableChannel(uint8_t ch);
  void disableChannel(uint8_t ch);
  void setThreshold(uint8_t ch, uint8_t thre);
  void setThrAll(uint8_t thre);
  void setWidth(uint8_t ch, uint8_t thre);
  void setWidthAll(uint8_t thre);
  uint16_t getVersion();
  uint16_t getManCode();
  uint16_t getFixed();

  void reset();

 protected:

 private:
  uint16_t m_enabled;

};

inline uint16_t v814::getVersion(){
  uint16_t val;
  read16phys(V814_VER,&val);
  return val;
}

inline uint16_t v814::getManCode(){
  uint16_t val;
  read16phys(V814_MAN,&val);
  return val;
}

inline uint16_t v814::getFixed(){
  uint16_t val;
  read16phys(V814_FIX,&val);
  return val;
}

inline void v814::selEnabledChannels(uint16_t mask){
  m_enabled = mask;
  write16phys(V814_EN,m_enabled);
}
 
inline void v814::enableChannel(uint8_t ch){
  m_enabled |= 1<<ch;
  write16phys(V814_EN,m_enabled);
}
 
inline void v814::disableChannel(uint8_t ch){
  m_enabled &= ~(1<<ch);
  write16phys(V814_EN,m_enabled);
}
 
inline void v814::setThreshold(uint8_t ch, uint8_t thre){
  write16phys(V814_BASE_TH+(ch*2),thre);
  m_enabled |= 1<<ch;
  write16phys(V814_EN,m_enabled);
  std::cout << std::hex << "v814 id 0x" << m_id << " chan " << std::dec
            << uint16_t(ch) << " enabled and threshold changed to: "
            << uint16_t(thre) << " (mV)" << std::endl;
}

inline void v814::setThrAll(uint8_t thre) {
  for (uint8_t ch = 0; ch < 16; ch ++) {
    this->setThreshold(ch,thre);
  }
  m_enabled = 0xffff;
  write16phys(V814_EN,m_enabled);
}

inline void v814::setWidth(uint8_t ch, uint8_t width) {
  if (ch < 8) 
    write16phys(V814_W0,width);
  else if (ch < 16)
    write16phys(V814_W1,width);
}

inline void v814::setWidthAll(uint8_t width) {
  for (uint8_t ch = 0; ch < 16; ch ++) {
    this->setWidth(ch,width);
  }
  m_enabled = 0xffff;
  write16phys(V814_EN,m_enabled);
}

inline void v814::reset(){
  m_enabled = 0;
  write16phys(V814_EN,m_enabled);
}

#endif // _MY_V814_H_
