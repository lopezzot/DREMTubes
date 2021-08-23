//$Id: myReadOutCorbo.cpp,v 1.3 2007/06/04 13:11:25 cvsdream Exp $
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
//#include "myV792AC.h" // CAEN ADC V792AC class derived from VME
#include "myKLOETDC.h"  // 32ch KLOETDC class derived from VME
//#include "mySIS3320.h"  // Flash ADC
#include "myL1182.h"    // LeCroy ADC
#include "myV262.h"     // CAEN V262 I/O REG
#include "myV260.h"     // CAEN V260 Scaler
#include "myL1176.h"    // LeCroy L1176 TDC
#include "myModules.h"  //

//#define PROFILING

using namespace std;

/****************************************/
// Declare all the hardware
/****************************************/
corbo   corbo(0xfff000,"/dev/vmedrv24d16");
//sis3320 fadc(0x20000000,"/dev/vmedrv32d32","/dev/vmedrv32d32");//,&io);

l1182 adc5(0x010000,"/dev/vmedrv24d16");
l1182 adc4(0x020000,"/dev/vmedrv24d16");
l1182 adc3(0x030000,"/dev/vmedrv24d16");
l1182 adc2(0x040000,"/dev/vmedrv24d16");
l1182 adc1(0x050000,"/dev/vmedrv24d16");
l1182 adc0(0x060000,"/dev/vmedrv24d16");

//v262  io(0x400000,"/dev/vmedrvb24d16");

v260 scaler0(0x200000,"/dev/vmedrv24d16");
l1176 tdc0(0x300000,"/dev/vmedrv24d32");

/****************************************/

unsigned int * buf;
unsigned int * bufsize;
unsigned int events, skipped;

unsigned int spills;
bool abort_run=false;

struct timespec rdtv0, rdtv1;
unsigned int rdtim;

void cntrl_c_handler(int sig)
{
  cout << endl << "Got control-c, end of current run" << endl << endl;
  abort_run = true;
}


void initIO(){
  //resets  I/O reg outputs
  //io.reset();

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

  //scaler
  corbo.enableChannel(3);
  corbo.setInputInternal(3);
  corbo.setBusyModeLevel(3);
  corbo.setBusyOutputLocal(3);
  corbo.disableEventIRQ(3);
  corbo.testTrigger(3);
}

//void setTriggerVeto(){
//  io.enableNIMOut(0);
//}

//void clearTriggerVeto(){
//  io.disableNIMOut(0);
//}

unsigned short isInSpill(){
  
  time_t t=time(NULL);

  if((t%10)<5)
    return 1;
  else
    return 0;

  //  return io.getNIMIn(2);
}

unsigned short isHWBusy(){
  return corbo.getInputState(1);
}

void unlockTrigger(){
  unsigned int tread;
  corbo.clearBusy(1);
  corbo.clearBusy(2);
  corbo.testTrigger(2);

  clock_gettime(CLOCK_REALTIME, &rdtv1);
  tread = (rdtv1.tv_sec-rdtv0.tv_sec)*1000000+(rdtv1.tv_nsec-rdtv0.tv_nsec)/1000;
  rdtim += tread;
}

void resetScaler(){

  corbo.clearBusy(3);
  corbo.testTrigger(3);
}

unsigned short isTriggerPresent(){
  clock_gettime(CLOCK_REALTIME, &rdtv0);
  return corbo.getBusyState(1);
}

