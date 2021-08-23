//$Id: myReadOut.cpp,v 1.40 2008/08/02 14:52:30 dreamdaq Exp $
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
#include "myCorbo.h"    // Creative Electronic Systems RCB 8047 CORBO class derived from VME
#include "myV792AC.h" // CAEN ADC V792AC class derived from VME
//#include "mySIS3320.h"  // Flash ADC
#include "myL1182.h"    // LeCroy ADC
#include "myV262.h"     // CAEN V262 I/O REG
#include "myV260.h"     // CAEN V260 Scaler
#include "myL1176.h"    // LeCroy L1176 TDC
#include "myModules.h"  //
#include "myTH03.h"
#include "myTekOsc.h"
#include "myV258.h"
#include "myV512.h"
#include "myV513.h"
#include "myTriggerNumber.h"

//#define PROFILING
#define USE_TEK_TDS7254B

using namespace std;

/****************************************/
// Declare all the hardware
/****************************************/
//corbo   corbo0(0xfff000,"/dev/vmedrv24d16");
#ifdef EVENT_NUMBER_IS_ON
vTriggerNumber evtnum(0x100000,"/dev/vmedrv32d32");
#endif

l1182 adc2(0x010000,"/dev/vmedrvb24d16");
//sis3320 fadc(0x20000000,"/dev/vmedrv32d32","/dev/vmedrv32d32");//,&io);

#ifdef USE_TEK_TDS7254B
tekOsc osc(0xFAFA,0,1,0);
#endif

//V792 0x04000000 0x06000000
v792ac adc0(0x4000000,"/dev/vmedrvb32d32"); 
v792ac adc1(0x6000000,"/dev/vmedrvb32d32");
v260 scaler0(0x200000,"/dev/vmedrvb24d16");
//l1176 tdc0(0x300000,"/dev/vmedrvb24d32");
//v258 disc(0xFF0000, "/dev/vmedrv24d16");
//v512 plu(0x20000000,"/dev/vmedrv32d16");

#ifndef IOREG_IS_V513
#define IOREG_IS_V262
 v262  io(0x400000,"/dev/vmedrvb24d16");
 v262  io2(0x800000,"/dev/vmedrvb24d16");
#endif
#ifdef IOREG_IS_V513
 v513 io0(0x30000000,"/dev/vmedrv32d16");
 v513 io1(0x40000000, "/dev/vmedrv32d16");
#endif

/****************************************/

unsigned int * buf;
unsigned int * bufsize;
unsigned int events(0), pedevents(0), doubletrig(0);

unsigned int spills(0);
bool abort_run=false;

// struct timespec rdtv0, rdtv1;
struct timeval rdtv0, rdtv1;
unsigned int rdtim;
struct th03_data th03;
unsigned int trigpipo;

void cntrl_c_handler(int sig)
{
  cout << endl << "Got control-c, end of current run" << endl << endl;
  abort_run = true;
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
#ifdef IOREG_IS_V262
  io.reset();
  io2.reset();
#endif
#ifdef IOREG_IS_V513
  io0.reset();

  io0.setChannelStatusRegister(0,7);
  io0.setChannelStatusRegister(1,7);
  io0.setChannelStatusRegister(2,7);
  io0.setChannelStatusRegister(3,7);

  io0.setChannelStatusRegister(4,6);
  io0.setChannelStatusRegister(5,6);
  io0.setChannelStatusRegister(6,6);
  io0.setChannelStatusRegister(7,6);
  io0.setChannelStatusRegister(8,6);
  io0.setChannelStatusRegister(9,6);
#endif
  
}

void setTriggerVeto(){
  //Physics
#ifdef IOREG_IS_V262
  io.enableNIMOut(0);
#endif
#ifdef IOREG_IS_V513
  io0.setOutputBit(4);
#endif
}

void clearTriggerVeto(){
  //Physics
#ifdef IOREG_IS_V262
  io.disableNIMOut(0);
#endif
#ifdef IOREG_IS_V513
  io0.clearOutputBit(4);
#endif
}


void setPedTriggerVeto(){
  //Pedestal
#ifdef IOREG_IS_V262
  io.enableNIMOut(3);
#endif
#ifdef IOREG_IS_V513
  io0.setOutputBit(5);
#endif
}

void clearPedTriggerVeto(){
  //Pedestal
#ifdef IOREG_IS_V262
  io.disableNIMOut(3);
#endif
#ifdef IOREG_IS_V513
  io0.clearOutputBit(5);
#endif
}

