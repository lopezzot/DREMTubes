//$Id: myV792AC.h,v 1.4 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myV792AC.h
  ---------

  Definition of the CAEN V792AC Charge ADC.
  This class is a class derived from the class vme.

  D. Raspino / Universita' di Cagliari & INFN Cagliari
  A. Cardini / INFN Cagliari

  Version 0.1: 24/1/2003

*****************************************/

#ifndef _MY_V792AC_H_
#define _MY_V792AC_H_

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

#include "myVme.h"

/*****************************************/
// The v792ac class 
/*****************************************/
class v792ac : public vme {

 public:

  // Constructor
  v792ac(unsigned int base, const char *dev);    

  // Generic Commands

  int readEvent(unsigned *evbuff);              // Read event from hardware and stores it in user-provided buffer
  inline unsigned int id();                     // Returns module ID
  inline int  getStatus1();
  inline int  getControl1();
  inline int  getStatus2();
  inline int  getBit1();
  inline int  getBit2();
  inline int dready(int udel);                  // Wait 'udel' usec then check if there is data in the output buffer
  inline int busy();                            // Module is busy
  inline void reset();                          // 2-shot reset
  inline void singleShotReset();                // 1-shot reset
  inline int eventBufferEmpty();                // Checks if event buffer is empty
  inline int eventBufferFull();                 // Checks if event buffer is full
  inline int eventCounter();                    // Reads 24 bit event counter
  inline void clearData();                      // Clears data
  inline void setEmptyEnable();                 // Always write Header and EOB to memory even if there are no accepted channels
  inline void setCrateNumber(unsigned short cn);// Write crate number into ADC
  inline unsigned short getCrateNumber();       // Write crate number into ADC
  inline void clearEventCounter();              // Clear event counter
  inline void setI1(unsigned short i1);         // Set I1 current value 
  inline int  getI1();                          // Get I1 current value
  inline void enableSlide();                    // Enable slide mechanism (default is enabled at power ON)
  inline void disableSlide();                   // Disable slide mechanism
  unsigned short getSlide();                    // Get slide mechanism status
  inline int  getGeoAddress();                  // Get Geographic Address 
  inline void setGeoAddress(unsigned short ga); // Set Geographic Address 
  void setChannelThreshold(unsigned short thr); // Set threshold 'thr' for all channel
  void setChannelThreshold(int cha, unsigned short thr); // Set threshold 'thr' in channel 'cha'
  unsigned short getChannelThreshold(int cha);  // Returns threshold of channel 'cha'
  void enableChannels();                        // Enable all channels
  void enableChannel(int cha);                  // Enable channel 'cha'
  void disableChannel(int cha);                 // Disable channel 'cha'
  inline void enableTestMode();                 // Enable Random Memory Access Test Mode
  inline void disableTestMode();                // Disable Random Memory Access Test Mode
  inline void testEventWriteReg(short word);   // Write a word for a test event
  inline void memTestAddrW(unsigned short addr);// W Memory Test Address Register 
  inline void memTestAddrR(unsigned short addr);// R Memory Test Address Register 
  inline void memTestWord(unsigned int val);    // 32 bit Test word for Memory Test
  inline void disableAutoIncr();                // disable autoincrement of the readout pointer
  inline void enableTestMemory();                // Allows to select the Random Memory Access Test Mode
  inline void disableTestMemory();                // Allows to select the Random Memory Access Test Mode

  inline void enableOverflowSupp();
  inline void disableOverflowSupp();

  inline void enableZeroSupp();
  inline void disableZeroSupp();

  // Crap
  void print();                                 // Simple dump function

 protected:

 private:

};

