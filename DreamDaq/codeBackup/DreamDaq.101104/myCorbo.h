//$Id: myCorbo.h,v 1.2 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myCorbo.h
  ---------

  Definition of the CREATIVE ELECTRONIC SYSTEM RCB8047 CORBO class.
  This class is a class derived from the class vme.

  W. Vandelli / INFN Pavia

  Version 0.1: 15/06/2006

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

#define CHECKCHANNEL(CH) { \
if ((CH < 1)||(CH > 4)) { \
  cout << "Channel: cannot set - channel " << CH << \
    " does not exist" << endl; \
  exit(1); \
} \
}

#define BITENABLE(CH, OFF, MUL, BIT) { \
unsigned short val; \
unsigned short addr; \
addr=MUL*(CH-1)+OFF; \
read16phys(addr, &val); \
write16phys(addr, val|BIT); \
}

#define BITDISABLE(CH, OFF, MUL, BIT) { \
unsigned short val; \
unsigned short addr; \
addr=MUL*(CH-1)+OFF; \
read16phys(addr, &val); \
write16phys(addr, val&(~(BIT))); \
}

#define GETBIT(CH, OFF, MUL, BIT, SHIFT){ \
unsigned short val; \
unsigned short addr; \
addr=MUL*(CH-1)+OFF; \
read16phys(addr, &val); \
return (val&BIT)>>SHIFT; \
}

/*****************************************/
// The corbo class 
/*****************************************/
class corbo : public vme {
  
 public:
  
  // Constructor
  corbo(unsigned int base, const char *dev);    
  
  
  //Generic Commands
  inline unsigned int id();                           // Module ID 
  //  inline void reset();                            // Module Reset

  // Set/get functioning modes
  void enableChannel(int cha);                   // Enable Channel
  void disableChannel(int cha);                  // Enable Channel
  unsigned int isChannelEnabled(int cha);        // Return CRSi bit0

  void setBusyModeLevel(int cha);                // BUSY is a level
  void setBusyModeFollower(int cha);             // BUSY follows the trigger input
  unsigned int getBusyMode(int cha);             // Return CRSi bit1

  void setInputFrontPanel(int cha);              // Front panel input
  void setInputDiffIn(int cha);                  // Differential input bus
  void setInputDiffOut(int cha);                 // Differential output bus
  void setInputInternal(int cha);               // Internal test pulse
  unsigned int getInputMode(int cha);             // Return CRSi bit2,bit3
  
  void setBusyOutputLocal(int cha);              // Local BUSY
  void setBusyOutputDiff(int cha);               // Differential output BUSY
  unsigned int getBusyOutputMode(int cha);       // Return CRSi bit4

  void setCounterInput(int cha);                 // Count input
  void setCounterBusy(int cha);                  // Count BUSY
  unsigned int getCounterMode(int cha);          // Return CRSi bit5

  void enableFastClear(int cha);                 // Enable fast clear
  void disableFastClear(int cha);                // Disable fast clear
  unsigned int getFastClearMode(int cha);        // Return CRSi bit6

  void enableButton(int cha);                    // Enable push button
  void disableButton(int cha);                   // Disable push button 
  unsigned int getButtonMode(int cha);           // Return CRSi bit7

  //Get CSRi states
  unsigned int getInputState(int cha);           // Return CRSi bit8
  unsigned int getBusyState(int cha);            // Return CRSi bit9
  unsigned int getDiffBusyState(int cha);        // Return CRSi bit10
  unsigned int getVMEIRQState(int cha);          // Return CRSi bit11
  
  //Set/Get counters
  void setEventCounter(int cha, unsigned int val);// Set Event counter
  unsigned int getEventCounter(int cha);         // Return event counter content

  void setDeadTimeCounter(int cha, unsigned short val);// Set Dead Time counter
  unsigned int getDeadTimeCounter(int cha);      // Return Dead Time counter content

  //IRQ
  void disableEventIRQ(int cha);                 // Disable Event IRQ
  void enableEventIRQ(int cha);                  // Enable Event IRQ
  unsigned int isEventIRQEnabled(int cha);       // Return BIMi bit4

  void disableTimeOutIRQ(int cha);               // Disable Time-out IRQ
  void enableTimeOutIRQ(int cha);                // Enable Time-out IRQ

