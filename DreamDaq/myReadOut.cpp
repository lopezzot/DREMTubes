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

class firstlastMessages
 {
  public:
   firstlastMessages() { set_t0(); cout << __FILE__ " compiled on " __DATE__ " " __TIME__ "\n**************************\n"; loginfo(); cout << "DREAM RUN STARTING" << endl; }
   ~firstlastMessages() { totprevious = 0; cout << "DREAM RUN CLOSED\n"; loginfo(); cout << "**************************\n" << endl; }
 };

firstlastMessages _firstlastMsgs;

/****************************************/
#include "myFIFO-IO.h"
#include "sigHandler.h"

#include "myModules.h"

#include "myVme.h"      // VME Base class
#include "myV792AC.h" // CAEN ADC V792AC class derived from VME
#include "myV262.h"     // CAEN V262 I/O REG
#include "myV513.h"     // CAEN V513 I/O REG
#include "myV2718.h"
#include "myV775.h"
#include "myV862AC.h"

//#define USE_DRS_V1742

/****************************************/
// Declare all the hardware
/****************************************/

#ifdef USE_DRS_V1742
  #include "myV1742.h"
  v1742 DRS4(0x0032000, "/V2718/cvA32_U_DATA/2", "/home/dreamtest/working/drs4command.list");
  uint32_t drs4_ev_size(0);
#endif

// ADC V792
v792ac adc0(0x04000000,"/V2718/cvA32_U_DATA/0");
v792ac adc1(0x05000000,"/V2718/cvA32_U_DATA/0");
v792ac adc2(0x06000000,"/V2718/cvA32_U_DATA/0");
v792ac adc3(0x07000000,"/V2718/cvA32_U_DATA/0");
// ADC V862 - identical to V792 from SW point of view
v862ac adc4(0x03000000,"/V2718/cvA32_U_DATA/0");

uint32_t n_v792 = 5;
v792ac* v792s[] = {&adc4,&adc3,&adc2,&adc1,&adc0};

v775 ntdc(0x08000000,"/V2718/cvA32_U_DATA/0");

v262 io(0x400000,"/V2718/cvA24_U_DATA/1");
v513 ion(0xa00000,"/V2718/cvA24_U_DATA/1");

v2718 v2718_1(0,"/V2718/cvA24_U_DATA/0");
v2718 v2718_2(0,"/V2718/cvA24_U_DATA/1");

/****************************************/

uint32_t * buf;
uint32_t * bufsize;
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

bool xheckEvntCntrs()
 {
  bool all_fine;
  bool ngates(true);
  uint32_t ndaq = totevents;
  uint32_t nev775 = ntdc.evtCnt()+1;

  all_fine = (ndaq == nev775);

  uint32_t nev792[n_v792];
  for(size_t i=0; i < n_v792; ++i)
  // for(size_t i=0; i < 1; ++i)      // PATCH FOR DEBUG
   {
    nev792[i] = v792s[i]->eventCounter() + 1;
    all_fine = all_fine && (ndaq == nev792[i]);
    ngates = ngates && (nev775 == nev792[i]);
   }

  if (!ngates)
   {
    cout << " ****** PLEASE STOP RUN NOW ****** MISMATCH IN THE NUMBER OF ADC GATES ****** ndaq: " << ndaq << " ****** nev775: " << nev775;
    for(size_t i=0; i < n_v792; ++i)
    // for(size_t i=0; i < 1; ++i)      // PATCH FOR DEBUG
     {
      cout << " nev792[" << i << "]: " << nev792[i];
     }

    cout << endl;
   }
  else if (!all_fine)
   {
    cout << " ****** event " << ndaq << " ****** wrong number of gates " << nev775 << " ****** reading out more events ****** " << endl;
    m_usleep(100000);
   }
// if (!all_fine) { unlockTrigger(); exit(-999); }
  return all_fine;
 }

bool checkEvntCntrs()
 {
  uint32_t ndaq = totevents;
  uint32_t nev775 = (ntdc.evtCnt()+1) & 0xffffff;
  bool good775 = (nev775==ndaq);
  uint32_t nev792[n_v792];
  bool good792[n_v792];

  bool all_fine = good775;

  for(size_t i=0; i < n_v792; ++i)
   {
    nev792[i] = (v792s[i]->eventCounter()+1) & 0xffffff;
    good792[i] = (nev792[i]==ndaq);
    all_fine = all_fine && good792[i];
   }
  if (!all_fine) {
    cout << "MISMATCH in event counters! DAQ evt.s : " << ndaq << " - hw evt.s in v775: " << nev775;
    for(size_t i=0; i < n_v792; ++i)
     {
      cout << " - hw evt.s in v792[" << i << "]: " << nev792[i];
     }
    cout << endl;
   }
  return all_fine;
 }

