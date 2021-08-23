//$Id: myV513.h,v 1.3 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myV513.h
  --------

  Definition of the CAEN V513 IO Register.
  This class is a class derived from the class vme.

  A. Cardini / INFN Cagliari
  D. Raspino / Universita' Cagliari & INFN Cagliari

  Version 0.1: 22/1/2003

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
}

// C++ header files
#include <iostream>
using namespace std;

/*****************************************/
// The v513 class 
/*****************************************/
class v513 : public vme {

 public:

  // Constructor
  v513(unsigned int base, const char *dev);    

  // Generic Commands
  inline unsigned int id();                                                    // Returns module ID
  inline void reset();                                                         // Module Reset
  inline void initStatusRegister();                                            // Initialize Status Register
  inline void clearDataRegister();                                             // Clear Data Register
  int setChannelStatusRegister(int channel, unsigned short val);               // Set Channel Statur Register
  unsigned short getChannelStatusRegister(int channel);                        // Get Channel Statur Register
  unsigned short readInputRegister();                                              // Read 16 bit from Input Register
  inline void writeOutputRegister(unsigned short val);                         // Write 16 bit to Output Register
  void setOutputBit(int bit);                                                  // Write 1 in bit 'bit' in Output Register
  void clearOutputBit(int bit);                                                // Write 0 in bit 'bit' in Output Register

  //GenericFunction
  
  void set0(int bit);                                                   // Write 0 in bit 'bit' in Output Register
  void set1(int bit);                                                   // Write 1 in bit 'bit' in Output Register
  void singlePulse(int bit);                                            // Write 1 0 in bit 'bit' in Output Register
  


  // Crap
  void print();                                 // Simple dump function
  
 protected:
  
 private:
  
};

/*****************************************/
// Generic Commands
/*****************************************/
inline unsigned int v513::id()              { return ba | ID_V513; }
inline void v513::reset()                   { write16phys(0x42, 0x0); }
inline void v513::initStatusRegister()      { write16phys(0x46, 0x0); }
inline void v513::clearDataRegister()       { write16phys(0x48, 0x0); }
unsigned short v513::readInputRegister()       { 
  unsigned short rc; 
  read16phys(0x04, &rc);
  volatile unsigned short rcvol = rc;  
  return rcvol;
}
inline void v513::writeOutputRegister(unsigned short val) { write16phys(0x04, val); }


int v513::setChannelStatusRegister(int channel, unsigned short val)
{
  int addr;
  
  if ((channel < 0)||(channel > 15)) {
    cout << "setChannelStatusRegister: cannot set - channel " << channel << " does not exist" << endl;
    return -1;
  }
  addr = 0x10 + 2*channel;
  write16phys(addr, val);
  return 0;
}

unsigned short v513::getChannelStatusRegister(int channel)
{
  int addr;
  unsigned short val;
  
  if ((channel < 0)||(channel > 15)) {
    cout << "getChannelStatusRegister: error - channel " << channel << " does not exist" << endl;
    return 0xFFFF;
  }
  addr = 0x10 + 2*channel;
  read16phys(addr, &val);
  return val&0xF;
}

void v513::setOutputBit(int bit) 
{
  unsigned short val;

  if ((bit < 0)||(bit > 15)) {
    cout << "writeOutputBit: cannot write at bit " << bit << " - only bits from 0 to 15 are allowed" << endl;
    return;
  }
  read16phys(0x04, &val);
  write16phys(0x04, (0x1<<bit)|val);
}

void v513::clearOutputBit(int bit) 
{
  unsigned short val;
  
  if ((bit < 0)||(bit > 15)) {
    cout << "writeOutputBit: cannot write at bit " << bit << " - only bits from 0 to 15 are allowed" << endl;
    return;
  }
  read16phys(0x04, &val);
  write16phys(0x04, val&(~(0x1<<bit)));
}

/*****************************************/
// Generic Functions
/*****************************************/

// Please set in the V513 init setChannelStatusRegister(bit, 0x6) before to use this function;

void v513::set0(int bit) {
  if ((bit < 0)||(bit > 15)) {
    cout << "set0: cannot write at bit " << bit << " - only bits from 0 to 15 are allowed" << endl;
    return;
  }
  setOutputBit(bit);
}



// Please set in the V513 init setChannelStatusRegister(bit, 0x4) before to use this function;

void v513::set1(int bit)        {
  if ((bit < 0)||(bit > 15)) {
    cout << "set1: cannot write at bit " << bit << " - only bits from 0 to 15 are allowed" << endl;
    return;
  }
  clearOutputBit(bit);
}



// Please set in the V513 init setChannelStatusRegister(bit, 0x4) before to use this function;

void v513::singlePulse(int bit) {
  if ((bit < 0)||(bit > 15)) {
    cout << "singlePulse: cannot write at bit " << bit << " - only bits from 0 to 15 are allowed" << endl;
    return;
  }
  set1(bit);
  set0(bit);
}


/*****************************************/
// Constructor
/*****************************************/
v513::v513(unsigned int base, const char *dev):vme(base, 0x1000, dev) {};

/*****************************************/
void v513::print() 
/*****************************************/
{
  cout << "*************************************" << endl;  
  cout << "CAEN I/O Register V513 Module / base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "*************************************" << endl;
  return;
}

/*****************************************/
// V513 Programming Functions
/*****************************************/






















