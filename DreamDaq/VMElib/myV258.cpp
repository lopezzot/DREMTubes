#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <cerrno>
#include <iostream>

using namespace std;

#include "myV258.h"
#include "myModules.h"

#define V258_EN (0x30)
#define V258_BASE_TH (0x10)
#define V258_VER (0xFE)
#define V258_MAN (0xFC)
#define V258_FIX (0xFA)

uint16_t v258::getManCode(){
  uint16_t val;
  read16phys(V258_MAN,&val);
  return val;
}

void v258::selEnabledChannels(uint16_t mask){
  m_enabled = mask;
  write16phys(V258_EN,m_enabled);
}
 
void v258::enableChannel(uint8_t ch){
  m_enabled |= 1<<ch;
  write16phys(V258_EN,m_enabled);
}
 
void v258::disableChannel(uint8_t ch){
  m_enabled &= ~(1<<ch);
  write16phys(V258_EN,m_enabled);
}
 
void v258::setThreshold(uint8_t ch, uint8_t thre){
  if(ch>15){
    cout << "V258 has only 16 channels. Channel " << ch << " is invalid" << endl;
    exit(1);
  }

  uint8_t register_off = ch;
  uint8_t shift = 0;
  if(register_off>7){
    register_off-=8;
    shift+=8;
  }
  uint16_t old, oldt, newt;  
  read16phys(V258_BASE_TH+(register_off*2),&old);
  oldt = (old>>shift) & 0xFF;
  if (oldt == thre) return;
  old &= ~(0xFF<<shift);
  newt = old | (thre<<shift);
  write16phys(V258_BASE_TH+(register_off*2),newt);
  if (0)
    cout << hex << this->id() << " " << int16_t(ch) << " "
         << V258_BASE_TH+register_off << " " << old << dec << endl;
  m_enabled |= 1<<ch;
  write16phys(V258_EN,m_enabled);
  cout << hex << this->id() << " chan " << dec << int16_t(ch)
       << " enabled and threshold changed ==> old: "
       << oldt*2 << " new: " << thre*2 << " (mV)" << endl;
}

void v258::setThrAll(uint8_t thre) {
  for (uint8_t ch = 0; ch < 16; ch ++) {
    this->setThreshold(ch,thre);
  }
  m_enabled = 0xffff;
  write16phys(V258_EN,m_enabled);
}

  
void v258::reset(){
  m_enabled = 0;
  write16phys(V258_EN,m_enabled);
}

/*****************************************/
// Constructor
/*****************************************/

v258::v258(uint32_t base, const char* dev):vme(base, 0xFF, dev)
 {
  m_id = base | ID_V258;
  m_name = "CAEN V258 Discriminator";
  reset();
 }

