#ifndef _MY_V792AC_H_
#define _MY_V792AC_H_

/*****************************************

  myV792AC.h
  ---------

  Definition of the CAEN V792AC Charge ADC.
  This class is a class derived from the class vme.

  Version 2014 25.11.2014 roberto.ferrari@pv.infn.it

*****************************************/

#include <unistd.h>

#include "myVme.h"

/*****************************************/
// The v792ac class 
/*****************************************/
class v792ac : public vme {

 public:

  // Constructor
  v792ac(uint32_t base, const char* dev);    

  // Generic Commands

  int32_t readEvent(uint32_t* evbuff);              // Read event from hardware and stores it in user-provided buffer
  inline int32_t getStatus1();
  inline int32_t getControl1();
  inline int32_t getStatus2();
  inline int32_t getBit1();
  inline int32_t getBit2();
  inline int32_t dready(int32_t usec);                  // check if there is data in the output buffer
  inline int32_t dready();                          // check if there is data in the output buffer
  inline int32_t busy();                            // Module is busy
  inline void reset();                          // 2-shot reset
  inline void singleShotReset();                // 1-shot reset
  inline int32_t eventBufferEmpty();                // Checks if event buffer is empty
  inline int32_t eventBufferFull();                 // Checks if event buffer is full
  inline uint32_t eventCounter();                    // Reads 24 bit event counter
  inline void clearData();                      // Clears data
  inline void setEmptyEnable();                 // Always write Header and EOB to memory even if there are no accepted channels
  inline void setCrateNumber(uint16_t cn);// Write crate number into ADC
  inline uint16_t getCrateNumber();       // Write crate number into ADC
  inline void clearEventCounter();              // Clear event counter
  inline void setI1(uint16_t i1);         // Set I1 current value 
  inline int32_t  getI1();                          // Get I1 current value
  inline void enableSlide();                    // Enable slide mechanism (default is enabled at power ON)
  inline void disableSlide();                   // Disable slide mechanism
  uint16_t getSlide();                    // Get slide mechanism status
  inline int32_t  getGeoAddress();                  // Get Geographic Address 
  inline void setGeoAddress(uint16_t ga); // Set Geographic Address 
  void setChannelThreshold(uint16_t thr); // Set threshold 'thr' for all channel
  void setChannelThreshold(int32_t cha, uint16_t thr); // Set threshold 'thr' in channel 'cha'
  uint16_t getChannelThreshold(int32_t cha);  // Returns threshold of channel 'cha'
  void enableChannels();                        // Enable all channels
  void enableChannel(int32_t cha);                  // Enable channel 'cha'
  void disableChannel(int32_t cha);                 // Disable channel 'cha'
  inline void enableTestMode();                 // Enable Random Memory Access Test Mode
  inline void disableTestMode();                // Disable Random Memory Access Test Mode
  inline void testEventWriteReg(int16_t word);   // Write a word for a test event
  inline void memTestAddrW(uint16_t addr);// W Memory Test Address Register 
  inline void memTestAddrR(uint16_t addr);// R Memory Test Address Register 
  inline void memTestWord(uint32_t val);    // 32 bit Test word for Memory Test
  inline void disableAutoIncr();                // disable autoincrement of the readout pointer
  inline void enableTestMemory();                // Allows to select the Random Memory Access Test Mode
  inline void disableTestMemory();                // Allows to select the Random Memory Access Test Mode

  inline void enableOverflowSupp();
  inline void disableOverflowSupp();

  inline void enableZeroSupp();
  inline void disableZeroSupp();

  void setEvents( uint32_t ntriggers ) { m_trignum_now=ntriggers; }

 protected:

 private:
  uint32_t m_eventcounter;
  uint32_t m_trignum_now, m_trignum_pre;

};

