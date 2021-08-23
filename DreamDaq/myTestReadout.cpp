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

#define DRS4_FULL_EV_SIZE 13836
#define DRS4_HALF_EV_SIZE 6920

using namespace std;

bool run_closing(false);
uint32_t maxevts(0), maxpedevts(0), trignr(0), events(0), pedevents(0), doubletrig(0), totevents(0), spills(0), totprevious(0);
uint32_t bsum(0);
bool ped_enabled(true);
double phypedratio(0);

string tprint()
 {
  time_t tt(time(NULL));
  char tbuf[26];
  ctime_r( &tt, tbuf);
  tbuf[24] = '\0';
  return string(tbuf);
 }

time_t t0(0);

void set_t0()
 {
  struct timeval t1;
  gettimeofday(&t1, NULL);
  t0 = t1.tv_sec;
  return;
 }

double get_time()
 {
  double time_us;
  struct timeval t1;
  gettimeofday(&t1, NULL);
  t1.tv_sec -= t0;
  time_us = (t1.tv_sec) * 1000000.0f + t1.tv_usec;
  return time_us;
 }

time_t tlast(0);
time_t tdiff(0);
time_t period1(0), period2(0);
time_t spillStart(0), spillEnd(0), totDuration(0);
void loginfo(double occ=0, double pedocc=0)
 {
  uint32_t neededsp(0);
  if ((spills != 0) && (!run_closing))
   {
    time_t tnow(time(NULL));
    if (tlast != 0)
     {
      time_t p = tnow-tlast;
      if ((period1 != 0) && (period2 != 0) && (abs(period1-p)>1) && (abs(period2-p)>1))
       {
        cout << spills << " *** spill period was " << period1 << " " << period2 << " now is " << p << endl;
        period2 = period1;
        period1 = p;
       }
      else if (period1 == 0)
       {
        period1 = p;
       }
      else if (period2 == 0)
       {
        period2 = period1;
        period1 = p;
       }
     }
    tlast = tnow;
    if ((events) && (maxevts > events))
     {
      double ratio = maxevts/double(events)-1.;
      neededsp=uint32_t(ratio*spills+1);
     }
   }

// if (abort_run) break;
  time_t spillDuration = spillEnd-spillStart;
  totDuration += spillDuration;
  uint32_t takenNow = totevents-totprevious;
  cout << spills << " : " << takenNow
       << " Events taken -- Total " << totevents
       << " -- Triggers " << trignr
       << " -- Beam " << events
       << " ("<< occ*100 << "% occ.)"
       << " -- Pedestals " << pedevents
       << " ("<< pedocc*100 << "% occ.)"
       << " -- Both " << doubletrig
       << " -- End in ~ " << neededsp
       << " spills -- Spill length " << spillDuration << " sec -- Rate "
       << ((spillDuration > 0) ? takenNow/spillDuration : 0)
       << " Hz -- " + tprint() + '\n' << endl;
  totprevious = totevents;
  sync();
 }

/****************************************/
#include "sigHandler.h"

#include "myModules.h"

#include "myVme.h"      // VME Base class
#include "myV262.h"     // CAEN V262 I/O REG
#include "myV513.h"     // CAEN V513 I/O REG

//#define USE_DRS_V1742

/****************************************/
// Declare all the hardware
/****************************************/

v262 io(0x400000,"/V2718/cvA24_U_DATA/1");
v513 ion(0xa00000,"/V2718/cvA24_U_DATA/1");

/****************************************/

volatile bool abort_run=false;

uint32_t rdtim;

uint32_t evinfo[2]={0,0};
void cntrl_c_handler ( int32_t sig )
 {
  time_t timestr = time(NULL);
  char * stime = ctime(&timestr);
  stime[24] = 0;
  fprintf(stderr,"%s cntrl_c_handler: sig%d\n\n", stime, sig);
  fprintf(stderr,"aborting run\n");
  abort_run = true;
 }

double t_istrigger(1), t_rdoarm(0), t_readev(0), t_mtrigger(0), t_trghandler(0);
uint64_t n_loop(0);

