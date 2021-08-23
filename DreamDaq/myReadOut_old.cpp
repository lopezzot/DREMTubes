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

uint32_t events(0), pedevents(0), doubletrig(0), totevents(0), spills(0), totprevious(0);

string tprint()
 {
  time_t tt(time(NULL));
  char tbuf[26];
  ctime_r( &tt, tbuf);
  tbuf[24] = '\0';
  return string(tbuf);
 }

time_t tlast(0);
time_t tdiff(0);
void loginfo(uint32_t neededsp=0, double occ=0, double pedocc=0)
 {
  time_t tnow(time(NULL));
  if (tlast != 0)
   {
    time_t df = tnow-tlast-tdiff;
    if ((tdiff != 0) && (abs(df) > 1) && (spills > 2)) cout << spills << " *** spill period was " << tdiff << " now is " << tdiff+df << endl;
    tdiff += df;
   }
  tlast = tnow;
  cout << spills << " : " << totevents-totprevious << " Events taken"
       << " -- Total " << totevents
       << " -- Beam " << events
       << " ("<< occ*100 << "% occ.)"
       << " -- Pedestals " << pedevents
       << " ("<< pedocc*100 << "% occ.)"
       << " -- Both " << doubletrig
       << " -- End in ~ " << neededsp
       << " spills - " << tprint()
       << '\n' << endl;
  totprevious = totevents;
  sync();
 }

class firstlastMessages
 {
  public:
   firstlastMessages() { cout << __FILE__ " compiled on " __DATE__ " " __TIME__ "\n**************************\n"; loginfo(); cout << "DREAM RUN STARTING" << endl; }
   ~firstlastMessages() { cout << "DREAM RUN CLOSED\n"; loginfo(); cout << "**************************" << endl; }
 };

firstlastMessages _firstlastMsgs;

/****************************************/
#include "myFIFO-IO.h"
#include "myRunNumber.h"
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
#endif

// ADC V792
v792ac adc0(0x04000000,"/V2718/cvA32_U_DATA/0");
v792ac adc1(0x05000000,"/V2718/cvA32_U_DATA/0");
v792ac adc2(0x06000000,"/V2718/cvA32_U_DATA/0");
v792ac adc3(0x07000000,"/V2718/cvA32_U_DATA/0");
// ADC V862 - identical to V792 from SW point of view
v862ac adc4(0x03000000,"/V2718/cvA32_U_DATA/0");

uint32_t n_v792 = 5;
v792ac* v792s[] = {&adc0,&adc1,&adc2,&adc3,&adc4};

v775 ntdc(0x08000000,"/V2718/cvA32_U_DATA/0");

v262 io(0x400000,"/V2718/cvA24_U_DATA/1");
v513 ion(0xa00000,"/V2718/cvA24_U_DATA/1");

v2718 v2718_1(0,"/V2718/cvA24_U_DATA/0");
v2718 v2718_2(0,"/V2718/cvA24_U_DATA/1");

/****************************************/

uint32_t * buf;
uint32_t * bufsize;
volatile bool abort_run=false;

// struct timespec rdtv0, rdtv1;
struct timeval rdtv0, rdtv1;
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

double tref(0), t_istrigger(1), t_rdoarm(0), t_readev(0), t_mtrigger(0), t_trghandler(0);
uint64_t n_loop(0);

double get_time()
 {
  double time_us;
  struct timeval t1;
  struct timezone tz;
  gettimeofday(&t1, &tz);
  t1.tv_sec -= 1417000000;
  time_us = (t1.tv_sec) * 1000000.0f + t1.tv_usec;
  return time_us;
 }

void initIO() { io.reset(); ion.reset(); } //resets  I/O reg outputs

#ifndef _USE_NIM_LOGIC       // then use FPGA LOGIC (reversed wrt NIM LOGIC)
 #define SET_IO_PORT io.disableNIMOut
 #define CLEAR_IO_PORT io.enableNIMOut
