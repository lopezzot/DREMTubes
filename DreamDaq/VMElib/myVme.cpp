
#include <iostream>

#include "myVme.h"

using namespace std;

map<uint32_t,uint32_t> vme::countModules;
map<uint32_t,int32_t> vme::storeHandle;

void vme::caenInit( CVBoardTypes board, int16_t link, int16_t device, int32_t* handle)
 {
  uint32_t code=(board<<16) | (link<<8) | device;
  if (countModules.count(code) == 0)
   {
    // Initialize the Board
    cout << " opening board " << board << " link " << link << " device " << device << endl;
    if( CAENVME_Init(board, link, device, handle) != cvSuccess )
     {
      cout << "\n\n Error opening the device" << endl;
      exit(1);
     }
    storeHandle[code] = *handle;
    char swrel[256];
    CAENVME_SWRelease(swrel);
    cout << "CAEN V2718 swrel " << swrel << endl;
    CAENVME_BoardFWRelease(*handle, swrel);
    cout << "CAEN V2718 fwrel " << swrel << endl;
    // set output lines
    CAENVME_SetOutputConf(*handle, cvOutput0, cvDirect, cvActiveHigh, cvManualSW);
    CAENVME_SetOutputConf(*handle, cvOutput1, cvDirect, cvActiveHigh, cvManualSW);
    CAENVME_SetOutputConf(*handle, cvOutput2, cvDirect, cvActiveHigh, cvManualSW);
    CAENVME_SetOutputConf(*handle, cvOutput3, cvDirect, cvActiveHigh, cvManualSW);
    CAENVME_SetOutputConf(*handle, cvOutput4, cvDirect, cvActiveHigh, cvManualSW);
    // set input lines
    CAENVME_SetInputConf(*handle, cvInput0, cvDirect, cvActiveHigh);
    CAENVME_SetInputConf(*handle, cvInput1, cvDirect, cvActiveHigh);
   }
  countModules[code] ++;
  *handle = storeHandle[code];
  return;
 }

void vme::caenEnd( CVBoardTypes board, int16_t link, int16_t device, int32_t handle)
 {
  uint32_t code=(board<<16) | (link<<8) | device;
  if (countModules.count(code) != 0)
   {
    countModules[code] --;
    if (countModules[code] == 0)
     {
      CAENVME_End(handle);
      countModules.erase(code);
     }
   }
  return;
 }

void vme::init()
 {
  return;
 }

/*****************************************/
// Constructor
/*****************************************/
#define IS_VME_AM(DEV,STR) (strstr(DEV,#STR ) != NULL) ? STR

static CVAddressModifier whichVmeAm( const char* dev )
 {
  return IS_VME_AM(dev, cvA16_S) :
       IS_VME_AM(dev, cvA16_U) :
       IS_VME_AM(dev, cvA16_LCK) :
       IS_VME_AM(dev, cvA24_S_BLT) :
       IS_VME_AM(dev, cvA24_S_PGM) :
       IS_VME_AM(dev, cvA24_S_DATA) :
       IS_VME_AM(dev, cvA24_S_MBLT) :
       IS_VME_AM(dev, cvA24_U_BLT) :
       IS_VME_AM(dev, cvA24_U_PGM) :
       IS_VME_AM(dev, cvA24_U_DATA) :
       IS_VME_AM(dev, cvA24_U_MBLT) :
       IS_VME_AM(dev, cvA24_LCK) :
       IS_VME_AM(dev, cvA32_S_BLT) :
       IS_VME_AM(dev, cvA32_S_PGM) :
       IS_VME_AM(dev, cvA32_S_DATA) :
       IS_VME_AM(dev, cvA32_S_MBLT) :
       IS_VME_AM(dev, cvA32_U_BLT) :
       IS_VME_AM(dev, cvA32_U_PGM) :
       IS_VME_AM(dev, cvA32_U_DATA) :
       IS_VME_AM(dev, cvA32_U_MBLT) :
       IS_VME_AM(dev, cvA32_LCK) :
       IS_VME_AM(dev, cvCR_CSR) :
       IS_VME_AM(dev, cvA40_BLT) :
       IS_VME_AM(dev, cvA40_LCK) :
       IS_VME_AM(dev, cvA40) :
       IS_VME_AM(dev, cvA64) :
       IS_VME_AM(dev, cvA64_BLT) :
       IS_VME_AM(dev, cvA64_MBLT) :
       IS_VME_AM(dev, cvA64_LCK) :
       IS_VME_AM(dev, cvA3U_2eVME) :
       IS_VME_AM(dev, cvA6U_2eVME) : cvA40;
 }

vme::vme(uint32_t base, uint32_t size, const char* dev)
{
  string dev_s;
  if ((strstr(dev, "/v2718/") != NULL) || (strstr(dev, "/V2718/") != NULL))
   {
    m_isCaen = true;
    m_isBit3 = false;
    dev_s = string(dev+7);
   }
  else if ((strstr(dev, "/bit3/") != NULL) || (strstr(dev, "/Bit3/") != NULL))
   {
    m_isCaen = false;
    m_isBit3 = true;
    dev_s = string(dev+6);
   }
  else
   {
    m_isCaen = false;
    m_isBit3 = false;
    throw -1;
   }

  m_VMEBoard = cvV2718;

  am = whichVmeAm(dev_s.c_str());

  uint32_t v(0);
  size_t spc = dev_s.find_last_of('/');
  if (spc)
   {
    dev_s.erase(0,spc+1);
    int32_t c = sscanf ( dev_s.c_str(), "%u", &v);
    if (c != 1) throw -2;
   }
  m_Link = v;

  // m_Link = 1;
  // m_Link = 0;
  m_Device = 0;

  cout << "vme::vme addr 0x" << hex << base << " device " << dev << " link 0x" << m_Link << " vme am 0x" << am << dec << endl;

  // Initialize the Board
  vme::caenInit(m_VMEBoard, m_Link, m_Device, &m_BHandle);

  ba = base;
  length = size;
  irqlev = 0;
  irqvec = 0;
  m_id = 0;
  m_name = "";
  vbuf = NULL;

  m_eventCounterOffset = 0;

  return;
}

/*****************************************/
// Destructor
/*****************************************/
vme::~vme()
{
  vme::caenEnd(m_VMEBoard, m_Link, m_Device, m_BHandle);
  return;
}

/*****************************************/
// Dump Function
/*****************************************/
void vme::print() 
{
  cout << "*************************************\n";  
  cout << m_name << " Caen VME Module / base " << hex << ba << dec << ", length " << length;
  cout << "\n*************************************" << endl;
  return;
}