/*****************************************/
// Generic Commands
/*****************************************/
inline unsigned int v792ac::id()  {return m_id; } //   { return ba | ID_V792AC; }
inline int  v792ac::getStatus1()              { unsigned short ga; read16phys(0x100E, &ga); return ga; }
inline int  v792ac::getControl1()              { unsigned short ga; read16phys(0x1010, &ga); return ga; }
inline int  v792ac::getStatus2()              { unsigned short ga; read16phys(0x1022, &ga); return ga; }
inline int  v792ac::getBit1()              { unsigned short ga; read16phys(0x1006, &ga); return ga; }
inline int  v792ac::getBit2()              { unsigned short ga; read16phys(0x1032, &ga); return ga; }
inline int  v792ac::dready(int udel)             { unsigned short i; usleep(udel); read16phys(0x100E, &i); return i&0x1;}
inline int  v792ac::busy()                       { unsigned short i; read16phys(0x100E, &i); return (i>>2)&0x1;}
inline void v792ac::reset()                      { write16phys(0x1006, 0x80); usleep(10000); write16phys(0x1008, 0x80); }
inline void v792ac::singleShotReset()            { write16phys(0x1016, 0x0); }
inline int  v792ac::eventBufferEmpty()           { unsigned short i; read16phys(0x1022, &i); return (i>>1)&0x1; }
inline int  v792ac::eventBufferFull()            { unsigned short i; read16phys(0x1022, &i); return (i>>2)&0x1; }
inline int  v792ac::eventCounter()               { unsigned short h,l; read16phys(0x1026, &h); read16phys(0x1024, &l); return (h<<16)|l; }
inline void v792ac::clearData()                  { write16phys(0x1032, 0x1<<2); write16phys(0x1034, 0x1<<2); }

inline void v792ac::setEmptyEnable()             { write16phys(0x1032, 0x1<<12); usleep(1); }
inline void v792ac::enableSlide()                { write16phys(0x1032, 0x1<<7); usleep(1); }
inline void v792ac::disableSlide()               { write16phys(0x1034, 0x1<<7); usleep(1); }
inline unsigned short v792ac::getSlide()         { unsigned short sl; read16phys(0x1032, &sl); return (sl>>7)&0x1; }
inline int  v792ac::getGeoAddress()              { unsigned short ga; read16phys(0x1002, &ga); return ga; }
inline void v792ac::setGeoAddress(unsigned short ga) { write16phys(0x1002, ga); usleep(1); }
inline void v792ac::setCrateNumber(unsigned short cn) { write16phys(0x103C, cn); usleep(1); }

inline unsigned short v792ac::getCrateNumber()   { unsigned short i; read16phys(0x103C, &i); return i; }
inline void v792ac::clearEventCounter()          { write16phys(0x1040, 0x0); usleep(1); }
inline void v792ac::setI1(unsigned short i1)     { write16phys(0x1060, i1); usleep(1); }
inline int  v792ac::getI1()                      { unsigned short i; read16phys(0x1060, &i); return i; }

inline void v792ac::enableTestMode() { write16phys(0x1032, 0x1<<6); usleep(1); }
inline void v792ac::disableTestMode(){ write16phys(0x1034, 0x1<<6); usleep(1); }
inline void v792ac::enableZeroSupp() { write16phys(0x1034, 0x1<<4); usleep(1); }
inline void v792ac::disableZeroSupp() { write16phys(0x1032, 0x1<<4); usleep(1); }
inline void v792ac::enableOverflowSupp() { write16phys(0x1034, 0x1<<3); usleep(1); }
inline void v792ac::disableOverflowSupp() { write16phys(0x1032, 0x1<<3); usleep(1); }

inline void v792ac::enableTestMemory() { write16phys(0x1032, 0x1); usleep(1); }
inline void v792ac::disableTestMemory() { write16phys(0x1034, 0x1); usleep(1); }
inline void v792ac::testEventWriteReg(short word) { write16phys(0x103E, word); }


inline void v792ac::memTestAddrW(unsigned short addr) { write16phys(0x1036, addr&0x7FF); }
inline void v792ac::memTestAddrR(unsigned short addr) { write16phys(0x1064, addr&0x7FF); }
inline void v792ac::memTestWord(unsigned int val) { write16phys(0x1038, val>>16); write16phys(0x103A, val&0xFFFF); }


