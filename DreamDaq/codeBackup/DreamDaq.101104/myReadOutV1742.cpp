//$Id: myReadOut.cpp,v 1.42 2009/07/26 10:56:48 dreamdaq Exp $
/*****************************************

  myDaq.cpp
  ---------

  Main DAQ program.

  A. Cardini / INFN Cagliari

  Version 0.1: 24/5/2001
  Version 0.5: 04/6/2001
  Version 0.6: 08/6/2001

*****************************************/

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

#include "myRunNumber.h"
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
#include "myV792AC.h" // CAEN ADC V792AC class derived from VME
#include "myV262.h"     // CAEN V262 I/O REG
#include "myV260.h"     // CAEN V260 Scaler
#include "myModules.h"  //
#include "myTH03.h"
#include "myTekOsc.h"
#include "myTriggerNumber.h"
#include "myV775.h"
#include "myV862AC.h"

#include <vector>
//#define PROFILING
//#define USE_TEK_TDS7254B

using namespace std;

/****************************************/
// Declare all the hardware
/****************************************/

#ifdef USE_TEK_TDS7254B
tekOsc osc(0xFAFA,0,1,0);
#endif

v792ac adc0(0x04000000,"/dev/vmedrv32d32");
v792ac adc1(0x05000000,"/dev/vmedrv32d32");
v792ac adc2(0x06000000,"/dev/vmedrv32d32");
v862ac adc3(0x07000000,"/dev/vmedrv32d32");

v792ac* v792s[] = {&adc0,&adc1,&adc2,&adc3};
unsigned int n_v792 = 4;

v260 scaler0(0x200000,"/dev/vmedrv24d16");

v775 ntdc(0x08000000,"/dev/vmedrv32d16");

v262  io(0x400000,"/dev/vmedrv24d16");

/****************************************/

unsigned int * buf;
unsigned int * bufsize;
unsigned int events(0), pedevents(0), doubletrig(0), totevents(0), scopevents(0);

unsigned int spills(0);
volatile bool abort_run=false;

// struct timespec rdtv0, rdtv1;
struct timeval rdtv0, rdtv1;
unsigned int rdtim;
unsigned int trigpipo;

uint32_t evinfo[2]={0,0};
char message[256];
time_t timestr;
bool tryonce = true;
void cntrl_c_handler(int sig)
{
  char * stime = asctime(localtime(&timestr));
  stime[24] = 0;
  fprintf(stderr,"%s cntrl_c_handler: %s\n\n", stime, message);
  fprintf(stderr,"aborting run\n");
  abort_run = true;
}

void storemessage(const char* msg){
}

double tref(0), t_istrigger(1), t_rdoarm(0), t_readev(0), t_mtrigger(0), t_trghandler(0), t_gettek(0), t_sleep(0), t_schedy(0);
uint64_t n_loop(0);

double get_time()
{
        double time_us;
        struct timeval t1;
        struct timezone tz;
        gettimeofday(&t1, &tz);
        t1.tv_sec -= 1287000000;
        time_us = (t1.tv_sec) * 1000000.0f + t1.tv_usec;
        return time_us;
}

void myTH03Handler(pthread_t * tid,struct th03_data *th03){
  int ret;

  th03->stop=false;
  th03->running=false;
  th03->init=false;

  ret = pthread_create(tid, NULL, th03_handler, th03);

  if(ret){
    cout << "Failed to create TH03 thread. Aborting." << endl;
    abort_run=true;
  }
}

void myTH03wait(struct th03_data * th03){
  
  cout << "Waiting for temperature monitor to settle..."<<flush;
  while(!(th03->init)){
    usleep(10000);
  }
  cout  << "done." << endl;

}

void myTH03Stop(pthread_t tid,struct th03_data *th03){
  
  th03->stop=true;
  if(th03->running){
    pthread_join(tid,NULL);
  }
}

void initIO(){
  //resets  I/O reg outputs
  io.reset();
}

void setTriggerVeto(){
  //Physics
  io.enableNIMOut(0);
}

void clearTriggerVeto(){
  //Physics
  io.disableNIMOut(0);
}


void setPedTriggerVeto(){
  //Pedestal
  io.enableNIMOut(3);
}

void clearPedTriggerVeto(){
  //Pedestal
  io.disableNIMOut(3);
}