/*****************************************/
// Generic Commands
/*****************************************/
inline int32_t  v792ac::getStatus1()              { uint16_t ga; read16phys(0x100E, &ga); return ga; }
inline int32_t  v792ac::getControl1()              { uint16_t ga; read16phys(0x1010, &ga); return ga; }
inline int32_t  v792ac::getStatus2()              { uint16_t ga; read16phys(0x1022, &ga); return ga; }
inline int32_t  v792ac::getBit1()              { uint16_t ga; read16phys(0x1006, &ga); return ga; }
inline int32_t  v792ac::getBit2()              { uint16_t ga; read16phys(0x1032, &ga); return ga; }
inline int32_t  v792ac::dready(int32_t usec)       { m_usleep(usec); return dready(); }
inline int32_t  v792ac::dready()                     { volatile uint16_t i; read16phys(0x100E, &i); return i&1;}
inline int32_t  v792ac::busy()                       { volatile uint16_t i; read16phys(0x100E, &i); return (i>>2)&1;}
inline void v792ac::reset()                      { write16phys(0x1006, 0x80); m_usleep(1); write16phys(0x1008, 0x80); }
inline void v792ac::singleShotReset()            { write16phys(0x1016, 0); }
inline int32_t  v792ac::eventBufferEmpty()           { uint16_t i; read16phys(0x1022, &i); return (i>>1)&1; }
inline int32_t  v792ac::eventBufferFull()            { uint16_t i; read16phys(0x1022, &i); return (i>>2)&1; }
inline uint32_t  v792ac::eventCounter()               { uint16_t h,l; read16phys(0x1026, &h); read16phys(0x1024, &l); return (m_eventCounterOffset+((h<<16)|l)); }
// inline void v792ac::clearData()                  { write16phys(0x1032, 1<<2); write16phys(0x1034, 1<<2); }
inline void v792ac::clearData()                  { write16phys(0x1032, 1<<2);
uint16_t b2; read16phys(0x1032, &b2); write16phys(0x1034, 1<<2); read16phys(0x1032, &b2); } // roberto 19/07/2011

inline void v792ac::setEmptyEnable()             { write16phys(0x1032, 1<<12); m_usleep(1); }
inline void v792ac::enableSlide()                { write16phys(0x1032, 1<<7); m_usleep(1); }
inline void v792ac::disableSlide()               { write16phys(0x1034, 1<<7); m_usleep(1); }
inline uint16_t v792ac::getSlide()         { uint16_t sl; read16phys(0x1032, &sl); return (sl>>7)&1; }
inline int32_t  v792ac::getGeoAddress()              { uint16_t ga; read16phys(0x1002, &ga); return ga; }
inline void v792ac::setGeoAddress(uint16_t ga) { write16phys(0x1002, ga); m_usleep(1); }
inline void v792ac::setCrateNumber(uint16_t cn) { write16phys(0x103C, cn); m_usleep(1); }

inline uint16_t v792ac::getCrateNumber()   { uint16_t i; read16phys(0x103C, &i); return i; }
inline void v792ac::clearEventCounter()          { write16phys(0x1040, 0); m_usleep(1); }
inline void v792ac::setI1(uint16_t i1)     { write16phys(0x1060, i1); m_usleep(1); }
inline int32_t  v792ac::getI1()                      { uint16_t i; read16phys(0x1060, &i); return i; }

inline void v792ac::enableTestMode() { write16phys(0x1032, 1<<6); m_usleep(1); }
inline void v792ac::disableTestMode(){ write16phys(0x1034, 1<<6); m_usleep(1); }
inline void v792ac::enableZeroSupp() { write16phys(0x1034, 1<<4); m_usleep(1); }
inline void v792ac::disableZeroSupp() { write16phys(0x1032, 1<<4); m_usleep(1); }
inline void v792ac::enableOverflowSupp() { write16phys(0x1034, 1<<3); m_usleep(1); }
inline void v792ac::disableOverflowSupp() { write16phys(0x1032, 1<<3); m_usleep(1); }

inline void v792ac::enableTestMemory() { write16phys(0x1032, 1); m_usleep(1); }
inline void v792ac::disableTestMemory() { write16phys(0x1034, 1); m_usleep(1); }
inline void v792ac::testEventWriteReg(int16_t word) { write16phys(0x103E, word); }


inline void v792ac::memTestAddrW(uint16_t addr) { write16phys(0x1036, addr&0x7FF); }
inline void v792ac::memTestAddrR(uint16_t addr) { write16phys(0x1064, addr&0x7FF); }
inline void v792ac::memTestWord(uint32_t val) { write16phys(0x1038, val>>16); write16phys(0x103A, val&0xFFFF); }

inline void v792ac::disableAutoIncr() { write16phys(0x1034, 1<<11); }

#endif // _MY_V792AC_H_