void printEvntCntrs()
 {
  uint32_t ndaq = totevents;
  uint32_t nev775 = ntdc.evtCnt()+1;

  cout << " DAQ evt.s : " << ndaq << " - hw evt.s in v775: " << nev775;

  uint32_t nev792[n_v792];
  for(size_t i=0; i < n_v792; ++i)
   {
    nev792[i] = v792s[i]->eventCounter() + 1;
    cout << " - hw evt.s in v792[" << i << "]: " << nev792[i];
   }
  cout << endl;
 }

void initV792(){
  for(size_t i=0; i < n_v792; ++i)
   {
    v792s[i]->print();
    v792s[i]->reset();
    v792s[i]->disableSlide();
    v792s[i]->disableOverflowSupp();
    v792s[i]->disableZeroSupp();
    v792s[i]->clearEventCounter();
    v792s[i]->clearData();
    uint16_t ped;
    ped = v792s[i]->getI1();
    uint32_t bid = v792s[i]->id();
    cout << "v792[" << i << "] addr 0x" << hex << bid << dec
         << ": default ped I1 value is " << ped << endl;
    
    ped = 255;                  // >~ minimum possible Iped (see manual par. 2.1.2.1)
    v792s[i]->setI1(ped);       // set I1 current to define pedestal position
    ped =v792s[i]->getI1();     // in the range [0x00-0xFF] (see manual par. 4.34.2)
    uint16_t thr = 0x00;
    v792s[i]->setChannelThreshold(thr);
    cout << "v792[" << i << "] addr 0x" << hex << bid << dec << ": now ped  I1 value is " << ped << " thr " << thr << endl;
   }
  
  for(size_t i=0; i < n_v792; ++i)
   {
    v792s[i]->enableChannels();

    uint32_t bid = v792s[i]->id();
    uint16_t reg1 = v792s[i]->getStatus1();
    uint16_t reg2 = v792s[i]->getControl1();
    uint16_t reg3 = v792s[i]->getStatus2();
    uint16_t reg4 = v792s[i]->getBit1();
    uint16_t reg5 = v792s[i]->getBit2();
    uint32_t evc = v792s[i]->eventCounter();
    cout << "v792[" << i << "] addr 0x" << hex << bid << " status 1 0x" << reg1
         << " control 1 0x" << reg2 << " status 2 0x" << reg3 << " bit 1 0x" << reg4
         << " bit 2 0x" << reg5 << dec << endl;
    cout << "v792[" << i << "] event counter " << evc << endl;
   }
 }

void myReadOutInit(){

  initV792();

  ntdc.swReset();
  ntdc.zeroSuppression(false);
  ntdc.overSuppression(false);
  ntdc.setLSB(140);   // set the LSB (ps)  full scale = 4096*LSB ==> 140 ps * 4096 ~ 570 ns
  ntdc.storeEmpty();
  ntdc.evCntReset();
  ntdc.dataReset();
  ntdc.printRegisters();

}

void myReadOutArm(){

  double tref = get_time();

  for(size_t i=0; i < n_v792; ++i){
    v792s[i]->clearData();
  }

  ntdc.dataReset();

#ifdef USE_DRS_V1742
  DRS4.dataReset();
#endif

  t_rdoarm += get_time()-tref;
}
 
uint32_t nSpill(0);
uint32_t nBeam(0);
uint32_t nPed(0);
uint32_t nTrig(0);
uint32_t nBoth(0);