void resetNIMScaler(){
  io.NIMPulse(1);
}

void clearedCount(){
  //io.NIMPulse(2);
}

bool Go()
 {
  return true;
 }

bool inSpill(0);

unsigned short isInSpill(){
  return inSpill = io.getNIMIn(2);
}

unsigned short isHWBusy(){
  return io.getNIMIn(1);
}

void unlockTrigger(){
  io.NIMPulse(0);
}

unsigned short isTriggerPresent(unsigned int * trigmask){
  // clock_gettime(CLOCK_REALTIME, &rdtv0);
  gettimeofday(&rdtv0, NULL);
  tref = get_time(); 
  unsigned short phys = io.getNIMIn(0);
  unsigned short ped = io.getNIMIn(3);
  if (ped) phys = io.getNIMIn(0);
  if (ped || phys) t_istrigger += get_time()-tref;

  *trigmask=(ped<<1)|phys;

  trigpipo=*trigmask;

  return phys | ped;
}

void clearADCs(){
  //clear ADC memory content
  //adcl.clear();
}

void initV792(){
  for(size_t i=0; i < n_v792; ++i){
    v792s[i]->print();
    v792s[i]->reset();
    v792s[i]->disableSlide();
    v792s[i]->disableOverflowSupp();
    v792s[i]->disableZeroSupp();
    unsigned short reg1 = v792s[i]->getStatus1();
    unsigned short reg2 = v792s[i]->getControl1();
    unsigned short reg3 = v792s[i]->getStatus2();
    unsigned short reg4 = v792s[i]->getBit1();
    unsigned short reg5 = v792s[i]->getBit2();
    cout << hex << "status 1 0x" << reg1 << " control 1 0x" << reg2 << " status 2 0x" << reg3 << " bit 1 0x" << reg4 << " bit 2 0x" << reg5 << dec << endl;

    unsigned short ped;
    ped = v792s[i]->getI1();
    unsigned int bid = v792s[i]->id();
    cout << hex << "0x" << bid << dec << " adc" << i << ": Default pedestal value is " << ped << endl;
    
    ped = 80; // >~ minimum possible Iped (see manual par. 2.1.2.1)
    v792s[i]->setI1(ped);  // Set I1 current to define pedestal position
    ped =v792s[i]->getI1();// in the range [0x00-0xFF] (see manual par. 4.34.2)
    unsigned short thr = 0x00;
    v792s[i]->setChannelThreshold(thr);
    cout << "Now adc" << i << " ped is " << ped << " thr " << thr << endl;
  }
  
  for(size_t i=0; i < n_v792; ++i) v792s[i]->enableChannels();
}

void myReadOutInit(){

#ifdef USE_TEK_TDS7254B
  osc.stop();
#endif
  
  initV792();

  clearADCs();
  
  //clears the counter contents
  scaler0.clear();
  //disables IRQs
  scaler0.disableIRQ();
  //removes inhibit
  scaler0.inhibitReset();
  
  ntdc.swReset();
  ntdc.setFullScale(1000);
  ntdc.printRegisters();

}

void myReadOutArm(){

  tref = get_time();

  //clear memory content
  //tdc0.clear();
    
  clearADCs();
  
  for(size_t i=0; i < n_v792; ++i){
    v792s[i]->clearData();
  }
  
  ntdc.dataReset();

  t_rdoarm += get_time()-tref;
}
 
unsigned int nSpill(0);
unsigned int nBeam(0);
unsigned int nPed(0);
unsigned int nTrig(0);
unsigned int nBoth(0);