void resetNIMScaler(){
#ifdef IOREG_IS_V262
  io.NIMPulse(1);
#endif
#ifdef IOREG_IS_V513
  io0.setOutputBit(9);
  io0.clearOutputBit(9);
#endif
}

void clearedCount(){
  //io.NIMPulse(2);
}

bool Go()
 {
  return true;
 }

unsigned short isInSpill(){
#ifdef IOREG_IS_V262
  return io.getNIMIn(2);
#endif
#ifdef IOREG_IS_V513
  return (io0.readInputRegister()>>2)&0x1;
#endif
}

unsigned short isHWBusy(){
#ifdef IOREG_IS_V262
  return io.getNIMIn(1);
#endif
#ifdef IOREG_IS_V513
  return (io0.readInputRegister()>>1)&0x1;
#endif
}

void unlockTrigger(){
#ifdef IOREG_IS_V262
  io.NIMPulse(0);
#endif
#ifdef IOREG_IS_V513
  io0.setOutputBit(8);
  io0.clearOutputBit(8);
#endif
}

unsigned short isTriggerPresent(unsigned int * trigmask){
  // clock_gettime(CLOCK_REALTIME, &rdtv0);
  gettimeofday(&rdtv0, NULL);
#ifdef IOREG_IS_V262
  unsigned short phys = io.getNIMIn(0);
  unsigned short ped = io.getNIMIn(3);
  if (ped) phys = io.getNIMIn(0);
#endif
#ifdef IOREG_IS_V513
  unsigned short ioreg = io0.readInputRegister();
  unsigned short phys = ioreg&0x1;
  unsigned short ped = (ioreg>>3)&0x1;
#endif

  *trigmask=(ped<<1)|phys;

  trigpipo=*trigmask;

  return phys | ped;
}

/*void initFlash(){
  
  unsigned int i;

  //resest the module
  fadc.reset();
  
  //disable IRQs
  fadc.disableIRQ();
  //set internal clock to 200MHz
  fadc.setClockInt200();
  
  //enables internal stop generation
  fadc.enableInternalTriggerStop();
  //disables autostart  
  fadc.disableAutostart();
  //enables external start
  fadc.enableLEMOStartStop();

  //set DAC values for offset control
  unsigned int dac[8];
  for(i=0;i<4;i++)
    dac[i]=0xA000;
  for(i=4;i<8;i++)
    dac[i]=0xC000;
  fadc.loadDACs(dac);

  //set ADC range
  fadc.setADCFullRange(1);
  fadc.setADCFullRange(2);
  fadc.setADCFullRange(3);
  fadc.setADCFullRange(4);
  fadc.setADCFullRange(5);
  fadc.setADCFullRange(6);
  fadc.setADCFullRange(7);
  fadc.setADCFullRange(8);

  
  //fadc.setADCHalfRange(5);
  //fadc.setADCHalfRange(6);
  //fadc.setADCHalfRange(7);
  //fadc.setADCHalfRange(8);
  

  //enables stop generation by sample length
  fadc.enableStopbyLength(0);
  //disables page wrap
  fadc.setMemoryWrap(0);
  //disables accumulator mode
  fadc.disableAccumulator(0);
  
  //sets the memory page
  fadc.setADCPage(0);
  
  //set the sample length
  fadc.setSampleLength(1,16);
  fadc.setSampleLength(2,16);
  fadc.setSampleLength(3,16);
  fadc.setSampleLength(4,16);
  

  //set memory start address
  fadc.setStartAddress(1,0);
  fadc.setStartAddress(2,0);
  fadc.setStartAddress(3,0);
  fadc.setStartAddress(4,0);
  
  //disable internal trigger
  fadc.disableLTTrigger(1,1);
  fadc.disableLTTrigger(1,2);
  fadc.disableLTTrigger(2,1);
  fadc.disableLTTrigger(2,2);
  fadc.disableLTTrigger(3,1);
  fadc.disableLTTrigger(4,2);
  fadc.disableLTTrigger(4,1);
  fadc.disableLTTrigger(4,2);
  
  fadc.disableGTTrigger(1,1);
  fadc.disableGTTrigger(1,2);
  fadc.disableGTTrigger(2,1);
  fadc.disableGTTrigger(2,2);
  fadc.disableGTTrigger(3,1);
  fadc.disableGTTrigger(4,2);
  fadc.disableGTTrigger(4,1);
  fadc.disableGTTrigger(4,2);
  
}*/