#else
 #define SET_IO_PORT io.enableNIMOut
 #define CLEAR_IO_PORT io.disableNIMOut
#endif

inline void setTriggerVeto() { SET_IO_PORT(0); } // Physics
inline void clearTriggerVeto() { CLEAR_IO_PORT(0); } // Physics

inline void setPedTriggerVeto() { SET_IO_PORT(3); } // Pedestal
inline void clearPedTriggerVeto() { CLEAR_IO_PORT(3); } // Pedestal

inline void unlockTrigger() { io.NIMPulse(0); }
inline void resetNIMScaler() { io.NIMPulse(1); }

inline bool isT1T2Trig() { return (ion.readInputRegister() & 1); /* io.getNIMIn(0); */ }
inline bool isHWBusy() { return (ion.readInputRegister() & 2); /* io.getNIMIn(1); */ } // return (v2718m.getInputRegister() & 1);
inline bool isInSpill() { return (ion.readInputRegister() & 4); /* io.getNIMIn(2); */ }
inline bool isPedTrig() { return (ion.readInputRegister() & 8); /* io.getNIMIn(3); */ }

bool isTriggerPresent (uint32_t * trigmask)
 {
  gettimeofday(&rdtv0, NULL);
  tref = get_time(); 
  bool physT1T2 = isT1T2Trig();
  bool ped = isPedTrig();

  if (ped) physT1T2 = isT1T2Trig();
  if (ped || physT1T2) t_istrigger += get_time()-tref;

  *trigmask = (ped<<1) | physT1T2;

  return physT1T2 || ped;
 }

void initV792(){
  for(size_t i=0; i < n_v792; ++i)
   {
    v792s[i]->print();
    v792s[i]->reset();
    v792s[i]->disableSlide();
    v792s[i]->disableOverflowSupp();
    v792s[i]->disableZeroSupp();
    uint16_t ped;
    ped = v792s[i]->getI1();
    uint32_t bid = v792s[i]->id();
    cout << "v792[" << i << "] addr 0x" << hex << bid << dec
         << ": default ped I1 value is " << ped << endl;
    
    ped = 255;                  // >~ minimum possible Iped (see manual par. 2.1.2.1)
    v792s[i]->setI1(ped);       // Set I1 current to define pedestal position
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
    cout << "v792[" << i << "] addr 0x" << hex << bid << " status 1 0x" << reg1
         << " control 1 0x" << reg2 << " status 2 0x" << reg3 << " bit 1 0x" << reg4
         << " bit 2 0x" << reg5 << dec << endl;
   }
 }

void myReadOutInit(){

  initV792();

// TDC Bob 16/06/2011
  ntdc.swReset();
  ntdc.zeroSuppression(false);
  ntdc.overSuppression(false);
  ntdc.setLSB(140);   // set the LSB (ps)  full scale = 4096*LSB ==> 140 ps * 4096 ~ 570 ns
  ntdc.storeEmpty();
  ntdc.printRegisters();

}