int32_t myReadEvent(){

  double tref = get_time();

  uint32_t size=0;

  //resets internal buffer
  myNewEvent();

// V775 TDC

  myFormatSubEvent(ntdc.id());

  size = ntdc.readSingleEvent(&buf[*bufsize]);

// cout << " *** TDC # " << ntdc.evtCnt()+1 << " - size " << size << endl;
if (size == 0xffffffff) { cout << " *** TDC event error *** " << endl; send_signal(); }
if (size == 0xffffffff) size = 0;
  myUpdateSubEvent(size);

// V792 and V862 ADCs

  for(size_t i=0; i < n_v792; ++i){
  // for(size_t i=0; i < 1; ++i){      // PATCH FOR DEBUG

    myFormatSubEvent(v792s[i]->id());

    // v792s[i]->setEvents(events,pedevents);  // old version < 15.08.2021
    v792s[i]->setEvents(totevents);
    size=v792s[i]->readEvent(&buf[*bufsize]);

    myUpdateSubEvent(size);
  }

#ifdef USE_DRS_V1742
// V1742 DRS4 Digitizer

  myFormatSubEvent(DRS4.id());

  time_t t1 = time(NULL);
  time_t dt(0);
  while (DRS4.busy() || !DRS4.eventReady())
   {
    m_usleep(10);
    time_t t2 = time(NULL)-t1;
    if (t2 > dt)
     {
      cout << " DRS4 event not ready ... waiting " << t2 << endl;
      dt += 10;
     }
   }
  
  size = DRS4.readSingleEvent(&buf[*bufsize]);
if (size != drs4_ev_size)
if (size < 10000)
 {
  if (size != DRS4_HALF_EV_SIZE)
    cout << " ****** ERROR FROM DRS READOUT ==> TRYING TO RECOVER ******" << size << " half " << DRS4_HALF_EV_SIZE << " full " << DRS4_FULL_EV_SIZE << endl;
  // do some tricks ...
  uint32_t * drs_init = &buf[*bufsize];
  uint32_t * drs_half = drs_init + size;
  drs_half -= SIZE_OF_V1742_HEADER;
  uint32_t temp_store[SIZE_OF_V1742_HEADER];
  for (uint32_t i=0; i<SIZE_OF_V1742_HEADER; i++) temp_store[i] = drs_half[i];
  while (!DRS4.eventReady()) { m_usleep(10); }
  uint32_t size1 = DRS4.readSingleEvent(drs_half);
  uint32_t evsize1 = drs_init[0] & 0xfffffff;
  uint32_t evsize2 = drs_half[0] & 0xfffffff;
  uint32_t gm1 = drs_init[1] & 0xf;
  uint32_t gm2 = drs_half[1] & 0xf;
  uint32_t ec1 = drs_init[2];
  uint32_t ec2 = drs_half[2];
  uint32_t ett1 = drs_init[3];
  uint32_t ett2 = drs_half[3];
  uint32_t tot_evsize = evsize1+evsize2-SIZE_OF_V1742_HEADER;
  uint32_t gr_mask = gm1 | gm2;
  uint32_t check_mask = gm1 & gm2;
  if ((gr_mask != 0xf) && (check_mask != 0))
   {
    cout << " gr mask 1 " << hex << gm1 << " gr mask 2 " << gm2
         << " OR " << gr_mask << " AND " << check_mask << dec << endl;
   }
  drs_init[0] = 0xa0000000 | tot_evsize;
  drs_init[1] = drs_init[1] | gr_mask ;
  
  for (uint32_t i=0; i<SIZE_OF_V1742_HEADER; i++) drs_half[i] = temp_store[i];
  
  if ((size1 != DRS4_HALF_EV_SIZE) || (evsize1 != DRS4_HALF_EV_SIZE) ||
      (evsize2 != DRS4_HALF_EV_SIZE) || (tot_evsize != DRS4_FULL_EV_SIZE))
    cout << spills << " events: -- Total " << totevents
         << " -- Beam " << events
         << " -- Pedestals " << pedevents
         << " -- Both " << doubletrig
         << " *** DRS problem size1: " << size1 << " evsize1: " << evsize1
         << " evsize2: " << evsize2 << " ==> now is: " << tot_evsize
         << "\n ev c[1] " << ec1 << " [2] " << ec2
         << hex << " ev t[1] " << ett1 << " [2] " << ett2
         << dec << endl;
  size = tot_evsize;

  // DRS4.acqReset();

 }

  myUpdateSubEvent(size);

#endif

  t_readev += get_time()-tref;

  return 0;
}

int32_t getEvent (uint32_t isPhysEvent){
  
  double tref = get_time();

  while (isHWBusy() && !abort_run) {}

  if(abort_run) return 1;

  myReadEvent();
  myWriteEvent(spills,isPhysEvent);
  t_mtrigger += get_time()-tref;

  return 0;
}

