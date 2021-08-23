/*****************************************

  myReadOut.cpp
  -------------

  Dream DAQ program.

  Version 2014: 25/11/2014
  roberto.ferrari@pv.infn.it

*****************************************/

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdint.h>

#include <cmath>
#include <ctime>
#include <climits>
#include <cerrno>
#include <csignal>
#include <cstdlib>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

#include "myFIFO-IO.h"
#include "sigHandler.h"

#include "myModules.h"

#include "myVme.h"      // VME Base class
#include "myV513.h"     // CAEN V513 I/O REG

v513 ion(0xa00000,"/V2718/cvA24_U_DATA/1");

/****************************************/
int32_t main(int32_t argc, char** argv)
/****************************************/
{
  uint32_t nnn = ion.readInputRegister();
  cout << "InputRegister 0x" << hex << nnn << dec
       << " isT1T2Trig " << ((nnn&1)&&true)
       << " isHWBusy " << ((nnn&2)&&true)
       << " isInSpill " << ((nnn&4)&&true)
       << " isPedTrig " << ((nnn&8)&&true) << endl;
  while(1)
   {
    nnn = ion.readInputRegister();
    cout << "InputRegister() 0x" << hex << nnn << dec
         << " isT1T2Trig " << ((nnn&1)&&true)
         << " isHWBusy " << ((nnn&2)&&true)
         << " isInSpill " << ((nnn&4)&&true) << endl;
    sleep(1);
   }
  
  return 0;
}
