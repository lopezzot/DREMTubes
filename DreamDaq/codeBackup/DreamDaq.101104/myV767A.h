//$Id: myV767A.h,v 1.2 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myV767A.h
  ---------

  Definition of the CAEN TDC V767A class.
  This class is a class derived from the class vme.

  A. Cardini / INFN Cagliari

  Version 0.1: 24/5/2001

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
// The v767a class 
/*****************************************/
class v767a : public vme {

 public:

  // Constructor
  v767a(unsigned int base, const char *dev);    

  // Generic Commands
  int readEvent(unsigned *evbuff);              // Read event from hardware and stores it in user-provided buffer
  inline int dready(int udel);                  // Wait 'udel' usec then checks if data is available for readout
  inline void clear();                          // Clears TDCs/outbuff/reset readout controller and ev. counter 
  inline void reset();                          // Single shot reset (nowait, returns immediately)
  inline void resetwait();                      // Single shot reset (with 2s wait afterwards)
  inline unsigned int id();                     // Module ID 

  // Acquisition Mode
  void setStopTriggerMatching();                // sets Stop Trigger Matching
  void setStartTriggerMatching();               // sets Start Trigger Matching
  void setStartGating();                        // sets Start Gating
  void setContinuousStorage();                  // sets Continuous Storage
  int getAcquisitionMode();                    // Gets Acquisition Mode

  // Channel Enable
  void enableChannel(int channel);                
  void disableChannel(int channel);
  void enableAllChannels();
  void disableAllChannels();

  // Trigger
  void setWindowWidth(unsigned short ticks);
  void setWindowOffset(unsigned short ticks);
  unsigned short getWindowWidth();
  unsigned short getWindowOffset();

  // Start
  void enableStartTimeReadout();
  void disableStartTimeReadout();
  void enableStartTimeSubtraction();
  void disableStartTimeSubtraction();
  void enableTriggerTimeSubtraction();
  void disableTriggerTimeSubtraction();

  // Edge Detection
  void setRisingEdgeOnly();
  void setFallingEdgeOnly();
  void setBothEdges();

  // Data Ready
  void setDataReadyEvent();
  void setDataReadyAlmostFullBuffer();
  void setDataReadyNotEmptyBuffer();
  unsigned short getDataReadyMode();
  void setAlmostFullLevel(unsigned short level);
  unsigned short getAlmostFullLevel();

  // Configuration

  void loadDefaultConfiguration();
  void saveUserConfiguration();
  void loadUserConfiguration();
  void enableAutoLoad();
  void disableAutoLoad();
  unsigned short readAutoLoad();

  // Test Memory

  inline void enableTestMode();                 // Enable  Memory Access Test Mode
  inline void disableTestMode();                // Disable  Memory Access Test Mode
  inline void memTestWord(unsigned int val);    // 32 bit Test word for Memory Test

  // Debug

  unsigned int readTdcErrorCode();

  // Crap
  void print();                                 // Simple dump function

 protected:

  // V767A Programming Functions
  int opwrite(unsigned short data);             // Send data to TDC microcontroller
  int opread(unsigned short *data);             // Get data from TDC microcontroller 

 private:

};

/*****************************************/
// Generic Commands
/*****************************************/
inline unsigned int v767a::id()        { return ba | ID_V767A; }
inline void v767a::reset()             { write16phys(0x18, 1); }
inline void v767a::resetwait()         { write16phys(0x18, 1); usleep(2000000); }
inline void v767a::clear()             { write16phys(0x54, 1); }
inline int  v767a::dready(int udel)    { unsigned short i; usleep(udel); read16phys(0xE, &i); return i&0x1;}
int v767a::readEvent(unsigned *evbuff)
{
  unsigned int data;
  int sz = 0;
  
  while (read32phys(0x0, &data), ( (data>>21) & 0x3) != 0x3) {
    *evbuff++ = data;
    sz++;
    //    cout << hex << "Data 0x" << data << endl;
  }
  return sz;
}
 
/*****************************************/
// Acquisition Mode
/*****************************************/
inline void v767a::setStopTriggerMatching()  { opwrite(0x1000); }
inline void v767a::setStartTriggerMatching() { opwrite(0x1100); }
inline void v767a::setStartGating()          { opwrite(0x1200); }
inline void v767a::setContinuousStorage()    { opwrite(0x1300); }
inline int  v767a::getAcquisitionMode()      { unsigned short i; opwrite(0x1400); opread(&i); return i&0x3;}

/*****************************************/
// Channel Enable
/*****************************************/
inline void v767a::enableChannel(int channel)     { opwrite(0x2000+channel); }
inline void v767a::disableChannel(int channel)    { opwrite(0x2100+channel); }
inline void v767a::enableAllChannels()            { opwrite(0x2300); }
inline void v767a::disableAllChannels()           { opwrite(0x2400); }

