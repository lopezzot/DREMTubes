/*****************************************

  myVme.h
  -------

  Definition of the vme class.
  * for CAEN V2718 optical bridge *

  Version 2014 25.11.2014 roberto.ferrari@pv.infn.it

*****************************************/

#ifndef _MY_VME_H_
#define _MY_VME_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <iostream>
#include <string>
#include <map>

#ifndef LINUX
#define LINUX
#endif
#include <CAENVMElib.h>

/*****************************************/
// The class
/*****************************************/
class vme {

 public:

  vme(uint32_t base, uint32_t size, const char *dev);     // Constructor
  ~vme();                                                         // Destructor
  void print();                                                   // Simple dump function
  void init();

  inline uint32_t getBaseAddr() { return ba; }
  inline uint32_t id() { return m_id; }
  inline void setinfo(const char* name, uint32_t id)
   { m_name = std::string(name); m_id = id; }

  // Read/Write at module (local) physical address (base address subtracted)
  int32_t read8phys(uint32_t addr, volatile uint8_t* data);
  int32_t read16phys(uint32_t addr, volatile uint16_t* data);
  int32_t read32phys(uint32_t addr, volatile uint32_t* data);
  int32_t read32block(uint32_t addr, volatile uint8_t* buffer, int32_t howmany);
  int32_t write8phys(uint32_t addr, uint8_t data);
  int32_t write16phys(uint32_t addr, uint16_t data);
  int32_t write32phys(uint32_t addr, uint32_t data);
  inline int32_t getLength() { return length; }
  inline void * getBuffer() { return vbuf; }

  uint32_t getRegister(CVRegisters reg);
  uint32_t getInputRegister();
  uint32_t setOutputRegister(uint16_t data);
  uint32_t clearOutputRegister(uint16_t data);

  void eventCounterOffset (uint32_t ofs) { m_eventCounterOffset = ofs; }
  uint32_t eventCounterOffset() { return m_eventCounterOffset; }

 protected:

  uint32_t ba;          // Base Address
  int32_t fd;                   // File Descriptor for I/O port
  void *vbuf;               // Pointer to mmap space for this module
  int32_t length;               // Size of the memory mapping
  int32_t irqlev;               // VME Interrupt Level
  int32_t irqvec;               // VME Interrupt Vector
  uint32_t m_id;        // module id
  std::string m_name;            // module name

  CVAddressModifier am;     // Addressing Mode
  CVBoardTypes  m_VMEBoard;
  int16_t       m_Link;
  int16_t       m_Device;
  int32_t       m_BHandle;

  uint32_t      m_eventCounterOffset;

 private:
  static void caenInit( CVBoardTypes board, int16_t link, int16_t device, int32_t* handle);
  static void caenEnd( CVBoardTypes board, int16_t link, int16_t device, int32_t handle);
  static std::map<uint32_t,uint32_t> countModules;
  static std::map<uint32_t,int32_t> storeHandle;

  bool m_isCaen;
  bool m_isBit3;

};

/*****************************************/
// VME Read / Write Functions
/*****************************************/
inline int32_t vme::read8phys(uint32_t addr, volatile uint8_t* data) {
  volatile uint32_t t_data;
  uint32_t t_addr = ba + addr;
  CVErrorCodes ret = CAENVME_ReadCycle(m_BHandle,t_addr,(uint32_t*)&t_data,am,cvD8);
  if (ret != cvSuccess) exit(1);
  *data = t_data & 0xff;
  return 0;
}
inline int32_t vme::write8phys(uint32_t addr, uint8_t data) {
  volatile uint32_t t_data = data;
  uint32_t t_addr = ba + addr;
  CVErrorCodes ret = CAENVME_WriteCycle(m_BHandle,t_addr,(uint32_t*)&t_data,am,cvD8);
  if (ret != cvSuccess) exit(1);
  return 0;
}

inline int32_t vme::read16phys(uint32_t addr, volatile uint16_t* data) {
  volatile uint32_t t_data;
  uint32_t t_addr = ba + addr;
  CVErrorCodes ret = CAENVME_ReadCycle(m_BHandle,t_addr,(uint32_t*)&t_data,am,cvD16);
  if (ret != cvSuccess) exit(1);
  *data = t_data & 0xffff;
  return 0;
}
inline int32_t vme::write16phys(uint32_t addr, uint16_t data) {
  volatile uint32_t t_data = data;
  uint32_t t_addr = ba + addr;
  CVErrorCodes ret = CAENVME_WriteCycle(m_BHandle,t_addr,(uint32_t*)&t_data,am,cvD16);
  if (ret != cvSuccess) exit(1);
  return 0;
}

inline int32_t vme::read32block(uint32_t addr, volatile uint8_t* buffer, int32_t howmany) {
  uint32_t t_addr = ba + addr;
  int32_t expSize = howmany * 4;
  int32_t gotSize;
  CVErrorCodes ret = CAENVME_BLTReadCycle(m_BHandle,t_addr,(void*)buffer,expSize,am,cvD32,&gotSize);
  if (gotSize != expSize) std::cout << " vme::read32block " << std::hex << t_addr << std::dec << " exp " << expSize << " got " << gotSize << std::endl;
  if (ret != cvSuccess) exit(1);
  return gotSize;
}

inline int32_t vme::read32phys(uint32_t addr, volatile uint32_t* data) {
  volatile uint32_t t_data;
  uint32_t t_addr = ba + addr;
  CVErrorCodes ret = CAENVME_ReadCycle(m_BHandle,t_addr,(uint32_t*)&t_data,am,cvD32);
  if (ret != cvSuccess) exit(1);
  *data = t_data;
  return 0;
}
inline int32_t vme::write32phys(uint32_t addr, uint32_t data) {
  volatile uint32_t t_data = data;
  uint32_t t_addr = ba + addr;
  CVErrorCodes ret = CAENVME_WriteCycle(m_BHandle,t_addr,(uint32_t*)&t_data,am,cvD32);
  if (ret != cvSuccess) exit(1);
  return 0;
}

inline uint32_t vme::getRegister(CVRegisters reg) {
  volatile uint32_t data;
  CAENVME_ReadRegister(m_BHandle, reg, (uint32_t*)&data);
  return data;
}

inline uint32_t vme::getInputRegister() {
  volatile uint32_t data;
  CAENVME_ReadRegister(m_BHandle, cvInputReg, (uint32_t*)&data);
  return data;
}

inline uint32_t vme::setOutputRegister(uint16_t data) {
  CAENVME_WriteRegister(m_BHandle, cvOutRegSet, data);
  return data;
}

inline uint32_t vme::clearOutputRegister(uint16_t data) {
  CAENVME_WriteRegister(m_BHandle, cvOutRegClear, data);
  return data;
}

inline void m_usleep(int32_t usec)
 {
  /*
  struct timeval t0, t1;
  double dt;
  gettimeofday(&t0, NULL);
  do
   {
    sched_yield();
    gettimeofday(&t1, NULL);
    double ds = double(t1.tv_sec - t0.tv_sec);
    double dus = double(t1.tv_usec - t0.tv_usec);
    dt = ds * 1000000.0f + dus;
   }
  while(dt<double(usec));
  */
  struct timespec tslp={0,usec*1000};
  nanosleep(&tslp,NULL);
  return;
 }

#endif // _MY_VME_H_
