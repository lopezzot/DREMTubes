#ifndef _MY_V775_H_
#define _MY_V775_H_

/*****************************************

  myV775.h
  ---------

  Definition of the CAEN V775 TDC.
  This class is a class derived from the class vme.

*****************************************/

#include <iostream>
#include <string>

#include "myVme.h"

#define V775_SETBITS(OFF,BITS) do { \
uint32_t val; \
read32phys(OFF,&val); \
val |= BITS; \
write32phys(OFF,val); \
} while(0)

#define V775_RESETBITS(OFF,BITS) do { \
uint32_t val; \
read32phys(OFF,&val); \
val &= (~(BITS)); \
write32phys(OFF,val); \
} while(0)

#define V775_GETBITS(OFF,BITS,SHIFT) do { \
uint32_t val; \
read32phys(OFF,&val); \
return ((val&BITS)>>SHIFT); \
} while(0)

#define V775_MARKER             0xCAE77500

// Set Register 1
#define V775_SELECT_ADDR_BIT    0x10
#define V775_SOFT_RESET_BIT     0x80

// Set register 2
#define V775_MEM_TEST_BIT       0x1
#define V775_OFFLINE_BIT        0x2
#define V775_CLEAR_DATA_BIT     0x4
#define V775_OVER_SUPPR_BIT     0x8
#define V775_ZERO_SUPPR_BIT     0x10
#define V775_VALID_CTRL_BIT     0x20
#define V775_TEST_ACQ_BIT       0x40
#define V775_SLIDE_ENABLED_BIT  0x80
#define V775_STEP_TH_BIT        0x100
#define V775_UNUSED             0x200
#define V775_START_STOP_BIT     0x400
#define V775_AUTO_INCR_BIT      0x800
#define V775_EMPTY_PROG_BIT     0x1000
#define V775_SLIDE_SUB_ENAB_BIT 0x2000
#define V775_ALL_TRG_BIT        0x4000

// Status register 1
#define V775_DATA_READY_BIT     0x1
#define V775_DATA_READY_G_BIT   0x2
#define V775_BUSY_BIT           0x4
#define V775_BUSY_G_BIT         0x8
#define V775_AMNESIA_BIT        0x10
#define V775_PURGED_BIT         0x20
#define V775_TERM_ON_BIT        0x40
#define V775_TERM_OFF_BIT       0x80
#define V775_EVRDY_BIT          0x100	

// Status register 2
#define V775_BUFFER_EMPTY_BIT   0x2
#define V775_BUFFER_EMPTY_FULL  0x4

// Control register 1
#define V775_BLK_END_BIT        0x4
#define V775_PROG_RESET_BIT     0x10
#define V775_BUS_ERR_ENABL_BIT  0x20
#define V775_ALIGN64_BIT        0x40

#define dataDecodeType(VAL)       (((VAL) >> 24) & 0x7     )
#define dataDecodeValue(VAL)      ( (VAL)        & 0xFFF   )
#define dataDecodeChannel(VAL)    (((VAL) >> 16) & 0x1F    )
#define dataDecodeEvtCnt(VAL)     ( (VAL)        & 0xFFFFFF)
#define dataDecodeChCnt(VAL)      (((VAL) >>  8) & 0x3F    )
#define dataDecodeValidity(VAL)   (((VAL) >> 14) & 0x1     ) // Bit 14
#define dataDecodeUnderThr(VAL)   (((VAL) >> 13) & 0x1     ) // Bit 13
#define dataDecodeOverFlow(VAL)   (((VAL) >> 12) & 0x1     ) // Bit 12
#define dataDecodeFlags(VAL)      (((VAL) >> 12) & 0x7     ) // Bit 12, 13 and 14
 
/*****************************************/
// The v775 class 
/*****************************************/
class v775 : public vme {

 public:

  // Constructor
  v775(uint32_t base, const char *dev);    
  ~v775();

