//$Id: myV258.h,v 1.2 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myV258.h
  ---------

  Definition of the CAEN discriminator V258 class.
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

#define V258_EN (0x30)
#define V258_BASE_TH (0x10)
#define V258_MAN (0xFC)

/*****************************************/
// The v258 class 
/*****************************************/
class v258 : public vme {
  
 public:
  
  // Constructor
  v258(unsigned int base, const char *dev);    
  
  
  //Generic Commands
  inline unsigned int id();                     // Module ID
  
  void enableChannels(unsigned short mask);
  void setThreshold(unsigned char ch, unsigned char thre);
  unsigned short getManCode();

  void reset();

  void  print();



 protected:

 private:

};

unsigned short v258::getManCode(){
  unsigned short val;
  read16phys(V258_MAN,&val);
  return val;
}

void v258::enableChannels(unsigned short mask){
  write16phys(V258_EN,mask);
}
 
void v258::setThreshold(unsigned char ch, unsigned char thre){
  if(ch>15){
    std::cout << "V258 has only 16 channels. Channel " << ch 
	      << " is invalid" << std::endl;
    exit(1);
  }
  
  unsigned char register_off = ch;
  unsigned char shift = 0;
  if(register_off>7){
    register_off-=8;
    shift+=8;
  }
  unsigned short old;  
  read16phys(V258_BASE_TH+(register_off*2),&old);
  old&=(~(0xFF<<shift));
  old|=(thre<<shift);
  write16phys(V258_BASE_TH+(register_off*2),old);
  std::cout << std::hex << V258_BASE_TH+register_off 
	     << " " << old << std::endl;
}

  
/*****************************************/
// Generic Commands
/*****************************************/

inline unsigned int v258::id() { return ba | ID_V258; }

void v258::reset(){
  enableChannels(0);
}

/*****************************************/
// Constructor
/*****************************************/

v258::v258(unsigned int base, const char *dev):vme(base, 0xFF, dev) {reset();};

/*****************************************/
void v258::print() 
/*****************************************/
{
  cout << "************************************" << endl;  
  cout << "  CAEN Discriminator  V258    Module  /  base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "************************************" << endl;
  return;
}