void myReadOutArm(){

  tref = get_time();

  for(size_t i=0; i < n_v792; ++i){
    v792s[i]->clearData();
  }

// TDC Bob 16/06/2011
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

  tref = get_time();

  uint32_t size=0;

  //resets internal buffer
  myNewEvent(false);

// TDC Bob 16/06/2011

  // V775
  
  myFormatSubEvent(ntdc.id());

  size = ntdc.readSingleEvent(&buf[*bufsize]);
  myUpdateSubEvent(size);

// ADC Bob 16/06/2011

  // V792
  
  for(size_t i=0; i < n_v792; ++i){

    myFormatSubEvent(v792s[i]->id());

    size=v792s[i]->readEvent(&buf[*bufsize]);

    myUpdateSubEvent(size);
  }

#ifdef USE_DRS_V1742
// DRS4 Bob 16/06/2011

  // V1742
  
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
if (size < 10000)
 {
  if (size != DRS4_HALF_EV_SIZE)
    cout << " ****** ERROR FROM DRS READOUT ==> TRYING TO RECOVER ******" << endl;
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
      (evsize1 != DRS4_HALF_EV_SIZE) || (tot_evsize != DRS4_FULL_EV_SIZE))
    cout << " size1: " << size1 << " evsize1: " << evsize1
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

int32_t myTrigger(uint32_t isPhysEvent){
  
  tref = get_time();

  while(isHWBusy() && !abort_run) {}

  if(abort_run) return 1;

  myReadEvent();
  myWriteEvent(spills,isPhysEvent);
  t_mtrigger += get_time()-tref;

  return 0;
}

void myTriggerHandler(uint32_t trigmask,
		      uint32_t maxpedevts,
		      struct timeval * tv0,
		      uint32_t * tsum,
		      uint32_t * bsum,
		      bool * ped_enabled){
  
  tref = get_time();

  struct timeval tv1;
  struct timezone tz;

  do{   // Roberto : read out also double triggered events for proper accounting
    
    if(!myTrigger(trigmask & 0x1)){
      
      gettimeofday(&tv1,&tz);
      *tsum += (tv1.tv_sec-tv0->tv_sec)*1000000+tv1.tv_usec-tv0->tv_usec;
      *bsum += *bufsize;
	  
      if(trigmask & 1){
	events++;
        if (trigmask & 2) doubletrig++;
      }else{
	pedevents++;
	if(pedevents>=maxpedevts){
	  setPedTriggerVeto();
	  *ped_enabled=false;
	}
      }
      totevents ++;
      
      *tv0 = tv1;
      
    }
    myReadOutArm();
  } while(0);

  t_trghandler += get_time()-tref;
}

/****************************************/
int32_t main(int32_t argc, char** argv)
/****************************************/
{
  uint32_t maxevts, runnr;
  uint32_t maxpedevts, maxtotevts;
  uint32_t trigmask;
  struct timeval tv0;
  struct timezone tz;
  uint32_t tsum=0, bsum=0;
  struct tms tmsbuf;
  uint32_t ticks_per_sec;
  double time0, cputime, tottime, percent;
  bool ped_enabled=true;
  double phypedratio;
  double oldevents=0;

  bool t1t2trig(true);
  if ((argc == 2) && (strcasecmp(argv[1],"T1*T2*T3")==0)) t1t2trig = false;

  time0 = times(&tmsbuf);
  ticks_per_sec = sysconf(_SC_CLK_TCK);

  signal(SIGINT, cntrl_c_handler);      // Control-C handler

  cout.precision(4);
   
  //Maximum number of events
  maxevts=myMaxEvents();
  phypedratio=myPhysPedRatio();
  double dmaxp = double(maxevts)/phypedratio;
  maxpedevts = lrint(dmaxp);
  maxtotevts=maxevts+maxpedevts;
  if(phypedratio<1){
    // cout << "Physics trigger DISABLED. Pedestal run." << endl;
  }

  runnr = readRunNumber();

  cout << tprint() << " Run " << runnr << " starting, going to acquire " << maxtotevts << " events with trigger " << (t1t2trig ? "T1*T2" : "T1*T2*T3") << endl;

#ifdef USE_DRS_V1742
  //check if V1792 initialization worked 
  if(DRS4.getErrorState()){
    m_usleep(1000);
    return -1;
  }
#endif

  //Init I/O Register
  initIO();

  //Set the vet
  setTriggerVeto();
  setPedTriggerVeto();
  unlockTrigger();
  
  //Reset and init readout
  myReadOutInit();
   
  //Connect the FIFO
  myOpenRun(&abort_run);

  //Internal buffer
  buf = myBuffer(&bufsize);

  //Start of run setting up
  events=0;
  spills=0;
  myReadOutArm();

  // clock_gettime(CLOCK_REALTIME, &rdtv0);
  gettimeofday(&rdtv0, NULL);
  resetNIMScaler();
  rdtim=0;

  // lock FIFO.s
  myFIFOlock();

  cout << tprint() << " Waiting for a spill ... " << endl;
  //Wait for a spill before to start
  while(!isInSpill() && !abort_run) {}
  cout << "\n Got ... start data taking at " << tprint() << endl;

  //Remove the veto and start
  gettimeofday(&tv0,&tz);
  clearPedTriggerVeto();
  clearTriggerVeto();

  while(1){

    if(isTriggerPresent(&trigmask)){

      myTriggerHandler(trigmask,maxpedevts,
		       &tv0,&tsum,&bsum,
		       &ped_enabled);

	if(trigmask & 0x2){
	  oldevents+=phypedratio;
	  if (events<(uint32_t)lrint(oldevents)) setPedTriggerVeto();
	}else if(trigmask & 0x1){
	  if(ped_enabled && (events>oldevents)){
	    clearPedTriggerVeto();
	  }
	}
      
      if ((totevents>=maxtotevts) || abort_run) break;

      unlockTrigger();
      gettimeofday(&rdtv1, NULL);
      uint32_t tread =
          (rdtv1.tv_sec-rdtv0.tv_sec)*1000000+(rdtv1.tv_usec-rdtv0.tv_usec);
      rdtim += tread;
    }
    
    if(!isInSpill()){
      setTriggerVeto();
      setPedTriggerVeto();
      unlockTrigger();
      usleep(10);
      spills++;
  
      myFIFOUpdateSpillNr();
      myFIFOUpdateEvtNr( events, pedevents );
        
      if(isTriggerPresent(&trigmask)){

	myTriggerHandler(trigmask,maxpedevts,
			 &tv0,&tsum,&bsum,
			 &ped_enabled);
	unlockTrigger();
        gettimeofday(&rdtv1, NULL);
        uint32_t tread =
            (rdtv1.tv_sec-rdtv0.tv_sec)*1000000+(rdtv1.tv_usec-rdtv0.tv_usec);
        rdtim += tread;
      }

      double occ=myFIFOOccupancy();
      double pedocc=myPedFIFOOccupancy();

      myFIFOunlock();

      if (events || pedevents)
       {
	uint32_t neededsp;
	if((events) && (maxevts > events)) {
	  double ratio = maxevts/double(events)-1.;
	  neededsp=uint32_t(ratio*spills+1);
	 } else neededsp=0;

// if (abort_run) break;

	loginfo(neededsp,occ,pedocc);
       }

if (abort_run) break;

      while(isInSpill()) {}
      unlockTrigger();

      while(!isInSpill() && !abort_run) { usleep(10); }

      myFIFOlock();

      if(!abort_run){
	clearTriggerVeto();
        if (events>=(uint32_t)lrint(oldevents)) clearPedTriggerVeto();
      }
      gettimeofday(&tv0,&tz);
    }

  }

  cout << tprint() << " Ending run ... setting Trigger vetoes" << endl;
  setTriggerVeto();
  setPedTriggerVeto();
  unlockTrigger();
  spills++;

  myFIFOUpdateSpillNr();
  myFIFOUpdateEvtNr( events, pedevents );

  double occ=myFIFOOccupancy();
  double pedocc=myPedFIFOOccupancy();

  loginfo(0,occ,pedocc);
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

  cout << "istrigger = " << uint32_t(t_istrigger)
       << " rdoarm = " << uint32_t(t_rdoarm)
       << " readev = " << uint32_t(t_readev)
       << " mtrigger = " << uint32_t(t_mtrigger)
       << " trghandler = " << uint32_t(t_trghandler);

  cout << "\nTIME - TOT: " << tottime << " sec - CPU: " << cputime << " sec ("       << percent << "%)" << endl;

  cout.precision(6);
  if (totevents>0) {
    uint32_t ut, rt, rate, meansz;
    ut = tsum/totevents;
    rt = rdtim/totevents;
    rate = 1000000/ut;
    meansz = bsum*sizeof(uint32_t)/totevents;
    cout << "Average size: " << meansz << " bytes - time/event: " << rt <<
	" usec - rate: " << rate << " Hz" << endl;
  }
  
  return 0;
}
