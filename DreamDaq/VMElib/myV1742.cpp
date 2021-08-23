#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <cerrno>
#include <iostream>
#include <iomanip>

#include "myV1742.h"
#include "myModules.h"
#include "DGTZconfig.h"

using namespace std;

/*****************************************/
// Constructor
/*****************************************/
v1742::v1742(uint32_t base, const char* dev) // :vme(base, 0x2000, dev)
 {
  m_ev_size = 0;
  string dev_s(dev);
  uint32_t v(0);
  size_t spc = dev_s.find_last_of('/');
  if (spc)
   {
    dev_s.erase(0,spc+1);
    int32_t c = sscanf ( dev_s.c_str(), "%u", &v);
    if (c != 1) throw -2;
   }
  m_Link = v;
  m_Device = 0;

  m_id = base | ID_V1742;
  m_name = "CAEN V1742 DRS4 ADC";
  ErrorState = false;
  m_configured = false;
 }

v1742::v1742(uint32_t base, const char* dev, const char* cfgfile) // :vme(base, 0x2000, dev)
 {
  m_ev_size = 0;
  string dev_s(dev);
  uint32_t v(0);
  size_t spc = dev_s.find_last_of('/');
  if (spc)
   {
    dev_s.erase(0,spc+1);
    int32_t c = sscanf ( dev_s.c_str(), "%u", &v);
    if (c != 1) throw -2;
   }
  m_Link = v;
  m_Device = 0;

  m_id = base | ID_V1742;
  m_name = "CAEN DRS4 V1742 ADC";
  ErrorState = false;
  this->config(cfgfile);
  m_configured = true;
 }

void v1742::config(const char* cfgfile)
 {
  CAEN_DGTZ_ConnectionType linkType = CAEN_DGTZ_ConnectionType(1L); // CAEN_DGTZ_PCIE_OpticalLink;
  cout << "DRS4 v1742 0x" << hex << m_id << dec << " setting command file is: " << cfgfile << endl;
  uint32_t baseAddr = 0x0;
  //uint32_t baseAddr = 0x00320000;
  if (ParseConfigFile(cfgfile, &WDcfg))
   {
    cout << "ERROR parsing config file, V1742 not initialized";
    cout << "\n\nDRS initialization failed, aborting the run...";
    cout << "\e[0;31m\nPlease check the configuration file for syntax errors\e[0;30m\n" << endl;

    ErrorState=true;
    return;
   }
  buffSize = WDcfg.NumEvents * FULL_EV_SIZE;
  cout << " buffer size for v1742 is " << buffSize << endl;

  EventX742 = newEventV1742();

  int32_t ret = CAEN_DGTZ_OpenDigitizer(linkType, m_Link, m_Device, baseAddr, &handle);

  cout << "linkType = " << linkType; 
  cout << "\nlinkNum = " << m_Link; 
  cout << "\nconetNode = " << m_Device; 
  cout << "\nbaseAddr = " << hex << baseAddr << dec << endl;

  // ------------------------- !!!!!!!!1
  //   exit(-1);

  if (ret != 0)
   {
    cout << ">>> OpenDigitizer RC = " << ret;
    cout << "\n\nDRS initialization failed, aborting the run...";
    cout << "\e[0;31m\nTry to power-cycle the DRS crate before starting the next run\e[0;30m\n" << endl;
    ErrorState=true;
    return;
   }

  ret = CAEN_DGTZ_GetInfo(handle, &boardInfo);
  if (ret != 0)
   {
    cout << ">>> GetInfo RC = " << ret;
    cout << "\n\nDRS initialization failed, aborting the run...";
    cout << "\e[0;31m\nTry to power-cycle the DRS crate before starting the next run\e[0;30m\n" << endl;
    ErrorState=true;
    return;
   }

  cout << "Connected to CAEN Digitizer Model " << boardInfo.ModelName;
  cout << "\nROC FPGA Release is " << boardInfo.ROC_FirmwareRel;
  cout << "\nAMC FPGA Release is " << boardInfo.AMC_FirmwareRel;
  cout << "\nDigitizer handle " << handle << endl;

  ret = CAEN_DGTZ_Reset(handle);
  if (ret != 0)
   {
    cout << "CAEN_DGTZ_Reset Failed, ret = " << ret << endl;
    ErrorState=true;
    return;
   }

  for(int32_t i=0; i<8; i++)
   {
    ret = CAEN_DGTZ_SetChannelDCOffset(handle, i, WDcfg.DCoffset[i]);
    cout << "CAEN_DGTZ_SetChannelDCOffset(" << handle << ", " << i << ", " << hex << WDcfg.DCoffset[i] << dec << ")" << " = " << ret << endl;
   }

  /* execute generic write commands */
  for(int32_t i=0; i<WDcfg.GWn; i++)
   {
    ret = CAEN_DGTZ_WriteRegister(handle, WDcfg.GWaddr[i], WDcfg.GWdata[i]);
    cout << "CAEN_DGTZ_WriteRegister(" << handle << hex << " " << WDcfg.GWaddr[i] << ", " << WDcfg.GWdata[i] << dec << ")" << " = " << ret << endl;
   }

  ret = CAEN_DGTZ_SetPostTriggerSize(handle, WDcfg.PostTrigger);
  cout << "CAEN_DGTZ_SetPostTriggerSize(" << handle << ", " << hex << WDcfg.PostTrigger << dec << ") = " << ret << endl;
  ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle, WDcfg.NumEvents);
  cout << "CAEN_DGTZ_SetMaxNumEventsBLT(" << handle << ", " << WDcfg.NumEvents << ") = " << ret << endl;
  ret = CAEN_DGTZ_SetAcquisitionMode(handle, WDcfg.AcquisitionMode);
  cout << "CAEN_DGTZ_SetAcquisitionMode(" << handle << ", " << hex << WDcfg.AcquisitionMode << dec << ") = " << ret << endl;