  //Test
  void testTrigger(int cha);                     // Simulate input trigger
  
  //Clear
  void clearBusy(int cha);                       // Clear BUSY

  //int readEvent(unsigned int *evbuff);         // Read event from hardware and stores it in user-provided buffer

  void  print();

 protected:

 private:
};


/**************************************************************/
//   En/Dis able Channel
/**************************************************************/

void corbo::enableChannel(int cha){
  CHECKCHANNEL(cha);
  
  BITDISABLE(cha,0x0,0x2,0x1);
}	

void corbo::disableChannel(int cha){
  CHECKCHANNEL(cha);

  BITENABLE(cha,0x0,0x2,0x1);
}	

unsigned int corbo::isChannelEnabled(int cha){
  CHECKCHANNEL(cha);

  GETBIT(cha,0x0,0x2,0x1,0);
}

/*****************************************/
// Set BUSY mode
/*****************************************/

void corbo::setBusyModeLevel(int cha){
  CHECKCHANNEL(cha);
  
  BITDISABLE(cha,0x0,0x2,0x2);
}

void corbo::setBusyModeFollower(int cha){
 
  CHECKCHANNEL(cha);

  BITENABLE(cha,0x0,0x2,0x2);
}

unsigned int corbo::getBusyMode(int cha){
  CHECKCHANNEL(cha);

  GETBIT(cha,0x0,0x2,0x2,1);
}

/*****************************************/
// Set INPUT mode
/*****************************************/

void corbo::setInputFrontPanel(int cha){
  CHECKCHANNEL(cha);
  
  BITDISABLE(cha,0x0,0x2,0xC);
}

void corbo::setInputDiffIn(int cha){
  CHECKCHANNEL(cha);
  
  BITDISABLE(cha,0x0,0x2,0x8);
  BITENABLE(cha,0x0,0x2,0x4);
}
 
void corbo::setInputDiffOut(int cha){
  CHECKCHANNEL(cha);
  
  BITDISABLE(cha,0x0,0x2,0x4);
  BITENABLE(cha,0x0,0x2,0x8);
}

void corbo::setInputInternal(int cha){
  CHECKCHANNEL(cha);
  
  BITENABLE(cha,0x0,0x2,0xC);
}

unsigned int corbo::getInputMode(int cha){
  CHECKCHANNEL(cha);

  GETBIT(cha,0x0,0x2,0xC,2);
}

/*****************************************/
// Set BUSY output mode
/*****************************************/

void corbo::setBusyOutputLocal(int cha){
  CHECKCHANNEL(cha);
  
  BITDISABLE(cha,0x0,0x2,0x10);
}

void corbo::setBusyOutputDiff(int cha){
  CHECKCHANNEL(cha);
  
  BITENABLE(cha,0x0,0x2,0x10);
}

unsigned int corbo::getBusyOutputMode(int cha){
  CHECKCHANNEL(cha);
  
  GETBIT(cha,0x0,0x2,0x10,4);
}

/*****************************************/
// Set counter mode
/*****************************************/

void corbo::setCounterInput(int cha){
  CHECKCHANNEL(cha);
  
  BITDISABLE(cha,0x0,0x2,0x20);
}

void corbo::setCounterBusy(int cha){
  CHECKCHANNEL(cha);
  
  BITENABLE(cha,0x0,0x2,0x20);
}

unsigned int corbo::getCounterMode(int cha){
  CHECKCHANNEL(cha);

  GETBIT(cha,0x0,0x2,0x20,5);
}

/*****************************************/
// Set fast clear mode
/*****************************************/

void corbo::enableFastClear(int cha){
  CHECKCHANNEL(cha);
   
  BITDISABLE(cha,0x0,0x2,0x40);
}

void corbo::disableFastClear(int cha){
  CHECKCHANNEL(cha);
  
  BITENABLE(cha,0x0,0x2,0x40);
}

unsigned int corbo::getFastClearMode(int cha){
  CHECKCHANNEL(cha);

  GETBIT(cha,0x0,0x2,0x40,6);
}

/*****************************************/
// Set push button mode
/*****************************************/

void corbo::enableButton(int cha){
   CHECKCHANNEL(cha);
  
   BITDISABLE(cha,0x0,0x2,0x80);
}
 
