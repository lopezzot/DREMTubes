extern "C" {
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>
#include <math.h>
#include <unistd.h>           // Needed for usleep
#include "errno.h"
}

/****************************************/
// C++ header files
/****************************************/
#include <iostream>
#include <fstream>
#include <signal.h>
#include <sys/time.h>


/****************************************/
// My header files with all classes
/****************************************/
#include "myVme.h"      // VME Base class
#include "myCorbo.h"    // Creative Electronic Systems RCB 8047 CORBO class derived from VME
#include "myModules.h"  //

//#define PROFILING

using namespace std;

/****************************************/
// Declare all the hardware
/****************************************/
corbo   corbo(0xfff000,"/dev/vmedrv24d16");

void unlockTrigger(){
  unsigned int tread;
  corbo.clearBusy(1);
  corbo.clearBusy(2);
  //usleep(10000);
  corbo.testTrigger(2);
}


unsigned short isTriggerPresent(){
  return corbo.getBusyState(1);
}


int main(int argc, char * argv[]){


  //trigger
  corbo.enableChannel(1);
  corbo.setInputFrontPanel(1);
  corbo.setBusyModeLevel(1);
  corbo.disableEventIRQ(1);

  //veto
  corbo.enableChannel(2);
  corbo.setInputInternal(2);
  corbo.setBusyModeLevel(2);
  corbo.setBusyOutputLocal(2);
  corbo.disableEventIRQ(2);
  corbo.testTrigger(2);
  unlockTrigger();

  while(1){
    if(isTriggerPresent())
      unlockTrigger();
  }

  return 0;
}