int myReadEvent(bool isPhysEv){
  
  tref = get_time();

  unsigned int size=0;

  //resets internal buffer
  myNewEvent(false);
  
  
#ifdef PROFILING
  bool onoff = true;
  //io.disableNIMOut(1);
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif

  for(size_t i=0; i < n_v792; ++i){

    myFormatSubEvent(v792s[i]->id());

    size=v792s[i]->readEvent(&buf[*bufsize]);

    myUpdateSubEvent(size);
  
    #ifdef PROFILING
    onoff = !onoff;
    (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
    #endif
  }

  // V775
  
  myFormatOscSubEvent(ntdc.id());

  size = ntdc.readSingleEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);
  
#ifdef PROFILING
  onoff = !onoff;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif


  myFormatSubEvent(scaler0.id());

  unsigned int * scalerData = &buf[*bufsize];
  size=scaler0.readEvent(&buf[*bufsize],5);

  unsigned int nSpillNew = scalerData[0];
  unsigned int nBeamNew = scalerData[1];
  unsigned int nPedNew = scalerData[2];
  unsigned int nTrigNew = scalerData[3];
  unsigned int nBothNew = scalerData[4];
  
  nSpill = nSpillNew;
  nBeam = nBeamNew;
  nPed = nPedNew;
  nTrig = nTrigNew;
  nBoth = nBothNew;

  myUpdateSubEvent(size);
  
#ifdef PROFILING
  onoff = !onoff;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif


#ifdef USE_TEK_TDS7254B
  myFormatOscSubEvent(osc.id());

  size=osc.readEvent(&buf[*bufsize]);
  uint32_t os_size = size;

  myUpdateOscSubEvent(size);

#ifdef PROFILING
  onoff = !onoff;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif
#endif

#ifdef PROFILING
  bool onoff = false;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif

#ifdef USE_TEK_TDS7254B
  if (totevents < 1)
    printf("total size %d osc %d\n",*bufsize,os_size); 
#endif // USE_TEK_TDS7254B

  t_readev += get_time()-tref;

  return 0;
}

int myTrigger(unsigned int isPhysEvent){
  
  tref = get_time();

  while(isHWBusy() && !abort_run) {}

  if(!abort_run){
    myReadEvent(isPhysEvent);
    myWriteEvent(spills,isPhysEvent);
    t_mtrigger += get_time()-tref;
    return 0;
  } else
    return 1;
}

void myTriggerHandler(unsigned int trigmask,
		      unsigned int maxpedevts,
		      struct timeval * tv0,
		      unsigned int * tsum,
		      unsigned int * bsum,
		      bool * ped_enabled){
  
  tref = get_time();

  struct timeval tv1;
  struct timezone tz;

  do{   // Roberto : read out also double triggered events for proper accounting
    
#ifdef PROFILING
    io.enableNIMOut(1);
#endif
    
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
      
#ifdef PROFILING
      io.disableNIMOut(1);
      io.disableNIMOut(2);
#endif
    }
    myReadOutArm();
  } while(0);

  t_trghandler += get_time()-tref;
}

unsigned int GetTekData(unsigned int runnr, unsigned int spills,
                        unsigned int nev, unsigned int max) 
{
  tref = get_time();
  unsigned int samples(0);
#ifdef USE_TEK_TDS7254B
  if (nev+1>=max) 
    usleep(10000);
  osc.stop();
  usleep(100000);
  osc.write(runnr,spills);
  usleep(100000);
  samples = osc.getNSample();
  scopevents += samples;
  cout << "scope " << nev << " " << samples << " total " << scopevents << endl;
  t_gettek += get_time()-tref;
#endif // USE_TEK_TDS7254B
  return samples;
}