int initV513( v513& ioreg )
 {
  uint16_t w;
  ioreg.read16phys(0xFE, &w);
  cout << hex << " V513 FE " << w << endl;
  usleep(1);
  ioreg.read16phys(0xFC, &w);
  cout << hex << " V513 FC " << w << endl;
  usleep(1);
  ioreg.read16phys(0xFA, &w);
  cout << hex << " V513 FA " << w << endl;
  usleep(1);
  w=0;
  ioreg.write16phys(0x48, w);
  usleep(1);
  ioreg.write16phys(0x46, w);
  usleep(1);
  ioreg.write16phys(0x42, w);
  usleep(1);

  ioreg.reset();
  usleep(1);

  ioreg.read16phys(0x04, &w);
  cout << hex << " V513 0x4 " << w << endl;
  usleep(1);

  for (int i=0; i<8; i++)
   {
    int reg = 0x10+i*2;
    ioreg.setChannelInput(i);
    usleep(1);
    ioreg.read16phys(reg, &w);
    cout << hex << " V513 " << reg << " " << w << endl;
    usleep(1);
   }

  for (int i=8; i<16; i++)
   {
    int reg = 0x10+i*2;
    ioreg.setChannelOutput(i);
    usleep(1);
    ioreg.read16phys(reg, &w);
    cout << hex << " V513 " << reg << " " << w << endl;
    usleep(1);
   }
  cout << dec;
  return 0;
 }

#define SET_IO_PORT(_port_nr_)    io.enableNIMOut(_port_nr_)
#define CLEAR_IO_PORT(_port_nr_)  io.disableNIMOut(_port_nr_)

inline void enableTriggers() { ion.clearOutputBit(15); } // { CLEAR_IO_PORT(0); } // to enable triggers clear (=0) out 0
inline void disableTriggers() { ion.setOutputBit(15); } // { SET_IO_PORT(0); } // to disable triggers set (=1) out 0

inline void enablePedTriggers() { ion.clearOutputBit(14); } // { CLEAR_IO_PORT(3); } // to enable pedestal triggers clear (=0) out 3
inline void disablePedTriggers() { ion.setOutputBit(14); } // { SET_IO_PORT(3); } // to disable pedestal triggers set (=1) out 3

inline void send_signal() { io.NIMPulse(2); };
inline void unlockTrigger() { io.NIMPulse(0); io.NIMPulse(3); } // to re-enable the trigger logic that goes auto-vetoed after any trigger
inline void resetNIMScaler() { io.NIMPulse(1); } // to reset the NIM scaler

inline bool isT1T2Trig() { return (ion.readInputRegister() & 1<<7); /* io.getNIMIn(0); */ }
inline bool isHWBusy() { return (ion.readInputRegister() & 1<<4); /* io.getNIMIn(1); */ } // return (v2718m.getInputRegister() & 1);
inline bool isInSpill() { return (ion.readInputRegister() & 1<<5); /* io.getNIMIn(2); */ }
inline bool isPedTrig() { return (ion.readInputRegister() & 1<<6); /* io.getNIMIn(3); */ }

uint32_t nSpill(0);
uint32_t nBeam(0);
uint32_t nPed(0);
uint32_t nTrig(0);
uint32_t nBoth(0);

int32_t getEvent (uint32_t isPhysEvent){
  
  double tref = get_time();

  while (isHWBusy() && !abort_run) {}

  if(abort_run) return 1;

  t_mtrigger += get_time()-tref;

  return 0;
}

bool isTriggerPresent (uint32_t * trigmask)
 {

  double tref = get_time(); 
  bool physT1T2 = isT1T2Trig();
  bool ped = isPedTrig();

  if (physT1T2 || ped) while (isHWBusy()) {}
  if (ped) physT1T2 = isT1T2Trig();
  if (ped || physT1T2) t_istrigger += get_time()-tref;

  *trigmask = (ped<<1) | physT1T2;

  return physT1T2 || ped;
 }