  uint16_t geoAddr();
  void geoAddr(uint16_t adr);
  uint16_t mcst_cbltAddr();
  void mcst_cbltAddr(uint16_t adr);
  uint16_t bsetReg1();
  void bsetReg1(uint16_t mask);
  uint16_t bclearReg1();
  void bclearReg1(uint16_t mask);
  uint16_t irqlReg();
  void irqlReg(uint16_t lev);
  uint16_t irqlVec();
  void irqlVec(uint16_t vec);
  uint16_t stReg1();
  uint16_t ctrlReg1();
  void ctrlReg1(uint16_t sets);
  uint16_t hiAddr();
  void hiAddr(uint16_t hiad);
  uint16_t loAddr();
  void loAddr(uint16_t load);
  void sshotReset();
  uint16_t mcst_cbltCtrl();
  void mcst_cbltCtrl(uint16_t sets);
  uint16_t evTrig();
  void evTrig(uint16_t nev);
  uint16_t stReg2();
  bool dready();
  uint16_t evcLo();
  uint16_t evcHi();
  inline uint32_t eventCounter();                    // Reads 24 bit event counter
  inline void clearEventCounter();              // Clear event counter
  void evIncr();
  void ofsIncr();
  uint16_t fastClrW();
  void fastClrW(uint16_t tw);
  uint16_t bsetReg2();
  void bsetReg2(uint16_t mask);
  void bclearReg2(uint16_t mask);
  void wmemTest(uint16_t wadr);
  void wmemHi(uint16_t whi);
  void wmemLo(uint16_t wlo);
  uint16_t csel();
  void csel(uint16_t cnr);
  void testEvent(void * evtp);
  void evCntReset();
  uint16_t fullScale();
  void fullScale(uint16_t fsr);
  void rmemTest(uint16_t radr);
  void swComm();
  uint16_t slideConst();
  void slideConst(uint16_t sld);
  uint16_t aadReg();
  uint16_t badReg();
  uint16_t thresMem(uint16_t chan);
  void thresMem(uint16_t chan, uint16_t thr);
  uint16_t romMem(uint16_t chan);
  void romPrint();

  //Control register
  void testCycle();      //Execute test cycle
  void enableEvenChTest();   //Enable test for even channels
  void disableEvenChTest();   //disable test for even channels
  void enableOddChTest();   //Enable test for odd channels
  void disableOddChTest();   //disable test for odd channels
  void setTestNHits(uint32_t hits);      //Set number of hists during test
  void setTestHitDuration(uint32_t time); //Set duration of test hits
  void setCommStartTimeout(uint32_t time);  //Set common start timeout
  void enableAuxCommonHit();                   //??????
  void disableAuxCommonHit();                   //??????
  void enableFallingEdge();     //Enable falling edge registration
  void disableFallingEdge();     //disable falling edge registration
  void enableRisingEdge();     //Enable rising edge registration
  void disableRisingEdge();     //disable rising edge registration
  void setTriggerPulse();   //Each hit
  void setTriggerLatch();   //Latch first hit
  void setCommStart();     //Common start
  void setCommStop();      //Common stop
  void setFastCLWindow(uint32_t time); //set fast clear window
  void enableFastClear();    //enable fast clear
  void disableFastClear();    //disable fast clear
  void enableAcq();        //enable data acquisition
  void disableAcq();       //disable data acquisition
  uint32_t isAcqEnabled();      //acquisition status
  uint32_t roip();          //read-out in progress
  void clear();         // clear
  uint32_t evtCnt();    //return number of events
  uint32_t isBufferFull();  //return bufer full bit
  uint32_t getCSR0();

  uint32_t readSingleEvent(uint32_t *buf);  //read data
  uint32_t readData(uint32_t *buf);  //read data

  void swReset();        // Software reset
  void dataReset();      // Data reset
  
  enum common {commonStart, commonStop};

  void     zeroSuppression(bool on); // Enable/disable zero-suppression
  bool     zeroSuppression();        // Get zero-suppression status
  void     overSuppression(bool on); // Enable/disable overflow suppression
  bool     overSuppression();        // Get overflow suppression status
  void     setCommon(uint32_t c);    // Switch between v775::commonStart/v775::commonStop  
  common   getCommon();              // Return v775::commonStart or v755::commonStop          
  std::string   getMode();
  uint32_t setLSB(uint32_t ps);      // Set the LSB in ps
  uint32_t setFullScale(uint32_t ns);// Set the end scale in ns
  void storeEmpty();                 // store empty events too
  uint32_t getPicoSecPerCount();     // Picoseconde per TDC count 
 
  void     setTestMode();            // Set the acquisition test mode (debugging)
  void     printRegisters();         // Dump registers to stdout
  bool     isDataReady();
  bool     isBusy();

  bool     isWordValid(uint32_t v);  // Check the status bits of the data word
  void     printDataHdr(uint32_t v);
  void     printDataBody(uint32_t v);
  void     printDataEnd(uint32_t v);

 protected:

 private:
  uint32_t m_eventcounter;

};

inline void v775::clearEventCounter()       { write16phys(0x1040, 0); }
inline uint32_t v775::eventCounter()        { uint16_t h,l; read16phys(0x1026, &h); read16phys(0x1024, &l); return (m_eventCounterOffset+((h<<16)|l)); }

inline uint16_t v775::geoAddr()
  {
   uint16_t i;
   read16phys(0x1002,&i);
   return i;
  }