bool isTriggerPresent (uint32_t * trigmask)
 {
  if (!ntdc.isDataReady()) return false;
// cout << totevents << " " << ntdc.evtCnt() << endl; sleep(1);

  double tref = get_time(); 
  bool physT1T2 = isT1T2Trig();
  bool ped = isPedTrig();

  if (physT1T2 || ped)
   {
    // while (isHWBusy()) {}

    if (!ntdc.isDataReady())
     {
cout << "****** physT1T2: " << (physT1T2 ? "true" : "false")
    << "****** ped: " << (ped ? "true" : "false")
    << " - ntdc.isDataReady: false - going to unlockTrigger" <<  endl;
      unlockTrigger();
      return false;
     }
   }
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
      
      *bsum += *bufsize;
	  
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
while (0 && !checkEvntCntrs());

    myReadOutArm();
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
	// while (isHWBusy()) {}
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
  maxevts=myMaxEvents();
  phypedratio=myPhysPedRatio();
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

#ifdef USE_DRS_V1742
  //check if V1792 initialization worked 
  if(DRS4.getErrorState()){
    m_usleep(1000);
    return -1;
  }
#endif

  //Init I/O Registers
  io.reset(); initV513( ion );

  //Set vetoes
  disableTriggers();
  disablePedTriggers();
  
  //Reset and init readout
  myReadOutInit();
   
  //Connect the FIFO
  myOpenRun(&abort_run);

  //Internal buffer
  buf = myBuffer(&bufsize);

#ifdef USE_DRS_V1742
  drs4_ev_size = DRS4.evSize();
  cout << " ************ drs4_ev_size is " << drs4_ev_size << " ********* " << endl;
  if (isInSpill()) m_usleep(200000);
#endif

  //Start of run setting up
  events=0;
  spills=0;
  myReadOutArm();

  unlockTrigger();

  resetNIMScaler();
  rdtim=0;

  cout << tprint() << " Waiting for a spill ... " << endl;
  //Wait for a spill before to start
  while(!isInSpill() && !abort_run) {}
  cout << "\n Got ... start data taking at " << tprint() << endl;
  tlast = time(NULL);

  //Remove the veto and start
  myFIFOlock();
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
  
      myFIFOUpdateSpillNr();
      myFIFOUpdateEvtNr( events, pedevents );
        
      double occ=myFIFOOccupancy();
      double pedocc=myPedFIFOOccupancy();

      myFIFOunlock();

      // printEvntCntrs();
      checkEvntCntrs();
      loginfo(occ,pedocc);

      while(!isInSpill() && !abort_run) { m_usleep(10); }

      myFIFOlock();
      if (abort_run) break;

      enableTriggers();
      spillStart = time(NULL);
    } // if(!isInSpill())

  } while(1); // daq loop ... exit on ctrl-c or when (events>=maxevts)

  run_closing = true;
  cout << tprint() << " Ending run ... setting Trigger veto" << endl;
  disableTriggers();

  check_and_handle_event();

  myFIFOUpdateSpillNr();
  myFIFOUpdateEvtNr( events, pedevents );

  double occ=myFIFOOccupancy();
  double pedocc=myPedFIFOOccupancy();

  myFIFOunlock();

  spillStart = 0; spillEnd = totDuration; totprevious = 0;
  loginfo(occ,pedocc);

  //Disconnect the FIFO
  myCloseRun();

  tottime = times(&tmsbuf) - time0;
  tottime /= ticks_per_sec;
  cputime = tmsbuf.tms_utime + tmsbuf.tms_stime;
  cputime /= ticks_per_sec;
  percent = cputime/tottime;
  percent = rint(percent*10000)/100;
  
  if (totevents) {
    t_istrigger /= totevents;
    t_rdoarm /= totevents;
    t_readev /= totevents;
    t_mtrigger /= totevents;
    t_trghandler /= totevents;
  }

  cout << "istrigger " << uint32_t(t_istrigger)
       << " usec - rdoarm " << uint32_t(t_rdoarm)
       << " usec - readev " << uint32_t(t_readev)
       << " usec - mtrigger " << uint32_t(t_mtrigger)
       << " usec - trghandler " << uint32_t(t_trghandler)
       << " usec\n";

  cout << "TIME - TOT: " << tottime << " sec - CPU: " << cputime << " sec ("       << percent << "%)" << endl;

  cout.precision(6);
  if (totevents>0) {
    uint32_t rt, meansz;
    rt = rdtim/totevents;
    meansz = bsum*sizeof(uint32_t)/totevents;
    cout << "Average size: " << meansz << " bytes - time/event: "
         << rt << " usec" << endl;
  }
  
  return 0;
}