/*****************************************/
// Trigger
/*****************************************/
inline void v767a::setWindowWidth(unsigned short ticks)   { opwrite(0x3000); opwrite(ticks); }
inline unsigned short v767a::getWindowWidth()             { unsigned short i; opwrite(0x3100); opread(&i); return i;}
inline void v767a::setWindowOffset(unsigned short ticks)  { opwrite(0x3200); opwrite(ticks); }
inline unsigned short v767a::getWindowOffset()            { unsigned short i; opwrite(0x3300); opread(&i); return i;}

/*****************************************/
// Start
/*****************************************/
inline void v767a::enableStartTimeReadout()          { opwrite(0x4000); }
inline void v767a::disableStartTimeReadout()         { opwrite(0x4200); }
inline void v767a::enableStartTimeSubtraction()      { opwrite(0x4300); }
inline void v767a::disableStartTimeSubtraction()     { opwrite(0x4400); }
inline void v767a::enableTriggerTimeSubtraction()    { opwrite(0x3600); }
inline void v767a::disableTriggerTimeSubtraction()   { opwrite(0x3700); }

/*****************************************/
// Edge Detection
/*****************************************/
inline void v767a::setRisingEdgeOnly()               { opwrite(0x6000); }
inline void v767a::setFallingEdgeOnly()              { opwrite(0x6100); }
inline void v767a::setBothEdges()                    { opwrite(0x6600); }

/*****************************************/
// Data Ready
/*****************************************/
inline void v767a::setDataReadyEvent()                        { opwrite(0x7000); }
inline void v767a::setDataReadyAlmostFullBuffer()             { opwrite(0x7100); }
inline void v767a::setDataReadyNotEmptyBuffer()               { opwrite(0x7200); }
inline unsigned short v767a::getDataReadyMode()               { unsigned short i; opwrite(0x7300); opread(&i); return i&0x3;}
inline void v767a::setAlmostFullLevel(unsigned short level)   { opwrite(0x7400); opwrite(level&0x7FFF); }
inline unsigned short v767a::getAlmostFullLevel()             { unsigned short i; opwrite(0x7500); opread(&i); return i&0x7FFF;}

/*****************************************/
// Configuration
/*****************************************/

inline void v767a::loadDefaultConfiguration()        { opwrite(0x1500); }
inline void v767a::saveUserConfiguration()           { opwrite(0x1600); }
inline void v767a::loadUserConfiguration()           { opwrite(0x1700); }
inline void v767a::enableAutoLoad()                  { opwrite(0x1800); }
inline void v767a::disableAutoLoad()                 { opwrite(0x1900); }
inline unsigned short v767a::readAutoLoad()          { unsigned short i; opwrite(0x1A00); opread(&i); return i&0x1;}

/*****************************************/
// Test Memory
/*****************************************/

inline void v767a::enableTestMode()                  { write16phys(0x004A, 0x1<<4); }
inline void v767a::disableTestMode()                 { write16phys(0x004A, 0x0<<4); }
inline void v767a::memTestWord(unsigned int val)     { write16phys(0x0058, val&0xFFFF); write16phys(0x0056, val>>16); }

/*****************************************/
// Debug
/*****************************************/
unsigned int v767a::readTdcErrorCode() {

  unsigned short i0, i1;

  opwrite(0x8000); opread(&i0);
  opwrite(0x8001); opread(&i1);

  return ((i0&0xF)|((i1&0xF)<<4));
}

/*****************************************/
// Constructor
/*****************************************/
v767a::v767a(unsigned int base, const char *dev):vme(base, 0x1000, dev) {};

/*****************************************/
void v767a::print() 
/*****************************************/
{
  cout << "*************************************" << endl;  
  cout << "CAEN TDC V767A Module / base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "*************************************" << endl;
  return;
}

/*****************************************/
// V767A Programming Functions
/*****************************************/
int v767a::opwrite(unsigned short data)
/*****************************************/
{
  unsigned short rdata;
  int time = 0;
  const int TIMEOUT = 1000000;

  do {
    read16phys(0x50, &rdata);
    time++;
  } while ((rdata != 0x2) && (time < TIMEOUT));

  if (time == TIMEOUT) {
    cout << "v767a::opwrite: timeout!" << endl;
    return -1;
  }
  usleep(100);    /* 10ms 1 us (raspino 12.4.2005) sleep before sending real data */
  write16phys(0x52, data);
  return 0;
}

/*****************************************/
int v767a::opread(unsigned short *data)
/*****************************************/
{
  unsigned short rdata;
  int time = 0;
  const int TIMEOUT = 100000;

  do {
    read16phys(0x50, &rdata);
    time++;
  } while ((rdata != 0x1) && (time < TIMEOUT));

  if (time == TIMEOUT) {
    cout << "v767a::opread: timeout!" << endl;
    return -1;
  }
  usleep(100);    /* (10ms) 1 us (raspino 12.4.2005) sleep before sending real data */
  read16phys(0x52, data);
  return 0;
}














