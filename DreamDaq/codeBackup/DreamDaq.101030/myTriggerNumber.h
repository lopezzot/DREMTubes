//$Id: myTriggerNumber.h,v 1.5 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myV260.h
  --------

  Definition of the CAEN V260 Scaler.
  This class is a class derived from the class vme.

  A. Cardini / INFN Cagliari

  Version 0.1: 28/5/2001

*****************************************/

// Standard C header files
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
}

#define TTNUMBER_VERSION (0x4)
#define TTNUMBER_NUMBER (0x8)
#define TTNUMBER_COUNTER (0xC)

// C++ header files
#include <iostream>
using namespace std;

/*****************************************/
// The vTriggerNumber class 
/*****************************************/
class vTriggerNumber : public vme {

 public:

  // Constructor
  vTriggerNumber(unsigned int base, const char *dev);    

  inline unsigned int version();
  inline unsigned int counter();
  inline unsigned int triggernumber();

  inline void reset();

  inline unsigned int id();                     // Module ID

  int getInfo(unsigned *evbuff);                // Get info

  // Crap
  void print();                                 // Simple dump function

 protected:

 private:

};

/*****************************************/
// Generic Commands
/*****************************************/
inline unsigned int vTriggerNumber::version(){
  unsigned int version;
  read32phys(TTNUMBER_VERSION, &version);
  return version;
}

inline unsigned int vTriggerNumber::counter(){
  unsigned int value;
  read32phys(TTNUMBER_COUNTER, &value);
  volatile unsigned int lvalue = value;
  return lvalue;
}

inline unsigned int vTriggerNumber::triggernumber(){
  unsigned int value;
  read32phys(TTNUMBER_NUMBER, &value);
  volatile unsigned int lvalue = value;
  return lvalue;
}

inline void vTriggerNumber::reset(){
  write32phys(TTNUMBER_COUNTER, 0x0);
}

inline unsigned int vTriggerNumber::id() { return ba | ID_TTNUMBER; }

int vTriggerNumber::getInfo(unsigned *evbuff)
{
  unsigned int data;
  int sz=0;

  read32phys(TTNUMBER_NUMBER, &data);
  *evbuff++ = data;
  sz++;
  read32phys(TTNUMBER_COUNTER, &data);
  *evbuff++ = data;
  sz++;

  return sz;
}

/*****************************************/
// Constructor
/*****************************************/
vTriggerNumber::vTriggerNumber(unsigned int base, const char *dev):vme(base, 0x100, dev) {};

/*****************************************/
void vTriggerNumber::print() 
/*****************************************/
{
  cout << "*************************************" << endl;  
  cout << "Trigger Number Module / base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "*************************************" << endl;
  return;
}

/*****************************************/
// V260 Programming Functions
/*****************************************/

