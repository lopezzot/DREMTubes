//$Id: myV775.h,v 1.4 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myV1742.h
  ---------

  Definition of the CAEN V1742 digitizer
  This class is a class derived from the class vme.

*****************************************/

// Standard C header files
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <CAENDigitizer.h>
#include "WaveDump.h"
}

// C++ header files
#include <iostream>
#include <iomanip>

#include "myVme.h"
#include "myModules.h"

#include "DGTZconfig.h"
#include "eventX742.h"

#define FULL_EV_SIZE 27680

#define HEXWORD(m) "0x" << std::hex << std::setfill('0') << std::setw(8) << m << std::dec

using namespace std;

/*****************************************/
// The v1742 class 
/*****************************************/
class v1742 : public vme {

 public:

  // Constructor
  v1742(unsigned int base, const char *dev, const char *cfgfile);    
  ~v1742();

  // Crap
  void print();                                 // Simple dump function

  inline unsigned int id();                     // Module ID
  inline unsigned int evsize();                 // event size (so far fixed)

  inline bool bufferEmpty();
  inline bool bufferFull();
  inline bool busy();

  inline unsigned int readSingleEvent(unsigned int *buf);

 protected:
  
 private:
  WaveDumpConfig_t WDcfg;
  int handle;
  CAEN_DGTZ_BoardInfo_t boardInfo;
  CAEN_DGTZ_EventInfo_t eventInfo;
  CAEN_DGTZ_V1742_EVENT_t * EventX742;

  uint32_t buffSize;
  uint32_t maxsz;
};

/*****************************************/
// Generic Commands
/*****************************************/


inline unsigned int v1742::id() {return ba | ID_V1742;}
inline unsigned int v1742::evsize() {return FULL_EV_SIZE / sizeof(unsigned int);}


/*****************************************/
// Constructor
/*****************************************/
v1742::v1742(unsigned int base, const char *dev, const char *cfgfile):vme(base, 0x2000, dev)
  {
   cout << "Command file for v1742 setting is: " << cfgfile << endl;
   CAEN_DGTZ_ConnectionType linkType = CAEN_DGTZ_PCI_OpticalLink;
   int linkNum = 0;
   int conetNode = 0;
   uint32_t baseAddr = 0x0;
   //uint32_t baseAddr = 0x00320000;

   if (ParseConfigFile(cfgfile, &WDcfg)) throw "baidindi ...";

   buffSize = WDcfg.NumEvents * FULL_EV_SIZE;
   cout << " buffer size for v1742 is " << buffSize << std::endl;

   EventX742 = newEventV1742();

   int ret = CAEN_DGTZ_OpenDigitizer(linkType, linkNum, conetNode, baseAddr, &handle);

   cout << "linkType = " << linkType << endl; 
   cout << "linkNum = " << linkNum << endl; 
   cout << "conetNode = " << conetNode << endl; 
   cout << hex << "baseAddr = " << baseAddr << endl; 

   // ------------------------- !!!!!!!!1
   //   exit(-1);

   if (ret) {  
     cout << ">>> OpenDigitizer RC = " << ret << std::endl;
     throw "ta cazzu ses faendi?";
   }

   ret = CAEN_DGTZ_GetInfo(handle, &boardInfo);
   if (ret) {
     cout << ">>> GetInfo RC = " << ret << std::endl;
     throw "ta cazzu ses narendi?";
   }

   cout << "Connected to CAEN Digitizer Model " << boardInfo.ModelName << endl;
   cout << "ROC FPGA Release is " << boardInfo.ROC_FirmwareRel << endl;
   cout << "AMC FPGA Release is " << boardInfo.AMC_FirmwareRel << endl;
   cout << "Digitizer handle " << handle << endl;

   CAEN_DGTZ_Reset(handle);

   for(int i=0; i<8; i++)
     CAEN_DGTZ_SetChannelDCOffset(handle, i, WDcfg.DCoffset[i]);
   for(int i=0; i<8; i++)
     cout << "CAEN_DGTZ_SetChannelDCOffset(" << handle << ", " << i << ", " << hex << WDcfg.DCoffset[i] << dec << ")" << endl;

   /* execute generic write commands */
   for(int i=0; i<WDcfg.GWn; i++)
     CAEN_DGTZ_WriteRegister(handle, WDcfg.GWaddr[i], WDcfg.GWdata[i]);
   for(int i=0; i<WDcfg.GWn; i++)
     cout << "CAEN_DGTZ_WriteRegister(" << handle << hex << WDcfg.GWaddr[i] << ", " << WDcfg.GWdata[i] << dec << ")" << endl;

   CAEN_DGTZ_SetPostTriggerSize(handle, WDcfg.PostTrigger);
   CAEN_DGTZ_SetMaxNumEventsBLT(handle, WDcfg.NumEvents);
   CAEN_DGTZ_SetAcquisitionMode(handle, WDcfg.AcquisitionMode);

   CAEN_DGTZ_SWStartAcquisition(handle);
   cout << "Acquisition started " << endl;
  }


v1742::~v1742()
  {
   cout << "v1742 leaving" << endl;

   CAEN_DGTZ_SWStopAcquisition(handle);
   CAEN_DGTZ_CloseDigitizer(handle);

   destroyEventV1742(EventX742);

   cout << "v1742 done" << endl;
  }

/*****************************************/
void v1742::print() 
/*****************************************/
{
  cout << "*************************************" << endl;  
  cout << "CAEN ADC V1742 Module / base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "*************************************" << endl;
  return;
}

bool v1742::bufferEmpty(){

  uint32_t st0, st1;

  CAEN_DGTZ_ReadRegister(handle, 0x1088, &st0);
  CAEN_DGTZ_ReadRegister(handle, 0x1188, &st1);

  return (st0 & 0x2) && (st1 & 0x2);
}

bool v1742::bufferFull(){
  
  uint32_t st0, st1;

  CAEN_DGTZ_ReadRegister(handle, 0x1088, &st0);
  CAEN_DGTZ_ReadRegister(handle, 0x1188, &st1);

  return (st0 & 0x1) || (st1 & 0x1);
}

bool v1742::busy(){

  uint32_t st0, st1;

  CAEN_DGTZ_ReadRegister(handle, 0x1088, &st0);
  CAEN_DGTZ_ReadRegister(handle, 0x1188, &st1);

  return (st0 & 0x80) || (st1 & 0x80);
}

unsigned int v1742::readSingleEvent(unsigned int *buf){

// cout << " handle " << handle << endl;
// cout << " buf " << buf << endl;
  uint32_t size = 0;
  uint8_t * b = reinterpret_cast<uint8_t *>(buf);
// cout << " b " << (void*)b << endl;
  CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
                     (char*)b, &size);
  if (size == 0) return 0;
// cout << " size " << size << endl;
  uint32_t nev;
  CAEN_DGTZ_GetNumEvents(handle, (char*)b, size, &nev);
  GetEventInfo(b, &eventInfo);
// if (size) cout << " nev " << nev << " size " << size << endl;
// if (size) cout << hex << " starting at " << (void*)b << " -> " << (void*)(b+size) << dec << endl;

  return size/sizeof(unsigned int);
}
