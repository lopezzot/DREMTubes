
#include <stdint.h>

#include <iostream>

#include "myV258.h"

using namespace std;

int32_t main( int32_t argc, char** argv )
 {
  if (argc < 4) {
    cout << "\n Usage: " << argv[0] << " <VME addr> <chan> <thr>";
    cout << "\n Example: " << argv[0] << " 0xEE0000 7 100";
    cout << "\n to set channel 7 of board at address 0xEE0000 to 100 mV threshold" << endl;
    return 1;
  }
  uint32_t addr(0);
  int32_t n = sscanf(argv[1], "%x", &addr);
  if (n != 1)
   {
    cout << " invalid format for VME address: " << argv[1] << endl;
    return 2;
   }
  cout << " addr " << hex << addr << dec << endl;
  v258 dboard(addr,"/dev/vmedrvb24d16");
  argc -= 2;
  argv += 2;
  uint16_t chn(0);
  uint16_t thr(0);
  for (int32_t j=0; j<argc-1; j+=2)
   {
    chn = atoi(argv[j]);
    thr = atoi(argv[j+1])/2;
    dboard.setThreshold(chn,thr);
   }

  return 0;
 }