void corbo::disableButton(int cha){
   CHECKCHANNEL(cha);
  
   BITENABLE(cha,0x0,0x2,0x80);
}

unsigned int corbo::getButtonMode(int cha){
  CHECKCHANNEL(cha);

  GETBIT(cha,0x0,0x2,0x80,7);
}

/*****************************************/
// Get CSR states
/*****************************************/

unsigned int corbo::getInputState(int cha){
  CHECKCHANNEL(cha);

  GETBIT(cha,0x0,0x2,0x100,8);
}

unsigned int corbo::getBusyState(int cha){
  CHECKCHANNEL(cha);

  GETBIT(cha,0x0,0x2,0x200,9);
}

unsigned int corbo::getDiffBusyState(int cha){
  CHECKCHANNEL(cha);

  GETBIT(cha,0x0,0x2,0x400,10);
}

unsigned int corbo::getVMEIRQState(int cha){
  CHECKCHANNEL(cha);

  GETBIT(cha,0x0,0x2,0x800,11);
}

/*****************************************/
// Get/Set Counters
/*****************************************/

void corbo::setEventCounter(int cha, unsigned int val){
  CHECKCHANNEL(cha);

  unsigned short data;
  unsigned short addr; 
  addr=(cha-1)*0x4+0x10;
  data=val>>16;
  write16phys(addr,data); 
  
  data=val&(0xFFFF);
  addr+=0x2;
  write16phys(addr,data); 
  
}

unsigned int corbo::getEventCounter(int cha){
  CHECKCHANNEL(cha);
  
  unsigned int result=0;
  unsigned short val; 
  unsigned short addr; 
  addr=(cha-1)*0x4+0x10; 
  read16phys(addr, &val); 
  result=val<<16;
  
  addr+=0x2;
  read16phys(addr, &val); 
  result+=val;
  return result;
}


void corbo::setDeadTimeCounter(int cha, unsigned short val){
    CHECKCHANNEL(cha);

    unsigned short addr; 
    addr=(cha-1)*0x2+0x20; 
    write16phys(addr, val); 
}
 
unsigned int corbo::getDeadTimeCounter(int cha){
  CHECKCHANNEL(cha);
  
  unsigned short val; 
  unsigned short addr; 
  addr=(cha-1)*0x2+0x20; 
  read16phys(addr, &val); 

  return val;
}


/*****************************************/
// Enable/Disable IRQ
/*****************************************/

void corbo::disableEventIRQ(int cha){
  CHECKCHANNEL(cha);
    
  BITDISABLE(cha,0x31,0x2,0x10);
}

void corbo::enableEventIRQ(int cha){
  CHECKCHANNEL(cha);

  BITENABLE(cha,0x31,0x2,0x10);
}

unsigned int corbo::isEventIRQEnabled(int cha){
  CHECKCHANNEL(cha);
  
  GETBIT(cha,0x31,0x2,0x10,4);
}


void corbo::disableTimeOutIRQ(int cha){
  CHECKCHANNEL(cha);
  
  BITDISABLE(cha,0x41,0x2,0x10);
}
 
void corbo::enableTimeOutIRQ(int cha){
  CHECKCHANNEL(cha);

  BITENABLE(cha,0x41,0x2,0x10);
}

/*****************************************/
// Test
/*****************************************/

void corbo::testTrigger(int cha){
  CHECKCHANNEL(cha);

  unsigned short addr; 
  addr=(cha-1)*0x2+0x50; 
  write16phys(addr, 0x0); 
}

/*****************************************/
// Clear
/*****************************************/

void corbo::clearBusy(int cha){
  CHECKCHANNEL(cha);

  unsigned short addr; 
  addr=(cha-1)*0x2+0x58; 
  write16phys(addr, 0x0); 
}

/*****************************************/
// Generic Commands
/*****************************************/

inline unsigned int corbo::id() { return ba | ID_CORBO; }
//inline void v488::reset()    { write16phys(0x1C, 0x0); }

/*****************************************/
// Constructor
/*****************************************/
corbo::corbo(unsigned int base, const char *dev):vme(base, 0x100, dev) {};

/*****************************************/
void corbo::print() 
/*****************************************/
{
  cout << "************************************" << endl;  
  cout << " CREATIVE ELECTORNIC SYSTEM RCB 8047 CORBO   Module  /  base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "************************************" << endl;
  return;
}
