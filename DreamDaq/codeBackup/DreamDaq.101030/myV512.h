//$Id: myV512.h,v 1.2 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myV512.h
  ---------

  Definition of the CAEN logic unit V512 class.
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

#define V512_FUN (0x10)
#define V512_INT (0x6)

/*****************************************/
// The v512 class 
/*****************************************/
class v512 : public vme {
  
 public:
  
  // Constructor
  v512(unsigned int base, const char *dev);    
  
  
  //Generic Commands
  inline unsigned int id();                     // Module ID
  
  void setFunctionMask(unsigned char func);
  void setVMEVeto();
  void resetVMEVeto();
  void disableInterrupt();

  void reset();

  void  print();



 protected:

 private:

};

void v512::setFunctionMask(unsigned char func){

  unsigned short old;  
  read16phys(V512_FUN,&old);
  old&=0x8;
  old|=(func&0x7);
  write16phys(V512_FUN,old);
}

void v512::setVMEVeto(){

  unsigned short old;  
  read16phys(V512_FUN,&old);
  old|=0x8;
  write16phys(V512_FUN,old);
}
void v512::resetVMEVeto(){
  
  unsigned short old;  
  read16phys(V512_FUN,&old);
  old&=0x7;
  write16phys(V512_FUN,old);
}

void v512::disableInterrupt(){
  write16phys(V512_INT,0);
}

/*****************************************/
// Generic Commands
/*****************************************/

inline unsigned int v512::id() { return ba | ID_V512; }

void v512::reset(){
  resetVMEVeto();
  setFunctionMask(0);
}

/*****************************************/
// Constructor
/*****************************************/

v512::v512(unsigned int base, const char *dev):vme(base, 0xFF, dev) {reset();};

/*****************************************/
void v512::print() 
/*****************************************/
{
  cout << "************************************" << endl;  
  cout << "  CAEN Loci unit V512   Module  /  base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "************************************" << endl;
  return;
}

