
#include <stdint.h>

#include "myV258.h"

v258 dshort(0xEE0000,"/V2718/cvA24_U_DATA");
v258 dlong(0xFF0000,"/V2718/cvA24_U_DATA");

int32_t main( int32_t argc, char** argv )
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

  CVRegisters reg;
  reg = cvStatusReg;
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  reg = cvInputReg;
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  reg = cvOutRegSet;
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;

  dshort.clearOutputRegister(0x7ff);
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  sleep(1);
  dshort.setOutputRegister(0x40);
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  sleep(1);
  dshort.clearOutputRegister(0x40);
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  sleep(1);
  dshort.setOutputRegister(0x80);
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  sleep(1);
  dshort.clearOutputRegister(0x80);
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  sleep(1);
  dshort.setOutputRegister(0x100);
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  sleep(1);
  dshort.clearOutputRegister(0x100);
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  sleep(1);
  dshort.setOutputRegister(0x200);
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  sleep(1);
  dshort.clearOutputRegister(0x200);
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  sleep(1);
  dshort.setOutputRegister(0x400);
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  sleep(1);
  dshort.clearOutputRegister(0x400);
  cout << hex << " reg " << reg << " val " << dshort.getRegister(reg) << dec << endl;
  sleep(1);

  uint16_t thre1 = atoi( envs );
  uint16_t thre2 = atoi( envl );
  cout << " threshold_1 " << thre1 << " threshold_2 " << thre2 << " (mV)" << endl;
  thre1 /= 2;
  thre2 /= 2;
  dshort.setThrAll(thre1);
  dlong.setThrAll(thre2);
  return 0;
 }