/*unsigned int myReadFlash(unsigned int *buf){

  unsigned int i,j;
  unsigned int size=0;

  for(i=1;i<5;i++)
    for(j=1;j<3;j++)
      size+=fadc.readData(i,j,&buf[size]);

  return size;
  }*/

void clearADCs(){
  //clear ADC memory content
  adc2.clear();
}

void l1176Init(){
  /*
  //sets common stop mode
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
  tdc0.enableAcq();*/
}

void initV792(){
  
  adc0.reset();
  adc1.reset();
  
  adc0.disableSlide();
  adc1.disableSlide();

  adc0.disableOverflowSupp();
  adc1.disableOverflowSupp();

  adc0.disableZeroSupp();
  adc1.disableZeroSupp();

  unsigned short ped;
  ped = adc0.getI1();
  cout << "adc0: Default pedestal value is " << ped << endl;
  ped = adc1.getI1();
  cout << "adc1: Default pedestal value is " << ped << endl;

  ped = 255;
  adc0.setI1(ped);           // Set I1 current to define pedestal position
  ped = adc0.getI1();        // in the range [0x00-0xFF] (see manual par. 4.34.2)
  cout << "Now adc0 ped is set to " << ped << endl;
  adc1.setI1(ped);           // Set I1 current to define pedestal position
  ped = adc1.getI1();        // in the range [0x00-0xFF] (see manual par. 4.34.2)
  cout << "Now adc1 ped is set to " << ped << endl;
  
  /*adc1.disableChannel(0);
  adc1.disableChannel(1);
  adc1.disableChannel(2);
  adc1.disableChannel(3);
  adc1.disableChannel(4);
  adc1.disableChannel(5);
  adc1.disableChannel(6);
  adc1.disableChannel(7);
  adc1.disableChannel(8);
  adc1.disableChannel(9);
  adc1.disableChannel(10);
  adc1.disableChannel(11);
  adc1.disableChannel(12);
  adc1.disableChannel(13);
  adc1.disableChannel(14);
  adc1.disableChannel(15);*/
}

void myReadOutInit(){

#ifdef USE_TEK_TDS7254B
  osc.stop();
#endif
  
//initFlash();

  initV792();

  l1176Init();

  clearADCs();
  
  //sets ADC gate from fron panel
  adc2.selectFrontPanelGate();
  
  
  //clears the counter contents
  scaler0.clear();
  //disables IRQs
  scaler0.disableIRQ();
  //removes inhibit
  scaler0.inhibitReset();
  
  /*plu.setFunctionMask(4);
  plu.resetVMEVeto();

  disc.reset();
  disc.enableChannels(0x0300);
  disc.setThreshold(8,50);
  disc.setThreshold(10,50);*/

#ifdef EVENT_NUMBER_IS_ON
  evtnum.reset();
#endif

}

void myReadOutArm(){
  //clear memory content
  //tdc0.clear();
    
  clearADCs();
  
  adc0.clearData();
  adc1.clearData();

  /*fadc.setStartAddress(1,0);
  fadc.setStartAddress(2,0);
  fadc.setStartAddress(3,0);
  fadc.setStartAddress(4,0);
  fadc.armSampling();*/
}

int myReadEvent(bool isPhysEv){
  
  unsigned int size=0;

  //resets internal buffer
  myNewEvent(false);
  
  
#ifdef PROFILING
  bool onoff = true;
  //io.disableNIMOut(1);
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif

#ifdef EVENT_NUMBER_IS_ON
  myFormatSubEvent(evtnum.id());

  size=evtnum.getInfo(&buf[*bufsize]);

  myUpdateSubEvent(size);

#ifdef PROFILING
  onoff = !onoff;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif

#endif
/*
  myFormatSubEvent(tdc0.id());

  size=tdc0.readSingleEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);
*/
#ifdef PROFILING
  onoff = !onoff;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif
  
  myFormatSubEvent(adc0.id());

  size=adc0.readEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);
  
#ifdef PROFILING
  onoff = !onoff;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif
  
   myFormatSubEvent(adc1.id());

  size=adc1.readEvent(&buf[*bufsize]);
 
  myUpdateSubEvent(size);
  
#ifdef PROFILING
  onoff = !onoff;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif

  /*myFormatSubEvent(fadc.id());

  size=myReadFlash(&buf[*bufsize]);

  myUpdateSubEvent(size);*/

 
  myFormatSubEvent(adc2.id());
  
  size=adc2.readSingleEvent(&buf[*bufsize],2);

  myUpdateSubEvent(size);
 
