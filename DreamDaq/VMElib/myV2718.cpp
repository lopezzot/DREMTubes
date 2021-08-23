#include <cstdio>
#include <string>
#include <iostream>

#include "myModules.h"

#include "myV2718.h"

using namespace std;

static uint64_t uw[4] = { 25, 1600, 410000, 104000000 };
static string us[4] = { "25 ns", "1600 ns", "410 us", "104 ms" };

uint32_t v2718::setPulserA( uint64_t period,  uint64_t width) {
  uint64_t u1 = period / 25;
  uint64_t u2 = period / 1600;
  uint64_t u3 = period / 410000;
  uint64_t u4 = period / 104000000;
  if (period < 25) { u1 = 2; width = 1; }
  if (u4 > 256) u4 = 255;
  CVTimeUnits u;
  uint8_t pe, wi;
  if (u3 > 256)
   {
    u = cvUnit104ms;
    pe = uint8_t(u4);
    wi = uint8_t(width/104000000);
   }
  else if (u2 > 256)
   {
    u = cvUnit410us;
    pe = uint8_t(u3);
    wi = uint8_t(width/410000);
   }
  else if (u1 > 256)
   {
    u = cvUnit1600ns;
    pe = uint8_t(u2);
    wi = uint8_t(width/1600);
   }
  else
   {
    u = cvUnit25ns;
    pe = uint8_t(u1);
    wi = uint8_t(width/25);
   }
  if (pe <= 1) pe = 2;
  if (wi >= pe) wi = pe/2;
  if (wi == 0) wi = 1;
  uint64_t rpe = pe * uw[u];
  uint64_t rwi = wi * uw[u];
  cout << "[Pulser A] unit  :  " << us[u] << endl;
  char q = (rpe >= 1000000000) ? ' '
          : (rpe >= 1000000) ? 'm'
          : (rpe >= 1000) ? 'u'
          : 'n';
  double dpe = (rpe >= 1000000000) ? rpe*1e-9
              : (rpe >= 1000000) ? rpe*1e-6
              : (rpe >= 1000) ? rpe*1e-3
              : rpe;
  cout << "           period:  " << dpe << ' ' << q << "sec" << endl;
  char r = (rwi >= 1000000000) ? ' '
          : (rwi >= 1000000) ? 'm'
          : (rwi >= 1000) ? 'u'
          : 'n';
  double dwi = (rwi >= 1000000000) ? rwi*1e-9
              : (rwi >= 1000000) ? rwi*1e-6
              : (rwi >= 1000) ? rwi*1e-3
              : rwi;
  cout << "           width :  " << dwi << ' ' << r << "sec" << endl;
  CAENVME_SetPulserConf(m_BHandle, cvPulserA, pe, wi, u, 0, cvManualSW, cvManualSW);
  CAENVME_WriteRegister(m_BHandle, cvOutMuxRegClear, 3);
  CAENVME_WriteRegister(m_BHandle, cvOutMuxRegSet, 2);
  uint32_t w;
  CAENVME_ReadRegister(m_BHandle, cvOutMuxRegSet, &w);
  cout << " OutMuxReg is 0x" << hex << w << dec << endl;
  CAENVME_StartPulser(m_BHandle, cvPulserA);
  return 0;
}

uint32_t v2718::setPulserB( uint64_t period,  uint64_t width) {
  uint64_t u1 = period / 25;
  uint64_t u2 = period / 1600;
  uint64_t u3 = period / 410000;
  uint64_t u4 = period / 104000000;
  if (period < 25) { u1 = 2; width = 1; }
  if (u4 > 256) u4 = 255;
  CVTimeUnits u;
  uint8_t pe, wi;
  if (u3 > 256)
   {
    u = cvUnit104ms;
    pe = uint8_t(u4);
    wi = uint8_t(width/104000000);
   }
  else if (u2 > 256)
   {
    u = cvUnit410us;
    pe = uint8_t(u3);
    wi = uint8_t(width/410000);
   }
  else if (u1 > 256)
   {
    u = cvUnit1600ns;
    pe = uint8_t(u2);
    wi = uint8_t(width/1600);
   }
  else
   {
    u = cvUnit25ns;
    pe = uint8_t(u1);
    wi = uint8_t(width/25);
   }
  if (pe <= 1) pe = 2;
  if (wi >= pe) wi = pe/2;
  if (wi == 0) wi = 1;
  uint64_t rpe = pe * uw[u];
  uint64_t rwi = wi * uw[u];
  cout << "[Pulser B] unit  :  " << us[u] << endl;
  char q = (rpe >= 1000000000) ? ' '
          : (rpe >= 1000000) ? 'm'
          : (rpe >= 1000) ? 'u'
          : 'n';
  double dpe = (rpe >= 1000000000) ? rpe*1e-9
              : (rpe >= 1000000) ? rpe*1e-6
              : (rpe >= 1000) ? rpe*1e-3
              : rpe;
  cout << "           period:  " << dpe << ' ' << q << "sec" << endl;
  char r = (rwi >= 1000000000) ? ' '
          : (rwi >= 1000000) ? 'm'
          : (rwi >= 1000) ? 'u'
          : 'n';
  double dwi = (rwi >= 1000000000) ? rwi*1e-9
              : (rwi >= 1000000) ? rwi*1e-6
              : (rwi >= 1000) ? rwi*1e-3
              : rwi;
  cout << "           width :  " << dwi << ' ' << r << "sec" << endl;
  CAENVME_SetPulserConf(m_BHandle, cvPulserB, pe, wi, u, 0, cvManualSW, cvManualSW);
  CAENVME_WriteRegister(m_BHandle, cvOutMuxRegClear, 0x30);
  CAENVME_WriteRegister(m_BHandle, cvOutMuxRegSet, 0x20);
  uint32_t w;
  CAENVME_ReadRegister(m_BHandle, cvOutMuxRegSet, &w);
  cout << " OutMuxReg is 0x" << hex << w << dec << endl;
  CAENVME_StartPulser(m_BHandle, cvPulserB);
  return 0;
}

uint32_t v2718::resetPulserA() {
  CAENVME_StopPulser(m_BHandle, cvPulserA);
  return 0;
}

uint32_t v2718::resetPulserB() {
  CAENVME_StopPulser(m_BHandle, cvPulserB);
  return 0;
}

/*****************************************/
// Constructor
/*****************************************/
v2718::v2718(uint32_t base, const char* dev):vme(base, 0xFF, dev)

 {
  m_id = base | ID_V2718;
  m_name = "CAEN V2718 Controller";
 }
