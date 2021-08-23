
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <cerrno>
#include <iostream>
#include <iomanip>

#include "myV775.h"
#include "myModules.h"

using namespace std;

#define HEXWORD(m) "0x" << hex << setfill('0') << setw(8) << m << dec

/*****************************************/
// Constructor
/*****************************************/
v775::v775(uint32_t base, const char *dev):vme(base, 0x2000, dev)
 {
  m_id = base | ID_V775;
  m_name = "CAEN V775 TDC";
  m_eventcounter = 0;
 }

v775::~v775()
 {
 }

void v775::romPrint()
 {
  cout << hex;
  cout << "*************************************";  
  cout << "\n     oui msb     " << romMem(0x0026);
  cout << "\n     oui         " << romMem(0x002a);
  cout << "\n     oui lsb     " << romMem(0x002e);
  cout << "\n     version     " << romMem(0x0032);
  cout << "\n     id msb      " << romMem(0x0036);
  cout << "\n     id          " << romMem(0x003a);
  cout << "\n     id lsb      " << romMem(0x003e);
  cout << "\n     revision    " << romMem(0x004e);
  cout << "\n     serial msb  " << romMem(0x0f02);
  cout << "\n     serial lsb  " << romMem(0x0f06);
  cout << "\n*************************************";
  cout << dec << endl;
 }

void v775::printDataHdr(uint32_t v)
 {
  cout << "Header: " << HEXWORD(v) 
       << " | T : "  << dataDecodeType(v) 
       << " #ch "    << dataDecodeChCnt(v)  << endl;
 }

void v775::printDataBody(uint32_t v)
 {
  cout << "Data  : " << HEXWORD(v) 
       << " | T : "  << dataDecodeType(v) 
       << " | ch: "  << dataDecodeChannel(v) 
       << " dat "    << dataDecodeValue(v) << endl;
 }

void v775::printDataEnd(uint32_t v)
 {
  cout << "EndBlk: " << HEXWORD(v)
       << " | T : "  << dataDecodeType(v) 
       << " #ev "    << dataDecodeEvtCnt(v) << endl;
 }

uint32_t v775::readData(uint32_t*)
 {
  return 0;
 }

uint32_t v775::readSingleEvent(uint32_t *buf)
 {
  uint32_t tout(0);
  while (!this->dready()) { m_usleep(1); tout ++; if (tout > 100) return 0; }

  // cout << " 1 - v775::readSingleEvent isBusy " << isBusy() << " isDataReady() " << isDataReady() << endl;
  // while ( isBusy() ) {}
  // if ( !isDataReady() ) return 0;
  // cout << " 2 - v775::readSingleEvent isBusy " << isBusy() << " isDataReady() " << isDataReady() << endl;

  volatile uint32_t evc = this->evtCnt();
  // cerr << " [V775] " << hex << m_id << " data ready " << this->dready() << " ev counter " << dec << evc << endl;
  // if (evc == 0) return 0;
  if (evc == 0xffffff)
   { cerr << "[V775]: " << hex << m_id << " Not valid event counter (= 0xffffff)" << dec << endl; m_usleep(100000); return 0xffffffff; }
  volatile uint32_t val;
  uint32_t size=0;

  // Read the block header
  read32phys(0,&val);
  // printDataHdr(val);
  if ( !isWordValid(val) )
   { cerr << "[V775]: Not valid data" << endl; return 0xffffffff; }
  
  volatile uint32_t vol = val;
  volatile uint32_t chs = dataDecodeChCnt(vol);
  for(uint32_t i=0; i < chs; i++)
   { 
    read32phys(0,&val); // printDataBody(val);
    vol = val;
    *buf++ = vol; 
    size++;
   
    // Andrea 6/7/2011: replace dataDecodeType with dataDecodeValidity 
    if ( dataDecodeType(vol) != 0 )
     {
       cerr << "[V775]: data type " << dataDecodeType(val) << " not valid (should be 0)" << endl;
       size = 0;
       
     }
   }

  read32phys(0,&val);
  m_eventcounter++;

  return size;
 }

void v775::storeEmpty()
 {
  bsetReg2(V775_EMPTY_PROG_BIT);
 }

void v775::setTestMode()
 {
  dataReset();
  swReset();                      // 1. Software reset
  m_usleep(10);
  bsetReg2(V775_EMPTY_PROG_BIT);
  bsetReg2(V775_VALID_CTRL_BIT);  // 2.
  bsetReg2(V775_TEST_ACQ_BIT);    // 3. Reset the write pointer
  bclearReg2(V775_TEST_ACQ_BIT);  // 4. Reset the read ptr and release the write one
  int32_t num = 16;
  for(int32_t i=0; i<num; i++)        // 5. Write data
    write16phys(0x103E,0x0A00 + i);  
  bsetReg2(V775_TEST_ACQ_BIT);    // 6. Reset the write ptr and release the read one   
  cout << "Test mode: written " << num << " events in the test register" << endl;
 }

void v775::printRegisters()
 {
  cout <<   "[V775]: ID               : " << HEXWORD(id());
  cout << "\n[V775]: Register bitset1 : " << HEXWORD(bsetReg1());
  cout << "\n[V775]: Register bitset2 : " << HEXWORD(bsetReg2());
  cout << "\n[V775]: Register status1 : " << HEXWORD(stReg1());
  cout << "\n[V775]: Register status2 : " << HEXWORD(stReg2());
  cout << "\n[V775]: Register control1: " << HEXWORD(ctrlReg1());
  cout << "\n[V775]: Mode             : " << getMode();
  float lsb = 8.9/fullScale() ;
  cout << "\n[V775]: Full scale       : " << HEXWORD(fullScale()) 
                                        << " T(lsb/fullrange) = " << lsb
                                        << "/" << lsb*4000 << " ns";
  cout << "\n[V775]: Events:            " << evtCnt() << endl;
 }

