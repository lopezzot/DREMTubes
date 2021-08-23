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
#include "myV262.h"     // CAEN V262 I/O REG
#include "myV513.h"     // CAEN V513 I/O REG

/****************************************/
// Declare all the hardware
/****************************************/

v262 io(0x400000,"/V2718/cvA24_U_DATA/1");

#define SET_IO_PORT(_port_nr_)    io.enableNIMOut(_port_nr_)
#define CLEAR_IO_PORT(_port_nr_)  io.disableNIMOut(_port_nr_)

inline void enableTriggers() { SET_IO_PORT(0); } // to enable triggers
inline void disableTriggers() { CLEAR_IO_PORT(0); } // to disable triggers

inline void enablePedTriggers() { SET_IO_PORT(3); } // to enable pedestal triggers
inline void disablePedTriggers() { CLEAR_IO_PORT(3); } // to disable pedestal triggers

inline void send_signal() { io.NIMPulse(2); };
inline void unlockTrigger() { io.NIMPulse(0); /* io.NIMPulse(3); */ } // to re-enable the trigger logic that goes auto-vetoed after any trigger
inline void resetNIMScaler() { io.NIMPulse(1); } // to reset the NIM scaler

/****************************************/
int32_t main(int32_t argc, char** argv)
/****************************************/
{
  (argc > 1) ? enableTriggers() : disableTriggers();
  if (argc > 2) unlockTrigger();
  return 0;
}