/****************************************/
int main(int argc, char** argv)
/****************************************/
{
  unsigned int maxevts, runnr, maxpedevts,maxoscevts,oldspillevts=0;
  unsigned int maxtotevts;
  unsigned int trigmask;
  //unsigned int i;
  struct timeval tv0;
  struct timezone tz;
  unsigned int tsum=0, bsum=0;
  struct tms tmsbuf;
  unsigned int ticks_per_sec;
  double time0, cputime, tottime, percent;
  bool ped_enabled=true;
  bool phys_enabled=true;
  unsigned int phypedratio;
  unsigned int oldevents=0;

#ifdef USE_TEK_TDS7254B
  unsigned int oscpts;
  unsigned int oscchmask;
#endif
  
  cout << " MAIN " << endl;

  maxoscevts = UINT_MAX;
  time0 = times(&tmsbuf);
  ticks_per_sec = sysconf(_SC_CLK_TCK);

  timestr = time(NULL);
  signal(SIGINT, cntrl_c_handler);      // Control-C handler

  ios::sync_with_stdio();          // To syncronize with stdio library (really needed?)

  cout.precision(4);
   
  //Maximum number of events
  maxevts=myMaxEvents();
  phypedratio=myPhysPedRatio();
  maxpedevts=maxevts/phypedratio;
  maxtotevts=maxevts+maxpedevts;
  if(phypedratio<1){
    cout << "Physics trigger DISABLED. Pedestal run." << endl;
    phys_enabled=false;
  }else
    cout << "Physics/Pedestal ratio: " << phypedratio << endl;

#ifdef USE_TEK_TDS7254B
  maxoscevts=osc.getMaxSample();
  oscpts=osc.getSampleLen();
  oscchmask=osc.getChMask();
#endif

  runnr = updateRunNumber();

  cout << "Run " << runnr << " starting, going to acquire " << maxtotevts << " events" << endl;

  //Init I/O Register
  initIO();

  //Set the vet
  setTriggerVeto();
  setPedTriggerVeto();
  
  //Reset and init readout
  myReadOutInit();
   
  //Connect the FIFO
  myOpenRun(&abort_run);

  //Internal buffer
  buf = myBuffer(&bufsize);

  //Start of run setting up
  //abort_run=true;
  //maxevts=1;
  events=0;
  spills=0;
  myReadOutArm();
#ifdef USE_TEK_TDS7254B
  osc.start(); 
  sleep(5);
#endif

  // clock_gettime(CLOCK_REALTIME, &rdtv0);
  gettimeofday(&rdtv0, NULL);
  unlockTrigger();
  resetNIMScaler();
  rdtim=0;

  cout << "Waiting for a spill ... " << flush;
  //Wait for a spill before to start
  while(!isInSpill() && !abort_run);
  cout << "starting loop " << endl;

  //exit(0);

  //Remove the veto and start
  gettimeofday(&tv0,&tz);
  clearPedTriggerVeto();

  if(phys_enabled)
    clearTriggerVeto();
      
  // myFIFObackup();
  while(1){

    timestr = time(NULL);
    if(isTriggerPresent(&trigmask)){

      timestr = time(NULL);
      myTriggerHandler(trigmask,maxpedevts,
		       &tv0,&tsum,&bsum,
		       &ped_enabled);
      timestr = time(NULL);
      if(phys_enabled){
	if(trigmask & 0x2){
	  setPedTriggerVeto();
	  oldevents+=phypedratio;
	}else if(trigmask & 0x1){
	  if(ped_enabled && (events>=oldevents+phypedratio))
	    clearPedTriggerVeto();
	}
      }
      
      if ((totevents>=maxtotevts) || abort_run) break;

      unlockTrigger();
      // clock_gettime(CLOCK_REALTIME, &rdtv1);
      gettimeofday(&rdtv1, NULL);
      unsigned int tread =
          (rdtv1.tv_sec-rdtv0.tv_sec)*1000000+(rdtv1.tv_usec-rdtv0.tv_usec);
      rdtim += tread;
    }
    
    timestr = time(NULL);
    if(!isInSpill() || 
       (events+pedevents-oldspillevts)==(maxoscevts-1)){
      setTriggerVeto();
      setPedTriggerVeto();
      usleep(1);
      spills++;

      // while(isInSpill());
  
      myFIFOUpdateSpillNr();
        
      if(isTriggerPresent(&trigmask)){

        timestr = time(NULL);
	myTriggerHandler(trigmask,maxpedevts,
			 &tv0,&tsum,&bsum,
			 &ped_enabled);
	// unlockTrigger();
        // clock_gettime(CLOCK_REALTIME, &rdtv1);
        gettimeofday(&rdtv1, NULL);
        unsigned int tread =
            (rdtv1.tv_sec-rdtv0.tv_sec)*1000000+(rdtv1.tv_usec-rdtv0.tv_usec);
        rdtim += tread;
      }

#ifdef USE_TEK_TDS7254B
      uint32_t rlat = time(NULL);
      uint32_t nev = events+pedevents-oldspillevts;
      uint32_t samples = GetTekData(runnr,spills,nev,maxoscevts);
      if (samples != nev) {
        cout << "1. Oscilloscope samples " << samples << " differ from VME: "
             << nev << ". This should not happen. Stopping here." << endl;
        abort_run = true;
        // myFIFOrestore(); osc.start();
      } else if (myUpdateOscData(samples,runnr,spills,oscchmask,oscpts)!=0) {
	cout << "Oscilloscope data update failed" << endl;
	abort_run=true;
      } else osc.start();
      rlat = time(NULL)-rlat;
      time_t result = time(NULL);
      char * tm = asctime(localtime(&result));
      tm[24] = 0;
      cout << tm << " - FIFO Update done for " << samples
           << " evt.s in " << rlat << " sec" << endl;
#endif
      
      double occ=myFIFOOccupancy();
      double pedocc=myPedFIFOOccupancy();

      myFIFOunlock();

      if (events || pedevents) {
	unsigned int neededsp;
	if((events) && (maxevts > events)) {
	  double ratio = maxevts/double(events)-1.;
	  neededsp=uint32_t(ratio*spills+1);
	} else
	  neededsp=0;

	if (abort_run) break;

	cout << spills << " - Total Events " << totevents
	     << " -- Phys. Events " << events
	     << " ("<< occ*100 << "% occ.)"
	     << " -- Pedestals " << pedevents
	     << " ("<< pedocc*100 << "% occ.)"
	     << " -- Double Triggered " << doubletrig
	     << " -- End in ~ " << neededsp
	     << " spills" 
	     << endl << endl;
      }

      oldspillevts=events+pedevents;

      while(isInSpill()) ;
      unlockTrigger();

      while(!isInSpill() && !abort_run){
        tref = get_time();
	usleep(1);
        t_usleep = get_time()-tref;
        tref = get_time();
	sched_yield();
        t_schedy = get_time()-tref;
 	n_loop ++;
      }
      myFIFOlock();
      // myFIFObackup();

      if(!abort_run){
	if(phys_enabled)
	  clearTriggerVeto();
	else
	  clearPedTriggerVeto();
      }
      gettimeofday(&tv0,&tz);
    }

  }

  setTriggerVeto();
  setPedTriggerVeto();
  spills++;

#ifdef USE_TEK_TDS7254B
  uint32_t nev = events+pedevents-oldspillevts;
  uint32_t samples = GetTekData(runnr,spills,nev,maxoscevts);
  if (samples != nev) {
    cout << "2. Oscilloscope samples " << samples << " differ from VME: "
         << nev << ". This should not happen." << endl;
  } else myUpdateOscData(samples,runnr,spills,oscchmask,oscpts);
#endif

  //Disconnect the FIFO
  myCloseRun();

  cout << endl;
  cout << "Events: " << events << endl;
  cout << "Pedestal events: " << pedevents << endl;

  cout.precision(6);
  events += pedevents;
  if (events>0) {
    unsigned int ut, rt, rate, rtmx, meansz;
    ut = tsum/events;
    rt = rdtim/events;
    rate = 1000000/ut;
    rtmx = 1000000/rt;
    meansz = bsum*sizeof(unsigned int)/events;
    cout << "Average size: " << meansz << " bytes - time/event: " << rt <<
	" usec - rate: " << rate << " Hz" << endl;
  }
  
  tottime = times(&tmsbuf) - time0;
  tottime /= ticks_per_sec;
  cputime = tmsbuf.tms_utime + tmsbuf.tms_stime;
  cputime /= ticks_per_sec;
  percent = cputime/tottime;
  percent = rint(percent*10000)/100;
  cout << "TIME - TOT: " << tottime << " sec - CPU: " << cputime << " sec ("       << percent << "%)" << endl;
  
  if (totevents) {
    t_istrigger /= totevents;
    t_rdoarm /= totevents;
    t_readev /= totevents;
    t_mtrigger /= totevents;
    t_trghandler /= totevents;
    t_gettek /= totevents;
  }
  cout << "istrigger = " << uint32_t(t_istrigger)
       << " rdoarm = " << uint32_t(t_rdoarm)
       << " readev = " << uint32_t(t_readev)
       << " mtrigger = " << uint32_t(t_mtrigger)
       << " trghandler = " << uint32_t(t_trghandler)
       << " gettek = " << uint32_t(t_gettek) << endl;
  if (n_loop) {
    t_sleep /= n_loop;
  }
  cout << "TIME - TOT: " << tottime << " sec - CPU: " << cputime << " sec ("       << percent << "%)" << endl;
  int i=5;
  while (i)
   {
    std::cout << i-- << "..";
    sleep(1);
   }
  std::cout << i << std::endl;
  return 0;
}