inline void v792ac::disableAutoIncr() { write16phys(0x1034, (0x1<<11)); }

/**************************************************************/
//   En/Dis able Channel
/**************************************************************/

void v792ac::enableChannels()
{
  int addr;
  unsigned short j;

  for (int cha=0; cha<32; cha++) {
    addr = 0x1080 + 2*cha;
    read16phys(addr, &j);
    write16phys(addr, 0x00FF&j);
  }
}

void v792ac::enableChannel(int cha)
{
  int addr;
  unsigned short j;

  if ((cha < 0)||(cha > 31)) {
    cout << m_name << " 0x" << hex << ba << dec << " enableChannel: cannot set - channel " << cha << " does not exist" << endl;
    return;
  }
  addr = 0x1080 + 2*cha;
  read16phys(addr, &j);
  write16phys(addr, 0x0FF&j);
}

void v792ac::disableChannel(int cha)
{
  int addr;
  unsigned short j;
  
  if ((cha < 0)||(cha > 31)) {
    cout << m_name << " 0x" << hex << ba << dec << " disableChannel: cannot set - channel " << cha << " does not exist" << endl;
    return;
  }
  addr = 0x1080 + 2*cha;
  read16phys(addr, &j);
  write16phys(addr, 0x100|j);
}

/**************************************************************/
//   Set/Get  Channel Threshold
/**************************************************************/



void v792ac::setChannelThreshold(unsigned short thr)
{
  int addr;
  unsigned short data;
  for (int cha=0; cha<32; cha++) {
    addr = 0x1080 + 2*cha;
    read16phys(addr, &data);
    write16phys(addr, (data&0x100)|(0xFF&thr));
  }
  return;
}

void v792ac::setChannelThreshold(int cha, unsigned short thr)
{
  int addr;
  unsigned short data;
  
  if ((cha < 0)||(cha > 31)) {
    cout << m_name << " 0x" << hex << ba << dec << " setChannelThreshold: cannot set - channel " << cha << " does not exist" << endl;
    return;
  }
  addr = 0x1080 + 2*cha;
  read16phys(addr, &data);
  write16phys(addr, (data&0x100)|(0xFF&thr));
  return;
}

unsigned short v792ac::getChannelThreshold(int cha)
{
  int addr;
  unsigned short thr; 
  
  if ((cha < 0)||(cha > 31)) {
    cout << m_name << " 0x" << hex << ba << dec << " getChannelThreshold: cannot set - channel " << cha << " does not exist" << endl;
    return   0xFF;
  }
  addr = 0x1080 + 2*cha;
  read16phys(addr, &thr);
  return 0xFF&thr;
}



int v792ac::readEvent(unsigned *evbuff)
{
  unsigned int data;
  int sz=0;
  int addr = 0x0;
  
  //while (read32phys(addr, &data), ( (data>>24) & 0x7) != 0x6) {
  while (read32phys(addr, &data), ( !((data>>26)&0x1))) {
    addr += 4;
    *evbuff++ = data;
    sz++;
    // cout << m_name << " 0x" << hex << ba << " " << addr/4 << " data: 0x" << data << dec << endl;
  }
  addr += 4;
  *evbuff++ = data;
  sz++;
  
  // cout << m_name << " 0x" << hex << ba << " " << addr/4 << " data: 0x" << data << dec << endl;
  //cout << endl;
  //cout << endl;

  return sz;
}

/*****************************************/
// Constructor
/*****************************************/
v792ac::v792ac(unsigned int base, const char *dev):vme(base, 0x2000, dev)
{
  m_id = base | ID_V792AC;
  m_name = "CAEN Charge ADC V792AC ";
}

/*****************************************/
void v792ac::print() 
/*****************************************/
{
  cout << "************************************" << endl;  
  cout << m_name << " Module / base " << hex << ba << ", mmap " << &vbuf << ", length " << dec << length << endl;
  cout << "************************************" << endl;
  return;
}

/*****************************************/
// V792AC Programming Functions
/*****************************************/


#endif // _MY_V792AC_H_
