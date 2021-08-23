
#include <stdint.h>

#include <iostream>

#include "myV258.h"

v258 dshort(0xEE0000,"/dev/vmedrvb24d16");
v258 dlong(0xFF0000,"/dev/vmedrvb24d16");

using namespace std;

int32_t main( int32_t , char** )
 {
  char* envs = getenv("THRESHOLD_SHORT");
  char* envl = getenv("THRESHOLD_LONG");
  

  if (envs == NULL) {
    cout << " Please setenv 'THRESHOLD_SHORT'" << endl;
    return 1;
  }
  if (envl == NULL) {
    cout << " Please setenv 'THRESHOLD_LONG'" << endl;
    return 2;
  }

  uint16_t thre1 = atoi( envs );
  uint16_t thre2 = atoi( envl );
  // cout << " threshold_1 " << thre1 << " threshold_2 " << thre2 << " (mV)" << endl;
  thre1 /= 2;
  thre2 /= 2;
  dshort.setThrAll(thre1);
  dlong.setThrAll(thre2);
  return 0;
 }