#ifdef PROFILING
  onoff = !onoff;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif

  /*
  myFormatSubEvent(adc3.id());
  
  size=adc3.readSingleEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);
  */
  
  /*
  myFormatSubEvent(adc4.id());
  
  size=adc4.readSingleEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);
  */
  /*
  myFormatSubEvent(adc5.id());
  
  size=adc5.readSingleEvent(&buf[*bufsize],4);

  myUpdateSubEvent(size);
  */

  
  myFormatSubEvent(scaler0.id());

  size=scaler0.readEvent(&buf[*bufsize],5);

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

  myFormatSubEvent(0xFFFF);

  buf[*bufsize]=th03.refs;
  buf[*bufsize+1]=th03.ch12;
  buf[*bufsize+2]=th03.ch3;

  myUpdateSubEvent(3);

#ifdef PROFILING
  bool onoff = false;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif

  if(events <2)
    printf("total %d osc %d\n",*bufsize,os_size); 

  return 0;
}

int myTrigger(unsigned int isPhysEvent){
  
  while(isHWBusy() && !abort_run) {}

  if(!abort_run){
    myReadEvent(isPhysEvent);
    myWriteEvent(spills,isPhysEvent);
    return 0;
  } else
    return 1;
}

void myTriggerHandler(unsigned int trigmask,
		      unsigned int maxpedevts,
		      struct timeval * tv0,
		      unsigned int * tsum,
		      unsigned int * bsum,
		      bool * ped_enabled,
		      unsigned int * cleared){
  
  struct timeval tv1;
  struct timezone tz;

  // if(trigmask<3){
  if(1){   // Roberto : read out also double triggered events for proper accounting
    
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
      
      *tv0 = tv1;
      
#ifdef PROFILING
      io.disableNIMOut(1);
      io.disableNIMOut(2);
#endif
    }
    myReadOutArm();
    //unlockTrigger();
  }else{
    //cout << "Found 2 trigger types. Stopping here" << endl;
    //abort_run=true;
    //usleep(10000);
    (*cleared)++;
    clearedCount();
    myNewEvent(true);
    while(isHWBusy() && !abort_run) {}
    myReadOutArm();
    //unlockTrigger();
  }
}

