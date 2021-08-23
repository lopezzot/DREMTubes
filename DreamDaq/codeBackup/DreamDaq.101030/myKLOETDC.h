/*****************************************

  myKLOETDC.h
  ---------

  Definition of the 32ch KLOE TDC class.
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

#define FIFO      (0x0)
#define EVRQ      (0x4)
#define GENS      (0x44)
#define MAIN      (0x8)
#define CREG      (0x48)
#define BECK      (0x88)
#define RSCK      (0xc8)
#define RANG      (0xc)
#define EVNU      (0x10)
#define CONT      (0x14)
#define MASKLS    (0x6*4)
#define MASKMS    (0x16*4)
#define EFLS(BUF) ((0x7+(BUF))*4)
#define EFMS(BUF) ((0x17+(BUF))*4)
#define EVRE(BUF) (((BUF)*0x10+0xb)*4)
#define STOP(BUF) (((BUF)*0x10+0xc)*4)
#define CLSB      (0x34)
#define REPT      (0x38)
#define CLAL      (0x3c)

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
// The kloetdc class 
/*****************************************/
class kloetdc : public vme {
  
 public:
  
  // Constructor
  kloetdc(unsigned int base, const char *dev);    
  
  
  //Generic Commands
  inline unsigned int id();                          // Module ID 
  //  inline void reset();                           // Module Reset

  // Working mode setup
  void  setMode(int nhits, int intck, int ckdis, int commstop, int smode,
                int tmode, int evnbits);         // Set main control
  unsigned int  getMode();                       // Get main control
  void  disClock();                              // Disable clock
  unsigned int  getTestMode();                   // Get test mode
  void  setTestMode();                           // Set test mode
  void  resetTestMode();                         // Reset test mode
  unsigned int  getEvNum();                      // Get event number
  void  setEvNum( unsigned int evNum);           // Set event number
  void  setClock160();                           // 160 MHz clock
  void  setClock480();                           // 480 MHz clock
  void  setClockExt();                           // External clock
  void  setClockAcq();                           // Acquisition mode
  unsigned int  getClockReg();                   // Get clock mode
  void  clearEvBuff();                           // Clear event buffer
  void  clearAll();                              // Clear all
  void  startDLL();                              // Start DLL
  void  initialise();

  int   readEvent(unsigned int * evbuff);        // Read event from hardware and stores it in user-provided buffer
  unsigned int  emptyFlags(int buff);            // read the status of the 32 FIFOs for a given event buffer
  unsigned int  stopCounts(int buff);            // read the status of the gray counter for a given event buffer
  inline unsigned int  internalStop();           // get internal stop status
  inline unsigned int  phase();                  // get counter phase
  inline unsigned int  noDataFlag();             // get NO-DATA flag
  inline unsigned int  writeBufP();              // get write event-buffer pointer
  inline unsigned int  readBufP();               // get read event-buffer pointer
  inline void  enaChan( unsigned int mask);      // enable channnels
  inline void  getTimeRange(unsigned int* countbits, unsigned int* depth);
  inline void  setTimeRange(unsigned int countbits, unsigned int depth);

  void  print();

 protected:

 private:
};


/**************************************************************/
//   Working mode setup
/**************************************************************/

void kloetdc::setMode(int nhits, int intck, int ckdis, int commstop, int smode, int tmode, int evnbits)
{
  unsigned int val;

  val = (nhits&0xF) | (intck&1)<<4 | (ckdis&1)<<5 | (commstop&1)<<6 |
      (smode&3)<<8 | (tmode&1)<<10 | (evnbits&0xF)<<12;

  write32phys(MAIN,val);
}

unsigned int kloetdc::getMode()
{
  unsigned int val;
  read32phys(MAIN,&val);
  return val;
}

void kloetdc::disClock()
{
  unsigned int val;
  read32phys(MAIN,&val);
  val |= 3<<4;
  write32phys(MAIN,val);
}

unsigned int kloetdc::getTestMode()
{
  unsigned int val;
  read32phys(MAIN,&val);
  return val & (1<<10);
}

void kloetdc::setTestMode()
{
  unsigned int val;
  read32phys(MAIN,&val);
  val |= (1<<10);
  write32phys(MAIN,val);
}

void kloetdc::resetTestMode()
{
  unsigned int val;
  read32phys(MAIN,&val);
  val &= ~(1<<10);
  write32phys(MAIN,val);
}

unsigned int  kloetdc::getEvNum()
{
  unsigned int val;
  read32phys(EVNU, &val);
  return val;
}

void  kloetdc::setEvNum( unsigned int evNum)
{
  unsigned int val, tmod;
  read32phys(MAIN,&tmod);
  val = tmod | (1<<10);
  write32phys(MAIN,val);
  write32phys(EVNU, evNum);
  write32phys(MAIN,tmod);
}

void  kloetdc::setClock160()
{
  unsigned int val, tmod;
  read32phys(MAIN,&tmod);
  val = tmod | (1<<10);
  write32phys(MAIN,val);
  write32phys(CREG,0x16<<10);
  tmod &= ~(3<<4);
  tmod |= (1<<4);
  write32phys(MAIN,tmod);
}

void  kloetdc::setClock480()
{
  unsigned int val, tmod;
  read32phys(MAIN,&tmod);
  val = tmod | (1<<10);
  write32phys(MAIN,val);
  write32phys(CREG,0x1e<<10);
  tmod &= ~(3<<4);
  tmod |= (1<<4);
  write32phys(MAIN,tmod);
}

