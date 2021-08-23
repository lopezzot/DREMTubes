//$Id: myL1182.h,v 1.5 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myL1182.h
  ---------

  Definition of the LeCroy 1182 Charge ADC.
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

// C++ header files
#include <iostream>
using namespace std;

#define EVTBASEADD 0x100

/*****************************************/
// The l1182 class 
/*****************************************/
class l1182 : public vme {

 public:

  // Constructor
  l1182(unsigned int base, const char *dev);    

  // Generic Commands
  int readSingleEvent(unsigned int * evbuff);   // Read event from hardware and stores it in user-provided buffer
  int readSingleEventMasked(unsigned int * evbuff, unsigned int mask);   // Read event from hardware and stores it in user-provided buffer
  int readSingleEvent(unsigned int * evbuff, unsigned int ncha);   // Read event from hardware and stores it in user-provided buffer
  int readEvent(unsigned int * evbuff);         // Read event from hardware and stores it in user-provided buffer
  int readEventMasked(unsigned int * evbuff, unsigned int mask);   // Read event from hardware and stores it in user-provided buffer

  inline int conversionComplete();              // Checks if conversion is complete
  inline int conversionInProgress();            // Checks if conversion is in progress
  inline int eventBufferNotFull();              // Checks if event buffer is not full
  inline int eventCounter();                    // Reads event counter
  inline void selectFrontPanelGate();           // Selects front panel gate
  inline void testGate();                       // Generates a test gate of 500ns
  inline void clear();                          // Clears module, conversionComplete and eventFull are cleared and reset
  inline unsigned int id();                     // Module ID

  // Crap
  void print();                                 // Simple dump function

 protected:

 private:

};

/*****************************************/
// Generic Commands
/*****************************************/
inline int l1182::conversionComplete()     { unsigned short i; read16phys(0x0, &i); return i&0x1; }
inline int l1182::conversionInProgress()   { unsigned short i; read16phys(0x0, &i); return i&0x2; }
inline int l1182::eventBufferNotFull()     { unsigned short i; read16phys(0x0, &i); return i&0x8; }
inline int l1182::eventCounter()           { unsigned short i; read16phys(0x0, &i); return (i>>4)&0xF; }

void l1182::selectFrontPanelGate(){ 
  unsigned short val;
  read16phys(0x0,&val);
  val |= 0x4;
  write16phys(0x0,val); 
  }

void l1182::testGate(){ 
  unsigned short val;
  read16phys(0x0,&val);
  val |= 0x200;
  write16phys(0x0,val); 
}

void l1182::clear(){ 
  unsigned short val;
  read16phys(0x0,&val);
  val |= 0x100;
  write16phys(0x0, val); 
}

inline unsigned int l1182::id() { return ba | ID_L1182; }

int l1182::readSingleEvent(unsigned int * evbuff)
{
  unsigned short data;
  unsigned int addr = 0x100;
  unsigned int i;

  *evbuff++ = 0xFCBBCF00;
  // Loop on all channels
  for (i=0; i<8; i++) {
    read16phys(addr, &data);
    *evbuff++ = data&(0xFFF)|(i<<12);
    addr += 2;
  }
  *evbuff++ = 0xABCCBA00;

  return 10;
}

int l1182::readSingleEventMasked(unsigned int * evbuff, unsigned int mask)
{
  unsigned short data;
  unsigned int addr = 0x100;
  unsigned int i;
  unsigned int size = 2;
  
  *evbuff++ = 0xFCBBCF00;
  // Loop on all channels
  for (i=0; i<8; i++) {
    if(mask&(1<<i)){
      read16phys(addr, &data);
      *evbuff++ = data&(0xFFF)|(i<<12);
      size++;
    }
    addr += 2;
  }
  *evbuff++ = 0xABCCBA00;

  return size;
}

int l1182::readSingleEvent(unsigned int * evbuff, unsigned int ncha)
{
  unsigned short data;
  unsigned int addr = EVTBASEADD;
  unsigned int i;

  *evbuff++ = 0xFCBBCF00;
  // Loop on all channels
  for (i=0; i<ncha; i++) {
    read16phys(addr, &data);
    *evbuff++ = data&(0xFFF)|(i<<12);
    addr += 2;
  }
  *evbuff++ = 0xABCCBA00;

  return ncha+2;
}

int l1182::readEvent(unsigned int * evbuff)
{
  unsigned short data;
  unsigned int addr;
  int nevt = eventCounter();
  int i, j;

  for (j=0; j<nevt; j++) {              // Loop on all stored events
    addr = EVTBASEADD + j*0x10;
    *evbuff++ = 0xFCBBCF00+j;
    for (i=0; i<8; i++) {   // Loop on all channels
      read16phys(addr, &data);
      *evbuff++ = data&(0xFFF)|(i<<12);
      addr += 2;
    }
    *evbuff++ = 0xABCCBA00+j;
  }
  return 10*nevt;
}

int l1182::readEventMasked(unsigned int * evbuff, unsigned int mask)
{
  unsigned short data;
  unsigned int addr;
  int nevt = eventCounter();
  int i, j;
  int sz;

  sz = 2*nevt;
  for (j=0; j<nevt; j++) {              // Loop on all stored events
    addr = EVTBASEADD + j*0x10;
    *evbuff++ = 0xFCBBCF00+j;
    for (i=0; i<8; i++) {   // Loop on all channels
      if(mask&(1<<i)){
	read16phys(addr, &data);
	*evbuff++ = data&(0xFFF)|(i<<12);
	sz ++;
      }
      addr += 2;
    }
    *evbuff++ = 0xABCCBA00+j;
  }

  return sz;
}

/*****************************************/
// Constructor
/*****************************************/
l1182::l1182(unsigned int base, const char *dev):vme(base, 0x200, dev) {};

/*****************************************/
void l1182::print() 
/*****************************************/
{
  cout << "*************************************" << endl;  
  cout << "LeCroy Charge ADC L1182 Module / base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "*************************************" << endl;
  return;
}

/*****************************************/
// L1182 Programming Functions
/*****************************************/

