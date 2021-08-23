//$Id: myV262.h,v 1.2 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myV488.h
  ---------

  Definition of the CAEN I/O REG V262 class.
  This class is a class derived from the class vme.


*****************************************/

// Standard C header files
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include "myModules.h"
#include <sys/time.h>
}
// C++ header files
#include <iostream>
using namespace std;

#define V262_CHECKNIMCH(CH){ \
if(CH>3){ \
cout << "Channel " << CH \
<< " does not exist" << endl; \
exit(1); \
} \
}

#define V262_CHECKNIMMASK(MASK){ \
if(MASK>0xF){ \
cout << "Mask " << MASK \
<< " is not valid" << endl; \
exit(1); \
} \
}


#define V262_SETBIT(OFF,CH,MASK){ \
MASK=MASK | (1<<CH); \
write16phys(OFF,MASK); \
}

#define V262_UNSETBIT(OFF,CH,MASK){ \
MASK=MASK & ~(1<<CH); \
write16phys(OFF,MASK); \
}

#define V262_GETBIT(OFF,CH){ \
unsigned short val; \
read16phys(OFF,&val); \
volatile unsigned short vol; \
vol=val & (1<<CH); \
return (vol >> CH); \
}

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
  v262(unsigned int base, const char *dev);    
  
  
  //Generic Commands
  inline unsigned int id();                     // Module ID
  
  void setECLOuts(unsigned short mask);
  void enableECLOut(unsigned short ch);
  void disableECLOut(unsigned short ch);
  void setNIMOuts(unsigned short mask);
  void enableNIMOut(unsigned short ch);
  void disableNIMOut(unsigned short ch);
  void NIMPulse(unsigned short ch);
  void NIMPulseS(unsigned short mask);
  unsigned short getNIMIn(unsigned int ch);

  void reset();

  void  print();



 protected:

 private:

  unsigned short NIMOutMask;
  unsigned short ECLOutMask;

};

void v262::setECLOuts(unsigned short mask){
  write16phys(V262_ECL,mask);
  ECLOutMask=mask;
}

void v262::enableECLOut(unsigned short ch){
  V262_SETBIT(V262_ECL,ch,ECLOutMask);
}

void v262::disableECLOut(unsigned short ch){
  V262_UNSETBIT(V262_ECL,ch,ECLOutMask);
}

void v262::setNIMOuts(unsigned short mask){
  write16phys(V262_NIM_LEVEL,mask);
  NIMOutMask=mask;
}

void v262::enableNIMOut(unsigned short ch){
  V262_CHECKNIMCH(ch);
  V262_SETBIT(V262_NIM_LEVEL,ch,NIMOutMask);
}

void v262::disableNIMOut(unsigned short ch){
  V262_CHECKNIMCH(ch);
  V262_UNSETBIT(V262_NIM_LEVEL,ch,NIMOutMask);
}

void v262::NIMPulse(unsigned short ch){
  V262_CHECKNIMCH(ch);
  write16phys(V262_NIM_PULSE,1<<ch);
}

inline void v262::NIMPulseS(unsigned short mask){
  V262_CHECKNIMMASK(mask);
  write16phys(V262_NIM_PULSE,mask);
}

inline unsigned short v262::getNIMIn(unsigned int ch){
  V262_CHECKNIMCH(ch);
  V262_GETBIT(V262_NIM_IN,ch);
}

  
/*****************************************/
// Generic Commands
/*****************************************/

inline unsigned int v262::id() { return ba | ID_V262; }

void v262::reset(){
  NIMOutMask=0;
  ECLOutMask=0;
  write16phys(V262_ECL,0x0);
  write16phys(V262_NIM_PULSE,0x0);
  write16phys(V262_NIM_LEVEL,0x0);
}

/*****************************************/
// Constructor
/*****************************************/

v262::v262(unsigned int base, const char *dev):vme(base, 0xFF, dev) {reset();};

/*****************************************/
void v262::print() 
/*****************************************/
{
  cout << "************************************" << endl;  
  cout << "  CAEN  I/O REF  V262    Module  /  base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "************************************" << endl;
  return;
}

