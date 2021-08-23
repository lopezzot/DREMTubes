
#include <iostream>

#include "myV513.h"

// v513 xio(0xa00000,"/V2718/cvA24_U_DATA/1");
// v513 xio(0xa00000,"/V2718/cvA24_U_DATA/1");
v513 xio(0xa00000,"/V2718/cvA24_U_DATA/0");   // Desy setup // roberto 15.06.2021

using namespace std;
inline bool isT1T2Trig() { return (xio.readInputRegister() & 1<<7); }
inline bool isHWBusy() { return (xio.readInputRegister() & 1<<4); }
inline bool isInSpill() { return (xio.readInputRegister() & 1<<5); }
inline bool isPedTrig() { return (xio.readInputRegister() & 1<<6); }

int32_t main( int32_t , char** )
 {
  uint16_t z;
  xio.read16phys(0x04, &z);
  cout << hex << " V513 0x4 " << z << endl;
  cout << "isT1T2Trig: " << isT1T2Trig() << " - isPedTrig: " << isPedTrig() << " - isInSpill " << isInSpill() << " - isHWBusy " << isHWBusy() << endl;
 // exit(0);

sleep(1);
do
 {
  uint16_t w;
  xio.read16phys(0xFE, &w);
  cout << hex << " V513 FE " << w << endl;
sleep(1);
  xio.read16phys(0xFC, &w);
  cout << hex << " V513 FC " << w << endl;
sleep(1);
  xio.read16phys(0xFA, &w);
  cout << hex << " V513 FA " << w << endl;
usleep(1);
  w=0;
  xio.write16phys(0x48, w);
usleep(1);
  xio.write16phys(0x46, w);
usleep(1);
  xio.write16phys(0x42, w);
usleep(1);

  xio.read16phys(0x04, &w);
  cout << hex << " V513 0x4 " << w << endl;
usleep(1);

  for (int i=0; i<8; i++)
   {
    int reg07 = 0x10+i*2;
    // xio.write16phys(reg07, 7);
    xio.setChannelInput(i);
usleep(1);
    xio.read16phys(reg07, &w);
    cout << hex << " V513 " << reg07 << " " << w << " ------- ";
usleep(1);
    int reg8f = 0x20+i*2;
    // xio.write16phys(reg8f, 6);
    xio.setChannelOutput(8+i);
usleep(1);
    xio.read16phys(reg8f, &w);
    cout << hex << " V513 " << reg8f << " " << w << endl;
usleep(1);
   }

  xio.setOutputMask(0xf000);
usleep(10);
  xio.clearOutputMask(0xf000);
usleep(10);
  xio.setOutputMask(0x0f00);
usleep(10);
  xio.clearOutputMask(0x0f00);
usleep(10);
 }
while(1);
  return 0;
 }