void  kloetdc::setClockExt()
{
  unsigned int val, tmod;
  read32phys(MAIN,&tmod);
  val = tmod | (1<<10);
  write32phys(MAIN,val);
  write32phys(CREG,0x1c<<10);
  tmod &= ~(3<<4);
  write32phys(MAIN,tmod);
}

void  kloetdc::setClockAcq()
{
  unsigned int val, tmod;
  read32phys(MAIN,&tmod);
  val = tmod | (1<<10);
  write32phys(MAIN,val);
  write32phys(CREG,0);
  tmod &= ~(3<<4);
  tmod |= (1<<4);
  write32phys(MAIN,tmod);
}

unsigned int kloetdc::getClockReg()
{
  unsigned int val;
  read32phys(CREG, &val);
  return val;
}

void  kloetdc::clearEvBuff()
{
  unsigned int val, tmod;
  read32phys(MAIN,&tmod);
  val = tmod | (1<<10);
  write32phys(MAIN,val);
  write32phys(CLSB,0);
  write32phys(MAIN,tmod);
}

void  kloetdc::clearAll()
{
  write32phys(CLAL,0);
}

void  kloetdc::startDLL()
{
  unsigned int val, tmod;
  read32phys(MAIN,&tmod);
  val = tmod | (1<<10);
  write32phys(MAIN,val);
  write32phys(BECK,0);
  write32phys(MAIN,tmod);
}

void  kloetdc::initialise()
{
/*
  disClock();
  setTestMode();
  int nhits = 1;
  int intck = 1;
  int ckdis = 1;
  int commstop = 0;
  int smode = 3;
  int tmode = 1;
  int evnbits = 15;
  setMode(nhits, intck, ckdis, commstop, smode, tmode, evnbits);
cout << " MODE RESET " << endl;
  int  countbits = 0xb;
  int depth = 3;
  setTimeRange(countbits, depth);
cout << " TIME RANGE SET " << endl;
  setClockAcq();
  startDLL();
  clearAll();
  unsigned int mask = 0xffffffff;
  enaChan(mask);
cout << " ENABLED " << endl;
  
  nhits = 1;
  intck = 1;
  ckdis = 0;
  commstop = 0;
  smode = 3;
  tmode = 0;
  evnbits = 15;
  setMode(nhits, intck, ckdis, commstop, smode, tmode, evnbits);
*/
  unsigned int val;
  read32phys(CLAL,&val);
  write32phys(MAIN,0xf77f);
  write32phys(RANG,0x28);
  write32phys(BECK,0x0);
  write32phys(CLAL,0x0); 
  write32phys(MASKLS, 0x1);
  write32phys(MASKMS, 0x0);
  write32phys(CLAL,0x0);
  write32phys(MAIN,0xf35f);
}

/*****************************************/
// Generic Commands
/*****************************************/

inline unsigned int kloetdc::id() { return ba | ID_KLOETDC; }

//inline void v488::reset()    { write16phys(0x1C, 0x0); }

unsigned int  kloetdc::emptyFlags(int buff)
{
  unsigned int dms, dls;
  read32phys(EFMS(buff), &dms);
  read32phys(EFLS(buff), &dls);
  dms&=0xffff;
  dls&=0xffff;
  return (dms<<16)|dls;
}

unsigned int  kloetdc::stopCounts(int buff)
{
  unsigned int dls;
  read32phys(STOP(buff), &dls);
  return dls;
}

inline unsigned int  kloetdc::internalStop()
{ unsigned int i; read32phys(REPT, &i); return (i>>6)&1; }

inline unsigned int  kloetdc::phase()
{ unsigned int i; read32phys(REPT, &i); return (i>>5)&1; }

inline unsigned int  kloetdc::noDataFlag()
{ unsigned int i; read32phys(REPT, &i); return (i>>4)&1; }

inline unsigned int  kloetdc::writeBufP()
{ unsigned int i; read32phys(REPT, &i); return (i>>2)&3; }

inline unsigned int  kloetdc::readBufP()
{ unsigned int i; read32phys(REPT, &i); return i&3; }

inline void  kloetdc::enaChan( unsigned int mask)
{ write32phys(MASKLS, mask&0xffff); write32phys(MASKMS, (mask>>16)&0xffff); }

inline void  kloetdc::getTimeRange(unsigned int* countbits, unsigned int* depth)
{ unsigned int i; read32phys(RANG, &i); *countbits = i&0xf; *depth = (i>>4)&3; }

inline void  kloetdc::setTimeRange(unsigned int countbits, unsigned int depth)
{ unsigned int i; i = (depth<<4)|countbits; write32phys(EVNU, i); }

/*****************************************/
// Constructor
/*****************************************/
kloetdc::kloetdc(unsigned int base, const char *dev):vme(base, 0x100, dev) {};

/*****************************************/
void kloetdc::print() 
/*****************************************/
{
  cout << "************************************" << endl;  
  cout << " 32ch KLOE TDC   Module  /  base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "************************************" << endl;
  return;
}

int kloetdc::readEvent(unsigned* evbuff)
{
  unsigned int data;
  unsigned int sz;
  unsigned int dms, dls;


  int buff = 0;
  read32phys(EFMS(buff), &dms);
  read32phys(EFLS(buff), &dls);

  //usleep(100);

  sz = 0;
  dms&=0xffff;
  dls&=0xffff;
  cout << "dms: " <<hex << dms <<dec
       << " dls: " << hex << dls << dec << endl;
  if ((dms&dls) != 0xffff) do
  {
    read32phys(FIFO, &data);
    cout << hex<<data <<dec <<endl;
    *evbuff++ = data;
    sz ++;
  } while ((data&(1<<24)) != 0);
  write32phys(CLAL,0x0);
  
  return sz;
}