/****************************************/
int main()
/****************************************/
{
  unsigned int maxevts, runnr, maxpedevts,maxoscevts,oldspillevts=0;
  unsigned int trigmask;
  unsigned int cleared;
  //unsigned int i;
  struct timeval tv0;
  struct timezone tz;
  unsigned int tsum=0, bsum=0;
  struct tms tmsbuf;
  unsigned int ticks_per_sec;
  double time0, cputime, tottime, percent;
  pthread_t tid;
  bool ped_enabled=true;
  bool phys_enabled=true;
  unsigned int phypedratio;
  unsigned int oldevents=0;

#ifdef USE_TEK_TDS7254B
  unsigned int oscpts;
  unsigned int oscchmask;
#endif

  maxoscevts = UINT_MAX;
  time0 = times(&tmsbuf);
  ticks_per_sec = sysconf(_SC_CLK_TCK);

  signal(SIGINT, cntrl_c_handler);      // Control-C handler

  ios::sync_with_stdio();          // To syncronize with stdio library (really needed?)

  cout.precision(4);
   
  //Maximum number of events
  maxevts=myMaxEvents();
  phypedratio=myPhysPedRatio();
  maxpedevts=maxevts/phypedratio;

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

  cout << "Run " << runnr << " starting, going to acquire " << maxevts << " events" << endl;

  //Starts the TH03 handler
  myTH03Handler(&tid,&th03);
  
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
  cleared=0;
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

  myTH03wait(&th03);

  cout << "Waiting for a spill ... "<<flush;
  //Wait for a spill before to start
  while(!isInSpill() && !abort_run);
  cout << "done" << endl;

  //exit(0);

  //Remove the veto and start
  gettimeofday(&tv0,&tz);
  clearPedTriggerVeto();

  if(phys_enabled)
    clearTriggerVeto();
      
  cout << "here I am " << endl;
  while(((pedevents<maxpedevts) || (events<maxevts)) && !abort_run){

    if(isTriggerPresent(&trigmask)){
      myTriggerHandler(trigmask,maxpedevts,
		       &tv0,&tsum,&bsum,
		       &ped_enabled,
		       &cleared);
      if(phys_enabled){
	if(trigmask & 0x2){
	  setPedTriggerVeto();
	  oldevents+=phypedratio;
	}else if(trigmask & 0x1){
	  if(ped_enabled && (events>=oldevents+phypedratio))
	    clearPedTriggerVeto();
	}
      }
      unlockTrigger();
      // clock_gettime(CLOCK_REALTIME, &rdtv1);
      gettimeofday(&rdtv1, NULL);
      unsigned int tread =
          (rdtv1.tv_sec-rdtv0.tv_sec)*1000000+(rdtv1.tv_usec-rdtv0.tv_usec);
      rdtim += tread;
    }
    
    if(!isInSpill() || 
       (events+pedevents+cleared-oldspillevts)==(maxoscevts-1)){
      setTriggerVeto();
      setPedTriggerVeto();
      spills++;

      // while(isInSpill());
  
      myFIFOUpdateSpillNr();
        
      if(isTriggerPresent(&trigmask)){
	myTriggerHandler(trigmask,maxpedevts,
			 &tv0,&tsum,&bsum,
			 &ped_enabled,
			 &cleared);
	// unlockTrigger();
        // clock_gettime(CLOCK_REALTIME, &rdtv1);
        gettimeofday(&rdtv1, NULL);
        unsigned int tread =
            (rdtv1.tv_sec-rdtv0.tv_sec)*1000000+(rdtv1.tv_usec-rdtv0.tv_usec);
        rdtim += tread;
      }

#ifdef USE_TEK_TDS7254B
      if (events+pedevents+cleared+1>=oldspillevts+maxoscevts) usleep(10000);
      uint32_t rlat = time(NULL);
      // usleep(10);  // ok
      // asm ("NOP");  not
      // sched_yield(); not
      // struct timespec tsleep = {0,1000};
      // nanosleep(&tsleep, NULL);
      osc.stop();
      //sleep(2);
      cout << " checking scope " << events+pedevents+cleared-oldspillevts << endl;
      uint nw=0;
      if(events+pedevents+cleared-oldspillevts)
	while(osc.getNSample()==0){
	  usleep(10000);
          if (++nw > 100) break; 
	}
      unsigned int smp = osc.getNSample();
      cout << " scope checked " << nw << " samples " << smp << endl;
      osc.write(runnr,spills);
      unsigned int samples = osc.getNSample();
      if(samples!=(events+pedevents+cleared-oldspillevts)){
	cout << "Oscilloscope samples " << samples << " differ from VME samples " << events+pedevents+cleared-oldspillevts << ". This should not happen. Stopping here." << endl;
	abort_run=true;
      }
      osc.start();

      //Manage oscilloscope data
      if(myUpdateOscData(samples,runnr,spills,oscchmask,oscpts)!=0){
	abort_run=true;
      }
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
	  double ratio = 1.*maxevts/events-1.;
	  neededsp=uint32_t(ratio*spills+1);
	} else
	  neededsp=0;

	  cout << spills <<" - Phys. Events "<< events
	       << " ("<< occ*100 << "% occ.)"
	       << " -- Pedestals " << pedevents
	       << " ("<< pedocc*100 << "% occ.)"
	       << " -- Double Triggered " << doubletrig
	       << " -- End in ~ " << neededsp
	       << " spills" 
	       << endl << endl;
      }

      oldspillevts=events+pedevents+cleared;

      while(isInSpill()) ;
      unlockTrigger();

      while(!isInSpill() && !abort_run){
	usleep(100);
      }

      myFIFOlock();

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
  osc.stop();
  if(events+pedevents+cleared-oldspillevts)
    while(osc.getNSample()==0);
  
  osc.write(runnr,spills);
  unsigned int samples = osc.getNSample();
  //cout << "Osc. sample " << samples << endl;
  if(samples<(events+pedevents+cleared-oldspillevts)){
    cout << "Oscilloscope samples " << samples << " differ from VME samples " << events+pedevents+cleared-oldspillevts << ". This should not happen. Stopping here." << endl;
  }else{
    //Manage oscilloscope data
    myUpdateOscData(samples,runnr,spills,oscchmask,oscpts);
  }
#endif

  //Disconnect the FIFO
  myCloseRun();

  //Close the TH03 thread
  myTH03Stop(tid,&th03);

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

  int i=5;
  while (i)
   {
    std::cout << i-- << "..";
    sleep(1);
   }
  std::cout << i << std::endl;
  return 0;
}
