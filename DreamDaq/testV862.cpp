
#include <iostream>

#include "myV862AC.h"

using namespace std;

uint32_t addresses[5]={0x04000000,0x05000000,0x06000000,0x07000000,0x03000000};

void initV792( v792ac& v792adc )
 {
  v792adc.print();
  v792adc.reset();
  v792adc.disableSlide();
  v792adc.disableOverflowSupp();
  v792adc.disableZeroSupp();
  v792adc.clearEventCounter();
  v792adc.clearData();
  uint16_t ped;
  ped = v792adc.getI1();
  uint32_t bid = v792adc.id();
  cout << "v792ac addr 0x" << hex << bid << dec
       << ": default ped I1 value is " << ped << endl;

  ped = 255;                  // >~ minimum possible Iped (see manual par. 2.1.2.1)
  v792adc.setI1(ped);       // set I1 current to define pedestal position
  ped =v792adc.getI1();     // in the range [0x00-0xFF] (see manual par. 4.34.2)
  uint16_t thr = 0x00;
  v792adc.setChannelThreshold(thr);
  cout << "v792ac addr 0x" << hex << bid << dec << ": now ped  I1 value is " << ped << " thr " << thr << endl;

  v792adc.enableChannels();

  uint16_t reg1 = v792adc.getStatus1();
  uint16_t reg2 = v792adc.getControl1();
  uint16_t reg3 = v792adc.getStatus2();
  uint16_t reg4 = v792adc.getBit1();
  uint16_t reg5 = v792adc.getBit2();
  uint32_t evc = v792adc.eventCounter();
  cout << "v792ac addr 0x" << hex << bid << " status 1 0x" << reg1
       << " control 1 0x" << reg2 << " status 2 0x" << reg3 << " bit 1 0x" << reg4
       << " bit 2 0x" << reg5 << dec << endl;
  cout << "v792ac event counter " << evc << endl;
 }

int32_t main (int32_t argc, char** argv)
 {
  uint32_t addr;
  if (argc==1)
    addr=0x03000000;
  else
   {
    int32_t i = atoi(argv[1]);
    addr = (i<5) ? addresses[i] : 0x03000000;
   }
    
  v862ac adc4(addr, "/V2718/cvA32_U_DATA/0");

  initV792 (adc4);

  cout << "busy " << adc4.busy();
  cout << "\ndready " << adc4.dready();
  cout << "\nbufferEmpty " << adc4.eventBufferEmpty();
  cout << "\nevent counter " << adc4.eventCounter() << endl;
  return 0;
 }
