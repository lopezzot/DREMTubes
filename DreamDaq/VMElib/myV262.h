#ifndef _MY_V262_H_
#define _MY_V262_H_

/*****************************************

  myV488.h
  ---------

  Definition of the CAEN I/O REG V262 class.
  This class is a class derived from the class vme.


*****************************************/

#include <iostream>

#include "myVme.h"

#define V262_VER (0xFE)
#define V262_MAN (0xFC)
#define V262_FIX (0xFA)

#define V262_CHECKNIMCH(CH) do { \
if(CH>3){ \
std::cout << "Channel " << CH \
<< " does not exist" << std::endl; \
exit(1); \
} \
} while(0)

#define V262_CHECKNIMMASK(MASK) do { \
if(MASK>0xF){ \
std::cout << "Mask " << MASK \
<< " is not valid" << std::endl; \
exit(1); \
} \
} while(0)

#define V262_SETBIT(OFF,CH,MASK) do { \
MASK=MASK | (1<<CH); \
write16phys(OFF,MASK); \
} while(0)

#define V262_UNSETBIT(OFF,CH,MASK) do { \
MASK=MASK & ~(1<<CH); \
write16phys(OFF,MASK); \
} while(0)

#define V262_GETBIT(OFF,CH) do { \
uint16_t val; \
read16phys(OFF,&val); \
volatile uint16_t vol; \
vol=val & (1<<CH); \
return (vol >> CH); \
} while(0)

#define V262_ECL (0x4)
#define V262_NIM_LEVEL (0x6)
#define V262_NIM_PULSE (0x8)
#define V262_NIM_IN (0xA)

/*****************************************/
// The v262 class 
/*****************************************/
class v262 : public vme {
  
 public:
  
  // Constructor
  v262(uint32_t base, const char* dev);
  
  //Generic Commands
  inline void setECLOuts(uint16_t mask)
   { write16phys(V262_ECL,mask); ECLOutMask=mask; }

  inline void enableECLOut(uint16_t ch)
   { V262_SETBIT(V262_ECL,ch,ECLOutMask); }

  inline void disableECLOut(uint16_t ch)
   { V262_UNSETBIT(V262_ECL,ch,ECLOutMask); }

  inline void setNIMOuts(uint16_t mask)
   { write16phys(V262_NIM_LEVEL,mask); NIMOutMask=mask; }

  inline void enableNIMOut(uint16_t ch)
   { V262_CHECKNIMCH(ch); V262_SETBIT(V262_NIM_LEVEL,ch,NIMOutMask); }

  inline void disableNIMOut(uint16_t ch)
   { V262_CHECKNIMCH(ch); V262_UNSETBIT(V262_NIM_LEVEL,ch,NIMOutMask); }

  inline void NIMPulse(uint16_t ch)
   { V262_CHECKNIMCH(ch); write16phys(V262_NIM_PULSE,1<<ch); }

  inline void NIMPulseS(uint16_t mask)
   { V262_CHECKNIMMASK(mask); write16phys(V262_NIM_PULSE,mask); }

  inline uint16_t getNIMIn(uint32_t ch)
   { V262_CHECKNIMCH(ch); V262_GETBIT(V262_NIM_IN,ch); }

  inline uint16_t getVersion();
  inline uint16_t getManCode();
  inline uint16_t getFixed();

  void reset();

 protected:

 private:

  uint16_t NIMOutMask;
  uint16_t ECLOutMask;

};

inline uint16_t v262::getVersion(){
  uint16_t val;
  read16phys(V262_VER,&val);
  return val;
}

inline uint16_t v262::getManCode(){
  uint16_t val;
  read16phys(V262_MAN,&val);
  return val;
}

inline uint16_t v262::getFixed(){
  uint16_t val;
  read16phys(V262_FIX,&val);
  return val;
}

#endif // _MY_V262_H_