inline void v775::geoAddr(uint16_t adr)
  {
   write16phys(0x1002,adr);
  }
inline uint16_t v775::mcst_cbltAddr()
  {
   uint16_t i;
   read16phys(0x1004,&i);
   return i;
  }
inline void v775::mcst_cbltAddr(uint16_t adr)
  {
   write16phys(0x1004,adr);
  }
inline uint16_t v775::bsetReg1()
  {
   uint16_t i;
   read16phys(0x1006,&i);
   return i;
  }
inline void v775::bsetReg1(uint16_t mask)
  {
   write16phys(0x1006,mask);
  }
inline uint16_t v775::bclearReg1()
  {
   uint16_t i;
   read16phys(0x1008,&i);
   return i;
  }
inline void v775::bclearReg1(uint16_t mask)
  {
   write16phys(0x1008,mask);
  }
inline uint16_t v775::irqlReg()
  {
   uint16_t i;
   read16phys(0x100a,&i);
   return i;
  }
inline void v775::irqlReg(uint16_t lev)
  {
   write16phys(0x100a,lev);
  }
inline uint16_t v775::irqlVec()
  {
   uint16_t i;
   read16phys(0x100c,&i);
   return i;
  }
inline void v775::irqlVec(uint16_t vec)
  {
   write16phys(0x100c,vec);
  }
inline uint16_t v775::stReg1()
  {
   uint16_t i;
   read16phys(0x100e,&i);
   return i;
  }
inline uint16_t v775::ctrlReg1()
  {
   uint16_t i;
   read16phys(0x1010,&i);
   return i;
  }
inline void v775::ctrlReg1(uint16_t sets)
  {
   write16phys(0x1010,sets);
  }
inline uint16_t v775::hiAddr()
  {
   uint16_t i;
   read16phys(0x1012,&i);
   return i;
  }
inline void v775::hiAddr(uint16_t hiad)
  {
   write16phys(0x1012,hiad);
  }
inline uint16_t v775::loAddr()
  {
   uint16_t i;
   read16phys(0x1014,&i);
   return i;
  }
inline void v775::loAddr(uint16_t load)
  {
   write16phys(0x1014,load);
  }
inline void v775::sshotReset()
  {
   write16phys(0x1016,0);
  }
inline uint16_t v775::mcst_cbltCtrl()
  {
   uint16_t i;
   read16phys(0x101a,&i);
   return i;
  }
inline void v775::mcst_cbltCtrl(uint16_t sets)
  {
   write16phys(0x101a,sets);
  }
inline uint16_t v775::evTrig()
  {
   uint16_t i;
   read16phys(0x1020,&i);
   return i;
  }
inline void v775::evTrig(uint16_t nev)
  {
   write16phys(0x1020,nev);
  }
inline uint16_t v775::stReg2()
  {
   uint16_t i;
   read16phys(0x1022,&i);
   return i;
  }
inline bool v775::dready()  { volatile uint16_t i; read16phys(0x100E, &i); return i&1;}
inline uint16_t v775::evcLo()
  {
   uint16_t i;
   read16phys(0x1024,&i);
   return i;
  }
inline uint16_t v775::evcHi()
  {
   uint16_t i;
   read16phys(0x1026,&i);
   return i;
  }
inline void v775::evIncr()
  {
   write16phys(0x1028,0);
  }
inline void v775::ofsIncr()
  {
   write16phys(0x102a,0);
  }
inline uint16_t v775::fastClrW()
  {
   uint16_t i;
   read16phys(0x102e,&i);
   return i;
  }
inline void v775::fastClrW(uint16_t tw)
  {
   write16phys(0x102e,tw);
  }
inline uint16_t v775::bsetReg2()
  {
   uint16_t i;
   read16phys(0x1032,&i);
   return i;
  }
inline void v775::bsetReg2(uint16_t mask)
  {
   write16phys(0x1032,mask);
  }
inline void v775::bclearReg2(uint16_t mask)
  {
   write16phys(0x1034,mask);
  }
inline void v775::wmemTest(uint16_t wadr)
  {
   write16phys(0x1036,wadr);
  }
inline void v775::wmemHi(uint16_t whi)
  {
   write16phys(0x1038,whi);
  }
inline void v775::wmemLo(uint16_t wlo)
  {
   write16phys(0x103a,wlo);
  }
inline uint16_t v775::csel()
  {
   uint16_t i;
   read16phys(0x103c,&i);
   return i;
  }
inline void v775::csel(uint16_t cnr)
  {
   write16phys(0x103c,cnr);
  }
inline void v775::testEvent(void * evtp)
  {
   uint16_t * u16p = static_cast<uint16_t *>(evtp);
   for (uint8_t i=32; i; i--) write16phys(0x103e,*u16p++);
  }