void handle_trigger (uint32_t trigmask,
		     uint32_t maxpedevts,
		     uint32_t * bsum,
		     bool * ped_enabled)
 {
    double tref = get_time();
do
 {
    if (!getEvent(trigmask & 0x1)) {
      
      if(trigmask & 1){
	events++;
        if (trigmask & 2) doubletrig++;
      }else{
	pedevents++;
	if(pedevents>=maxpedevts){
	  disablePedTriggers();
	  *ped_enabled=false;
	}
      }
      totevents ++;
    }
 }
while (0);

    t_trghandler += get_time()-tref;
 }

void check_and_handle_event()
 {
  uint32_t trigmask;
  double tref = get_time();
  if (isTriggerPresent(&trigmask)) {
	trignr ++;
	// send_signal();
	handle_trigger(trigmask, maxpedevts, &bsum, &ped_enabled);

	if (trigmask & 1) {
	  if (ped_enabled && (events>=phypedratio*pedevents)) enablePedTriggers();
	} else {
          if (events<phypedratio*pedevents) disablePedTriggers();
        }
	unlockTrigger();
	rdtim += get_time()-tref;
  } // if (isTriggerPresent(&trigmask))
 }

/****************************************/
int32_t main(int32_t argc, char** argv)
/****************************************/
{
int32_t runnr;
  struct tms tmsbuf;
  uint32_t ticks_per_sec;
  double time0, cputime, tottime, percent;

  time0 = times(&tmsbuf);
  ticks_per_sec = sysconf(_SC_CLK_TCK);

  signal(SIGINT, cntrl_c_handler);      // Control-C handler

  cout.precision(4);
   
  //Maximum number of events
  maxevts=10000000;
  phypedratio=10;
  double dmaxp = double(maxevts)/phypedratio;
  maxpedevts = lrint(dmaxp);
  cout << " maxevts " << maxevts << " maxpedevts " << maxpedevts << " phypedratio " << phypedratio << endl;

  int32_t xrunnr(0);
  while (--argc > 0)
   {
    ++argv;
    if ((strcmp(*argv, "-runnr") == 0) && (argc > 1))
     {
      --argc; ++argv;
      xrunnr = strtol(*argv, NULL, 10);
     }
   }
  runnr = xrunnr;
  if (runnr <= 0)
   {
    cerr << "****************** ERROR ****************** run number not found "
         << runnr << " exiting now\n" << endl;
    exit(1);
   }
  cout << tprint() << " Run " << runnr << " starting -> going to acquire " << maxevts << " events with trigger T1*T2*Vbar" << endl;

  //Init I/O Registers
  io.reset(); initV513( ion );

  //Set vetoes
  disableTriggers();
  disablePedTriggers();
  
  //Start of run setting up
  events=0;
  spills=0;

  unlockTrigger();

  resetNIMScaler();
  rdtim=0;

  cout << tprint() << " Waiting for a spill ... " << endl;
  //Wait for a spill before to start
  while(!isInSpill() && !abort_run) {}
  cout << "\n Got ... start data taking at " << tprint() << endl;
  tlast = time(NULL);

  //Remove the veto and start
  enableTriggers();
  spillStart = time(NULL);

  do { // daq loop ... exit on ctrl-c or when (events>=maxevts)

    check_and_handle_event();
    if ((events>=maxevts) || abort_run) break;

    if(!isInSpill()){
      disableTriggers();
      spillEnd = time(NULL);
      check_and_handle_event();
      spills++;
  
      loginfo();
      while(!isInSpill() && !abort_run) { m_usleep(10); }

      if (abort_run) break;

      enableTriggers();
      spillStart = time(NULL);
    } // if(!isInSpill())

  } while(1); // daq loop ... exit on ctrl-c or when (events>=maxevts)

  run_closing = true;
  cout << tprint() << " Ending run ... setting Trigger veto" << endl;
  disableTriggers();

  check_and_handle_event();

  spillStart = 0; spillEnd = totDuration; totprevious = 0;
  
  return 0;
}
