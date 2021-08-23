#ifndef _MY_V1742_H_
#define _MY_V1742_H_

/*****************************************

  myV1742.h
  ---------

  Definition of the CAEN V1742 digitizer class

  Version 2014 25.11.2014 roberto.ferrari@pv.infn.it

*****************************************/

#include <stdint.h>
#include <iostream>
#include <CAENDigitizer.h>

#include "myVme.h"
#include "WaveDump.h"
#include "eventX742.h"

#define DRS4_4G_DATA_SIZE 13836
#define DRS4_3G_DATA_SIZE 10378
#define DRS4_2G_DATA_SIZE 6920
#define DRS4_1G_DATA_SIZE 3462
#define DRS4_0G_DATA_SIZE 0

#define FULL_EV_SIZE 27680
#define SIZE_OF_V1742_HEADER 4

/*****************************************/
// The v1742 class 
/*****************************************/
class v1742 { // : public vme {

 public:

  // Constructor
  v1742(uint32_t base, const char* dev);    
  v1742(uint32_t base, const char* dev, const char* cfgfile);    
  ~v1742();

  void init() {}

  inline uint32_t id() { return m_id; }

  void config(const char* cfgfile);

  inline uint32_t __evsize()                  // event size (so far fixed)
   { return FULL_EV_SIZE / sizeof(uint32_t); }

  inline bool bufferEmpty()
   {
    uint32_t st0, st1, st2, st3;
    CAEN_DGTZ_ReadRegister(handle, 0x1088, &st0);
    CAEN_DGTZ_ReadRegister(handle, 0x1188, &st1);
    CAEN_DGTZ_ReadRegister(handle, 0x1288, &st2);
    CAEN_DGTZ_ReadRegister(handle, 0x1388, &st3);
    return (st0 & 2) && (st1 & 2) && (st2 & 2) && (st3 & 2);
   }

  inline bool bufferFull()
   {
    uint32_t st0, st1, st2, st3;
    CAEN_DGTZ_ReadRegister(handle, 0x1088, &st0);
    CAEN_DGTZ_ReadRegister(handle, 0x1188, &st1);
    CAEN_DGTZ_ReadRegister(handle, 0x1288, &st2);
    CAEN_DGTZ_ReadRegister(handle, 0x1388, &st3);
    return (st0 & 1) || (st1 & 1) || (st2 & 1) || (st3 & 1);
   }

  inline bool busy()
   {
    uint32_t st0, st1, st2, st3;
    CAEN_DGTZ_ReadRegister(handle, 0x1088, &st0);
    CAEN_DGTZ_ReadRegister(handle, 0x1188, &st1);
    CAEN_DGTZ_ReadRegister(handle, 0x1288, &st2);
    CAEN_DGTZ_ReadRegister(handle, 0x1388, &st3);
    return (st0 & 0x100) || (st1 & 0x100) || (st2 & 0x100) || (st3 & 0x100);
   }

  inline bool eventReady()
   {
    uint32_t st;
    CAEN_DGTZ_ReadRegister(handle, 0x8104, &st);
    return (st & 8);
   }

  inline void setReg(uint32_t reg, uint32_t val)
   {
    CAEN_DGTZ_WriteRegister(handle, reg, val);
   }

  inline uint32_t getReg(uint32_t reg)
   {
    uint32_t v;
    CAEN_DGTZ_ReadRegister(handle, reg, &v);
    return v;
   }

  inline bool getErrorState()
   {
    return ErrorState;
   }

  inline uint32_t readSingleEvent(uint32_t* buf)
   {
    uint32_t size(0);
    uint8_t* b = reinterpret_cast<uint8_t*>(buf);
    int32_t ret =
      CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
		 (char*)b, &size);
    if (ret != 0)
      std::cout << "CAEN_DGTZ_ReadData() Failed, ret = " << ret << std::endl;
    if (size == 0) return 0;
    uint32_t nev;
    CAEN_DGTZ_GetNumEvents(handle, (char*)b, size, &nev);
    if (nev != 1) std::cout << ">>>>>>>> Events in buffer: " << nev << std::endl;
    GetEventInfo(b, &eventInfo);
    return size/sizeof(uint32_t);
   }

  inline uint32_t dataReset()
   {
    int32_t ret(0);
    return ret;
   }

  inline uint32_t acqReset()
   {
    int32_t ret(0);
    ret = CAEN_DGTZ_SWStopAcquisition(handle);
    m_usleep(1000);
    ret = CAEN_DGTZ_SWStartAcquisition(handle);
    return ret;
   }

  inline uint32_t evSize() { return m_ev_size; }
 protected:
  
 private:
  WaveDumpConfig_t WDcfg;
  int32_t handle;
  CAEN_DGTZ_BoardInfo_t boardInfo;
  CAEN_DGTZ_EventInfo_t eventInfo;
  CAEN_DGTZ_V1742_EVENT_t * EventX742;

  uint32_t m_ev_size;

  uint32_t buffSize;
  uint32_t maxsz;

  bool ErrorState;
  bool m_configured;

  uint32_t m_id;        // module id
  std::string m_name;            // module name

  int16_t m_Link;
  int16_t m_Device;

};

#endif // _MY_V1742_H_