inline void v775::evCntReset()
  {
   write16phys(0x1040,0);
  }
inline uint16_t v775::fullScale()
  {
   uint16_t i;
   read16phys(0x1060,&i);
   return i;
  }
inline void v775::fullScale(uint16_t fsr)
  {
   write16phys(0x1060,fsr);
  }
inline void v775::rmemTest(uint16_t radr)
  {
   write16phys(0x1064,radr);
  }
inline void v775::swComm()
  {
   write16phys(0x1068,0);
  }
inline uint16_t v775::slideConst()
  {
   uint16_t i;
   read16phys(0x106a,&i);
   return i;
  }
inline void v775::slideConst(uint16_t sld)
  {
   write16phys(0x106a,sld);
  }
inline uint16_t v775::aadReg()
  {
   uint16_t i;
   read16phys(0x1070,&i);
   return i;
  }
inline uint16_t v775::badReg()
  {
   uint16_t i;
   read16phys(0x1072,&i);
   return i;
  }
inline uint16_t v775::thresMem(uint16_t chan)
  {
   uint16_t i;
   uint32_t addr = 0x1080 + 2*chan;
   read16phys(addr,&i);
   return i;
  }
inline void v775::thresMem(uint16_t chan, uint16_t thr)
  {
   uint32_t addr = 0x1080 + 2*chan;
   write16phys(addr,thr);
  }
inline uint16_t v775::romMem(uint16_t adr)
  {
   uint16_t i;
   uint32_t addr = 0x8000 + adr;
   read16phys(addr,&i);
   return i;
  }

inline bool v775::isWordValid(uint32_t v)
{ return dataDecodeType(v) != 0x6; }

inline uint32_t v775::evtCnt()
{
  uint16_t lo = evcLo(); 
  uint16_t hi = evcHi();
  return (hi<<16)|lo;
}

inline void v775::swReset()
{
  bsetReg1(V775_SOFT_RESET_BIT);   // Set the SOFT_RESET bit
  bclearReg1(V775_SOFT_RESET_BIT); // Clear the SOFT_RESET bit 
}

inline void v775::dataReset()
{
  bsetReg2(V775_CLEAR_DATA_BIT);   // Set the CLEAR_DATA bit
  bclearReg2(V775_CLEAR_DATA_BIT); // Clear the SOFT_RESET bit 
}

inline void v775::zeroSuppression(bool on)
{ (on)? bclearReg2(V775_ZERO_SUPPR_BIT) : bsetReg2(V775_ZERO_SUPPR_BIT); }

inline bool v775::zeroSuppression() 
{ return bsetReg2() & V775_ZERO_SUPPR_BIT; }

inline void v775::overSuppression(bool on)
{ (on)? bclearReg2(V775_OVER_SUPPR_BIT) : bsetReg2(V775_OVER_SUPPR_BIT); }

inline bool v775::overSuppression() 
{ return bsetReg2() & V775_OVER_SUPPR_BIT; }

inline void v775::setCommon(uint32_t c)  
{
  if(c==v775::commonStop)  bsetReg2(V775_START_STOP_BIT);
  if(c==v775::commonStart) bclearReg2(V775_START_STOP_BIT); 
}
  
inline v775::common v775::getCommon()
{
  if (bsetReg2() & V775_START_STOP_BIT) return v775::commonStop;
  else                                  return v775::commonStart;
}         

inline std::string v775::getMode()
{ return ( getCommon() == commonStop ) ? "CommonStop" : "CommonStart"; }

inline uint32_t v775::setLSB(uint32_t ps)
{
   uint32_t n   = uint32_t(0.5+8900.0/double(ps));
   fullScale(n);
   float lsb = 8.9/double(n);
   std::cout << "[V775]: fullscale(ns) = " << lsb*4096 << ", lsb(ns) = " << lsb << std::endl;
   return n;
}

inline uint32_t v775::setFullScale(uint32_t ns)
{
   uint32_t n   = uint32_t(0.5+(4096*8.9)/double(ns));
   fullScale(n);
   double lsb = 8.9/double(n);
   std::cout << "[V775]: fullscale(ns) = " << lsb*4096 << ", lsb(ns) = " << lsb << std::endl;
   return n;
}

inline uint32_t v775::getPicoSecPerCount()
{ return uint32_t(8900/fullScale()); }

 
inline bool v775::isDataReady() 
{ 
  uint16_t v = stReg1() & V775_DATA_READY_BIT;
  return v ;
}
 
inline bool v775::isBusy() 
{ 
  uint16_t v = stReg1() & V775_BUSY_BIT;
  return v ;
}
 
#endif