/* done via WriteRegister
  uint32_t mask = 0;
  ret = CAEN_DGTZ_GetGroupEnableMask(handle, &mask);
  cout << "GroupEnableMask = " << hex << mask << dec << " ret = " << ret << endl;
  ret = CAEN_DGTZ_SetGroupEnableMask(handle, 0xf);
  cout << "CAEN_DGTZ_SetGroupEnableMask(" << handle << ", " << hex << 0xf << dec << ") = " << ret << endl;
  ret = CAEN_DGTZ_GetGroupEnableMask(handle, &mask);
  cout << "GroupEnableMask = " << hex << mask << dec << " ret = " << ret << endl;
 */

  uint32_t maskReg(0x8120);
  uint32_t maskVal(0);
  ret = CAEN_DGTZ_ReadRegister(handle, maskReg, &maskVal);
  cout << "maskReg is " << hex << maskVal << dec << endl;

#define SIZE_4_GROUPS 13836
#define SIZE_3_GROUPS 10378
#define SIZE_2_GROUPS 6920
#define SIZE_1_GROUPS 3462
#define SIZE_0_GROUPS 0

  uint32_t j=0;
  for (uint32_t i=0; i<4; i++)
   {
    if (maskVal&1) j++;
    maskVal>>=1;
   }
  m_ev_size = (j==4) ? SIZE_4_GROUPS : (j==3) ? SIZE_3_GROUPS : 
                   (j==2) ? SIZE_2_GROUPS : (j==1) ? SIZE_1_GROUPS : SIZE_0_GROUPS;
  cout << "# groups " << j << " size " << m_ev_size << endl;

  ret = CAEN_DGTZ_SWStartAcquisition(handle);
  cout << "CAEN_DGTZ_SetAcquisitionMode() = " << ret;
  cout << "\nAcquisition started" << endl;
  m_usleep(1000);
 }

v1742::~v1742()
 {
  if (!m_configured) return;
  cout << "v1742 leaving" << endl;
  dataReset();
  int32_t ret = CAEN_DGTZ_SWStopAcquisition(handle);
  cout << "CAEN_DGTZ_SWStopAcquisition returned " << ret << endl;
  if (ret != 0)
   {
    cout << "CAEN_DGTZ_SWStopAcquisition returned " << ret << endl;
    ErrorState=true;
   }
  ret = CAEN_DGTZ_CloseDigitizer(handle);
  cout << "CAEN_DGTZ_CloseDigitizer returned " << ret << endl;
  if (ret != 0)
   {
    cout << "CAEN_DGTZ_CloseDigitizer returned " << ret
         << "\nDigitizer has not been closed correctly, the next run will crash if you do not power-cycle the V1742" << endl;
    ErrorState=true;
   }

  destroyEventV1742(EventX742);

  cout << "v1742 done" << endl;
 }