/*
void initFlash(){
  
  unsigned int i;

  //resest the module
  //fadc.reset();
  
  //disable IRQs
  //fadc.disableIRQ();
  //set internal clock to 200MHz
  //fadc.setClockInt200();
  
  //enables internal stop generation
  //fadc.enableInternalTriggerStop();
  //disables autostart  
  //fadc.disableAutostart();
  //enables external start
  //fadc.enableLEMOStartStop();

  //set DAC values for offset control
  unsigned int dac[8];
  for(i=0;i<4;i++)
    dac[i]=0xA000;
  for(i=4;i<8;i++)
    dac[i]=0xC000;
  //fadc.loadDACs(dac);

  //set ADC range
  // fadc.setADCFullRange(1);
  //fadc.setADCFullRange(2);
  //fadc.setADCFullRange(3);
  //fadc.setADCFullRange(4);
  //fadc.setADCFullRange(5);
  //fadc.setADCFullRange(6);
  //fadc.setADCFullRange(7);
  //fadc.setADCFullRange(8);


  fadc.setADCHalfRange(5);
  fadc.setADCHalfRange(6);
  fadc.setADCHalfRange(7);
  fadc.setADCHalfRange(8);


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
  
}
*/
								 /*
unsigned int myReadFlash(unsigned int *buf){

  unsigned int i,j;
  unsigned int size=0;

  for(i=1;i<5;i++)
    for(j=1;j<3;j++)
      size+=fadc.readData(i,j,&buf[size]);

  return size;
}
								 */
void myReadOutInit(){
 
  //initFlash();
}

void myReadOutArm(){
     
  //fadc.setStartAddress(1,0);
  //fadc.setStartAddress(2,0);
  //fadc.setStartAddress(3,0);
  //fadc.setStartAddress(4,0);
  //fadc.armSampling();
}

int myReadEvent(){
  
  unsigned int size=0;

  //resets internal buffer
  myNewEvent(false);
  
  myFormatSubEvent(tdc0.id());

  size=0;//tdc0.readSingleEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);

#ifdef PROFILING
  //io.enableNIMOut(1);
  io.enableNIMOut(2);
#endif

  //myFormatSubEvent(tdc1.id());

  //size=tdc1.readEvent(&buf[*bufsize]);

  //myUpdateSubEvent(size);
/*
  myFormatSubEvent(fadc.id());

  size=myReadFlash(&buf[*bufsize]);

  myUpdateSubEvent(size);
*/
#ifdef PROFILING
  //io.disableNIMOut(1);
  io.disableNIMOut(2);
#endif

  myFormatSubEvent(adc0.id());
  
  size=0;//adc0.readSingleEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);

#ifdef PROFILING
  io.enableNIMOut(2);
#endif

  myFormatSubEvent(adc1.id());
  
  size=0;//adc1.readSingleEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);

#ifdef PROFILING
  io.disableNIMOut(2);
#endif

  myFormatSubEvent(adc2.id());
  
  size=0;//adc2.readSingleEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);

#ifdef PROFILING
  io.enableNIMOut(2);
#endif

  myFormatSubEvent(adc3.id());
  
  size=0;//adc3.readSingleEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);

#ifdef PROFILING
  io.disableNIMOut(2);
#endif

  myFormatSubEvent(adc4.id());
  
  size=0;//adc4.readSingleEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);

#ifdef PROFILING
  io.enableNIMOut(2);
#endif  

  myFormatSubEvent(adc5.id());
  
  size=0;//adc5.readSingleEvent(&buf[*bufsize],4);

  myUpdateSubEvent(size);

#ifdef PROFILING
  io.disableNIMOut(2);
#endif

  myFormatSubEvent(scaler0.id());

  size=0;//scaler0.readEvent(&buf[*bufsize],1);

  myUpdateSubEvent(size);

#ifdef PROFILING
  io.enableNIMOut(2);
#endif

  return 0;
}

int myTrigger(){
  
  while(isHWBusy() && !abort_run) {}
#ifdef PROFILING
  io.disableNIMOut(2);
#endif
  if(!abort_run){
    myReadEvent();
    myWriteEvent(spills,1);
    return 0;
  } else
    return 1;
}

