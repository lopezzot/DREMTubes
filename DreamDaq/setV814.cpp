
#include <stdint.h>
#include <iostream>
#include "myV814.h"

using namespace std;

v814 dshort(0xCC0000,"/V2718/cvA24_U_DATA/1");

int32_t main( int32_t argc, char** argv )
 {
  uint8_t thr(10); 
  uint8_t wid(200); 
  if (argc > 1) thr = atoi(argv[1]);
  if (argc > 2) wid = atoi(argv[2]);
  cout << "Setting v814 threshold " << uint16_t(thr) << " and width " << uint16_t(wid) << endl;
  dshort.setThrAll(thr);
  dshort.setWidthAll(wid);
  return 0;
 }
