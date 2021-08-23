/****************************************/
// Standard C header files
/****************************************/
extern "C" {
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>
#include <math.h>
#include <unistd.h>           // Needed for usleep
#include "myFIFO-IO.h"            // Needed for my I/O stuff
#include "myBar.h"            // Needed for the progress bar
#include "myThread.h"    // Needed to read Slow Control Data
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
#include "myModules.h"  //
#include "myVme.h"      // VME Base class
#include "myL1176.h"    // LeCroy L1176 TDC

//#define PROFILING

using namespace std;

/****************************************/
// Declare all the hardware
/****************************************/
l1176 tdc0(0x300000,"/dev/vmedrvb24d32");

unsigned int * buf;
unsigned int * bufsize;
unsigned int events, skipped, pedevents;

unsigned int spills;
bool abort_run=false;

struct timespec rdtv0, rdtv1;
unsigned int rdtim;
unsigned int trigpipo;

void cntrl_c_handler(int sig)
{
  cout << endl << "Got control-c, end of current run" << endl << endl;
  abort_run = true;
}

void l1176Init(){
  //sets common stop mode
  cout << " INIT l1176 " << endl;
  tdc0.setCommStart();
  //Common start timeout
  tdc0.setCommStartTimeout(L1176_COMMSTART_TIMEOUT_1);
  //enables rising edge sampling
  tdc0.enableRisingEdge();
  //disables rising edge sampling
  tdc0.disableFallingEdge();
  //sets output signal mode
  tdc0.setTriggerPulse();
  //enables data acquisition
  tdc0.enableAcq();
}

int myReadEvent(){
  
  unsigned int size=0;

  //resets internal buffer
  myNewEvent(false);
  
  
  myFormatSubEvent(tdc0.id());

  uint32_t * p = &buf[*bufsize];
  size=tdc0.readSingleEvent(&buf[*bufsize]);
  for (uint32_t i=0; i<size; i++) std::cout << " " << i << " val " << p[i];
  std::cout << std::endl;

  myUpdateSubEvent(size);

  return 0;
}

/****************************************/
int main()
/****************************************/
{
  buf = myBuffer(&bufsize);
  for (uint i=0; i<100; i++)
   {
    l1176Init();
    tdc0.clear();
    cout << "waiting " << endl;
    while (tdc0.evtNum() == 0) { cout << " looping " << endl; tdc0.clear(); sleep(1); }
    tdc0.disableAcq();
    myReadEvent();
   }
  return 0;
}