/****************************************/
int main()
/****************************************/
{
  unsigned int maxevts, runnr;
  //unsigned int i;
  struct timeval tv0,tv1; 
  struct timezone tz;
  unsigned int tsum=0, bsum=0;
  struct tms tmsbuf;
  unsigned int ticks_per_sec;
  double time0, cputime, tottime, percent;

  time0 = times(&tmsbuf);
  ticks_per_sec = sysconf(_SC_CLK_TCK);

  signal(SIGINT, cntrl_c_handler);      // Control-C handler

  ios::sync_with_stdio();          // To syncronize with stdio library (really needed?)
 
  //Maximum number of events
  maxevts=myMaxEvents();

  runnr = updateRunNumber();

  cout << "Run " << runnr << " starting, going to acquire " << maxevts << " events" << endl;

  //Init I/O Register
  initIO();

  unlockTrigger();
  sleep(1);
  //Set the veto
  //setTriggerVeto();
  
  //Reset and init readout
  myReadOutInit();
   
  //Connect the FIFO
  myOpenRun(&abort_run);
  
  cout << "Connected to FIFO" << endl;
  //Internal buffer
  buf = myBuffer(&bufsize);

  cout << "Buffer allocated" << endl;
  //Start of run setting up
  //abort_run=true;
  //maxevts=1;
  events=0;
  spills=0;
  myReadOutArm();

  cout << "Modules armed" << endl;

  clock_gettime(CLOCK_REALTIME, &rdtv0);
  rdtim=0;

  cout.precision(4);

  //Wait for a spill before to start
  while(!isInSpill() && !abort_run);
    //    usleep(10000);
    //cout << "Not in spill" << endl;
    //}

  //Remove the veto and start
  gettimeofday(&tv0,&tz);
  cout << "Starting" << endl;
  resetScaler();
  unlockTrigger();
  //clearTriggerVeto();
  //io.NIMPulse(1);

  while(events<maxevts && !abort_run){
    
    if(isTriggerPresent()){

#ifdef PROFILING
      io.enableNIMOut(1);
      io.enableNIMOut(2);
#endif

      if(!myTrigger()){
      
	gettimeofday(&tv1,&tz);
	tsum += (tv1.tv_sec-tv0.tv_sec)*1000000+tv1.tv_usec-tv0.tv_usec;
	bsum += *bufsize;
	events ++;
	tv0 = tv1;

#ifdef PROFILING
	io.disableNIMOut(2);
	io.disableNIMOut(1);
#endif

      }
      myReadOutArm();
      unlockTrigger();
      //      cout << "Spill " << spills << " Unlock" << endl;
    }
    
    if(!isInSpill() && !abort_run){
      //setTriggerVeto();
      bool reset=false;
      spills++;
     
      myFIFOUpdateSpillNr();
        
      if(isTriggerPresent()){
	if(!myTrigger()){
	  gettimeofday(&tv1,&tz);
	  tsum += (tv1.tv_sec-tv0.tv_sec)*1000000+tv1.tv_usec-tv0.tv_usec;
	  bsum += *bufsize;
	  events ++;
	  tv0 = tv1;
	}
	reset=true;
	//unlockTrigger();
      }
      
      double occ=myFIFOOccupancy();

      myFIFOunlock();

      if (events) {
      cout << "Spills " << spills <<" - Events "<< events
	   << " - Buffer occ. "<< occ*100 << "%"
	   << " - End in ~ " << ((maxevts-events)*spills/events)+1
	   << " spills" << endl ;
      }

      while(!isInSpill() && !abort_run) usleep(100);
      
      myFIFOlock();

      if(reset){
	myReadOutArm();
	unlockTrigger();
      }

      //clearTriggerVeto();
      gettimeofday(&tv0,&tz);
    }

  }

  //Disconnect the FIFO
  myCloseRun();

  cout << endl;
  cout << "Events: " << events << endl;

  cout.precision(6);
  if (events>0) {
    unsigned int ut, rt, rate, rtmx, meansz;
    ut = tsum/events;
    rt = rdtim/events;
    rate = 1000000/ut;
    rtmx = 1000000/rt;
    meansz = bsum/events;
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

  return 0;
}
