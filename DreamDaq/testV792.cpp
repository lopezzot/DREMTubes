
#include <iostream>

#include "myV792AC.h"

using namespace std;

uint32_t addresses[5]={0x04000000,0x05000000,0x06000000,0x07000000,0x03000000};

int32_t main (int32_t argc, char** argv)
 {
  uint32_t addr;
  if (argc==1)
    addr=0x05000000;
  else
   {
    int32_t i = atoi(argv[1]);
    addr = (i<5) ? addresses[i] : 0x05000000;
   }
    
  cout << "instantiating V792ac " << hex << addr << dec << endl;
  v792ac adc4(addr, "/V2718/cvA32_U_DATA/1");
  cout << "busy " << adc4.busy();
  cout << "\ndready " << adc4.dready();
  cout << "\nbufferEmpty " << adc4.eventBufferEmpty();
  cout << "\nevent counter " << adc4.eventCounter() << endl;
  return 0;
 }
