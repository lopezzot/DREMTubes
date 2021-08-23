
#include <iostream>

#include "myV262.h"

// v262 xio(0x400000,"/Bit3/a24d16/1");
v262 xio(0x400000,"/V2718/cvA24_U_DATA/1");
v262 zio(0x400000,"/V2718/cvA24_U_DATA/1");

// v262 zio(0x400000,"/dev/vmedrvb24d16");

using namespace std;

int32_t main( int32_t argc, char** argv )
 {
  int16_t reg;
  if (argc != 1)
   {
    reg = atoi(argv[1]);
    if ((reg >= 0) && (reg <4))
     {
      zio.enableNIMOut(reg);
      sleep(1);
      zio.disableNIMOut(reg);
     }
    return 0;
   }
do {
  uint16_t w;
  zio.read16phys(0xA, &w);
  cout << hex << " V262CAEN " << w << endl;
  uint16_t z;
sleep(1);
  xio.read16phys(0xA, &z);
  cout << hex << " V262 " << z << endl;
sleep(1);
  zio.read16phys(0xA, &w);
  cout << hex << " V262CAEN " << w << endl;
sleep(1);
  xio.read16phys(0xA, &z);
  cout << hex << " V262 " << z << endl;
sleep(1);
  zio.read16phys(0xA, &w);
  cout << hex << " V262CAEN " << w << endl;
sleep(1);
  xio.read16phys(0xA, &z);
  cout << hex << " V262 " << z << endl;
sleep(1);
  uint16_t physT1T2 = zio.getNIMIn(0);
  cout << hex << " V262CAEN physT1T2 " << physT1T2 << endl;
sleep(1);
  uint16_t xphysT1T2 = xio.getNIMIn(0);
  cout << hex << " V262 physT1T2 " << xphysT1T2 << endl;
sleep(1);
  uint16_t busy = zio.getNIMIn(1);
  cout << hex << " V262CAEN busy " << busy << endl;
sleep(1);
  uint16_t xbusy = xio.getNIMIn(1);
  cout << hex << " V262 busy " << xbusy << endl;
sleep(1);
  uint16_t isSpill = zio.getNIMIn(2);
  cout << hex << " V262CAEN isSpill " << isSpill << endl;
sleep(1);
  uint16_t xisSpill = xio.getNIMIn(2);
  cout << hex << " V262 isSpill " << xisSpill << endl;
sleep(1);
  uint16_t ped = zio.getNIMIn(3);
  cout << hex << " V262CAEN ped " << ped << endl;
sleep(1);
  uint16_t xped = xio.getNIMIn(3);
  cout << hex << " V262 ped " << xped << endl;
} while(1);

  return 0;
 }
