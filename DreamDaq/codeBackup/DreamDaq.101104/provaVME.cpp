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
#include "myCorbo.h"    // Creative Electronic Systems RCB 8047 CORBO class derived from VME
#include "myV792AC.h" // CAEN ADC V792AC class derived from VME
//#include "mySIS3320.h"  // Flash ADC
#include "myL1182.h"    // LeCroy ADC
#include "myV262.h"     // CAEN V262 I/O REG
#include "myV260.h"     // CAEN V260 Scaler
#include "myL1176.h"    // LeCroy L1176 TDC
#include "myModules.h"  //
#include "myTekOsc.h"
#include "myV258.h"
#include "myV512.h"
#include "myV513.h"
#include "myV775.h"
#include "myTriggerNumber.h"
#include "dreamudp.h"

//#define PROFILING
//#define USE_TEK_TDS7254B
//#define UDPSYNC

using namespace std;

/****************************************/
// Declare all the hardware
/****************************************/
//corbo   corbo0(0xfff000,"/dev/vmedrv24d16");
#ifdef EVENT_NUMBER_IS_ON
vTriggerNumber evtnum(0x100000,"/dev/vmedrv32d32");
#endif

//l1182 adcl(0x010000,"/dev/vmedrvb24d16");
//sis3320 fadc(0x20000000,"/dev/vmedrv32d32","/dev/vmedrv32d32");//,&io);

#ifdef USE_TEK_TDS7254B
tekOsc osc(0xFAFA,0,1,0);
#endif

v792ac * padc0;
v792ac * padc1;
v792ac * padc2;
v792ac * padc3;
v792ac * padc4;
//V792 0x04000000 0x06000000
/*
v792ac adc0(0x06000000,"/dev/vmedrv32d32"); 
v792ac adc1(0x07000000,"/dev/vmedrv32d32");
v792ac adc2(0x08000000,"/dev/vmedrv32d32");
v792ac adc3(0x09000000,"/dev/vmedrv32d32");
v792ac adc4(0x0a000000,"/dev/vmedrv32d32");
*/
//v260 scaler0(0x00200000,"/dev/vmedrv24d16");
//l1176 tdc0(0x00300000,"/dev/vmedrv24d32");
//v258 disc(0xFF0000, "/dev/vmedrv24d16");
//v512 plu(0x20000000,"/dev/vmedrv32d16");

#ifndef IOREG_IS_V513
#define IOREG_IS_V262
// v262  io(0x00400000,"/dev/vmedrv24d16");
 // v262  io2(0x00800000,"/dev/vmedrv24d16");
#endif
#ifdef IOREG_IS_V513
 v513 io0(0x30000000,"/dev/vmedrv32d16");
 v513 io1(0x40000000, "/dev/vmedrv32d16");
#endif

/****************************************/

unsigned int * buf;
unsigned int * bufsize;
unsigned int events(0), pedevents(0), doubletrig(0), totevents(0);

unsigned int spills(0);
bool abort_run=false;

// struct timespec rdtv0, rdtv1;
struct timeval rdtv0, rdtv1;
unsigned int rdtim;
unsigned int trigpipo;

dreamudp dsrv;

uint32_t evinfo[2]={0,0};
char message[256];
time_t timestr;
bool tryonce = true;
void cntrl_c_handler(int sig)
{
  char * stime = asctime(localtime(&timestr));
  stime[24] = 0;
  fprintf(stdout,"%s cntrl_c_handler: %s\n\n", stime, message);
  fprintf(stdout,"aborting run\n");
  abort_run = true;
}

void storemessage(const char* msg){
}

void initIO(){
  //io.reset();
}

void setTriggerVeto(){
  //Physics
  //io.enableNIMOut(0);
}

void clearTriggerVeto(){
  //Physics
  //io.disableNIMOut(0);
}


void setPedTriggerVeto(){
  //io.enableNIMOut(3);
}

void clearPedTriggerVeto(){
  //io.disableNIMOut(3);
}

void resetNIMScaler(){
  //io.NIMPulse(1);
}

void clearedCount(){
  //io.NIMPulse(2);
}

bool Go()
 {
  return true;
 }

unsigned short isInSpill(){
  return 2; //io.getNIMIn(2);
}

unsigned short isHWBusy(){
  return 2;//io.getNIMIn(1);
}

void unlockTrigger(){
  //io.NIMPulse(0);
}

unsigned short isTriggerPresent(unsigned int * trigmask){
  // clock_gettime(CLOCK_REALTIME, &rdtv0);
  gettimeofday(&rdtv0, NULL);
  unsigned short phys = 2;//io.getNIMIn(0);
  unsigned short ped = 2;//io.getNIMIn(3);
  if (ped) phys = 2;//io.getNIMIn(0);

  *trigmask=(ped<<1)|phys;

  trigpipo=*trigmask;

  return phys | ped;
}

void clearADCs(){
}

void initV792(){
  
  (*padc0).reset();
  (*padc1).reset();
  (*padc2).reset();
  
  (*padc0).disableSlide();
  (*padc1).disableSlide();
  (*padc2).disableSlide();

  (*padc0).disableOverflowSupp();
  (*padc1).disableOverflowSupp();
  (*padc2).disableOverflowSupp();

  (*padc0).disableZeroSupp();
  (*padc1).disableZeroSupp();
  (*padc2).disableZeroSupp();

  unsigned short ped;
  ped = (*padc0).getI1();
  cout << "adc0: Default pedestal value is " << ped << endl;
  ped = (*padc1).getI1();
  cout << "adc1: Default pedestal value is " << ped << endl;
  ped = (*padc2).getI1();
  cout << "adc2: Default pedestal value is " << ped << endl;

  ped = 255;
  (*padc0).setI1(ped);           // Set I1 current to define pedestal position
  ped = (*padc0).getI1();        // in the range [0x00-0xFF] (see manual par. 4.34.2)
  cout << "Now adc0 ped is set to " << ped << endl;
  (*padc1).setI1(ped);           // Set I1 current to define pedestal position
  ped = (*padc1).getI1();        // in the range [0x00-0xFF] (see manual par. 4.34.2)
  cout << "Now adc1 ped is set to " << ped << endl;

  (*padc2).setI1(ped);           // Set I1 current to define pedestal position
  ped = (*padc2).getI1();        // in the range [0x00-0xFF] (see manual par. 4.34.2)
  cout << "Now adc2 ped is set to " << ped << endl;
  
}

void myReadOutInit(){

#ifdef USE_TEK_TDS7254B
  osc.stop();
#endif
  
//initFlash();

  initV792();

  clearADCs();
  
  //sets ADC gate from fron panel
  //adcl.selectFrontPanelGate();
  
  
  //clears the counter contents
  //scaler0.clear();
  //disables IRQs
  //scaler0.disableIRQ();
  //removes inhibit
  //scaler0.inhibitReset();
  
#ifdef EVENT_NUMBER_IS_ON
  evtnum.reset();
#endif

}

void myReadOutArm(){
  clearADCs();
  
  (*padc0).clearData();
  (*padc1).clearData();
  (*padc2).clearData();

}

unsigned int nSpill(0);
unsigned int nBeam(0);
unsigned int nPed(0);
unsigned int nTrig(0);
unsigned int nBoth(0);

int myReadEvent(bool isPhysEv){
  
  unsigned int size=0;

  //resets internal buffer
  myNewEvent(false);
  
  
#ifdef EVENT_NUMBER_IS_ON
  myFormatSubEvent(evtnum.id());

  size=evtnum.getInfo(&buf[*bufsize]);

  myUpdateSubEvent(size);

#endif

  myFormatSubEvent((*padc0).id());

  size=(*padc0).readEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);
  
  myFormatSubEvent((*padc1).id());

  size=(*padc1).readEvent(&buf[*bufsize]);
 
  myUpdateSubEvent(size);
  
  myFormatSubEvent((*padc2).id());

  size=(*padc2).readEvent(&buf[*bufsize]);
 
  myUpdateSubEvent(size);
  
  //myFormatSubEvent(scaler0.id());

  unsigned int * scalerData = &buf[*bufsize];
  //size=scaler0.readEvent(&buf[*bufsize],5);

  unsigned int nSpillNew = scalerData[0];
  unsigned int nBeamNew = scalerData[1];
  unsigned int nPedNew = scalerData[2];
  unsigned int nTrigNew = scalerData[3];
  unsigned int nBothNew = scalerData[4];
  
  unsigned int dSp = nSpillNew - nSpill;
  unsigned int dBm = nBeamNew - nBeam;
  unsigned int dPd = nPedNew - nPed;
  unsigned int dTr = nTrigNew - nTrig;
  unsigned int dBt = nBothNew - nBoth;

  if ((dBm > 1) || (dPd > 1) || (dTr > 1) || (dBt > 1)) {
    char msg[1024];
    sprintf(msg,
            "TRG LOST %d %d %d - nSpill %d / %d - nTrig %d / %d - nBoth %d / %d - beam %d / %d - ped %d / %d",
        pedevents, doubletrig, totevents, nSpillNew, nSpill, nTrigNew, nTrig,
        nBothNew, nBoth, nBeamNew, nBeam, nPedNew, nPed);
     storemessage(msg);
  }

  nSpill = nSpillNew;
  nBeam = nBeamNew;
  nPed = nPedNew;
  nTrig = nTrigNew;
  nBoth = nBothNew;

  myUpdateSubEvent(size);
  
#ifdef USE_TEK_TDS7254B
  myFormatOscSubEvent(osc.id());

  size=osc.readEvent(&buf[*bufsize]);
  uint32_t os_size = size;

  myUpdateOscSubEvent(size);

#endif

#ifdef USE_TEK_TDS7254B
  if (totevents < 1)
    printf("total size %d osc %d\n",*bufsize,os_size); 
#endif // USE_TEK_TDS7254B

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
		      bool * ped_enabled){
  
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
}

unsigned int GetTekData(unsigned int runnr, unsigned int spills,
                        unsigned int nev, unsigned int max) 
{
  unsigned int samples(0);
#ifdef USE_TEK_TDS7254B
  if (nev+1>=max) 
    usleep(10000);
  cout << " stop scope " << nev << endl;
  osc.stop();
  usleep(100000);
  osc.write(runnr,spills);
  usleep(100000);
  samples = osc.getNSample();
#endif // USE_TEK_TDS7254B
  return samples;
}
void dlkey()
 {
  std::cout << "Press any key to continue...";
  getchar();
  std::cout << std::endl;
 }

/****************************************/
int xmainx(int argc, char** argv)
/****************************************/
{
  dlkey();
  padc0 = new v792ac (0x06000000,"/dev/vmedrv32d32"); 
  (*padc0).reset();
  dlkey();
  padc1 = new v792ac (0x07000000,"/dev/vmedrv32d32");
  (*padc1).reset();
  dlkey();
  padc2 = new v792ac (0x08000000,"/dev/vmedrv32d32");
  (*padc2).reset();
  dlkey();
  padc3 = new v792ac (0x09000000,"/dev/vmedrv32d32");
  (*padc3).reset();
  dlkey();
  padc4 = new v792ac (0x0a000000,"/dev/vmedrv32d32");
  (*padc4).reset();
  dlkey();
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

  timestr = time(NULL);
  sprintf(message,"Starting now");
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
      
  cout << "starting loop " << endl;
  // myFIFObackup();
  while((totevents<maxtotevts) && !abort_run){

    timestr = time(NULL);
    sprintf(message,"Checking trigger %d", totevents);
    if(isTriggerPresent(&trigmask)){

      timestr = time(NULL);
      sprintf(message,"Trigger present %d", totevents);
      myTriggerHandler(trigmask,maxpedevts,
		       &tv0,&tsum,&bsum,
		       &ped_enabled);
      timestr = time(NULL);
      sprintf(message,"Trigger handled %d", totevents);
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
    
    timestr = time(NULL);
    sprintf(message,"checking !isInSpill() %d", totevents);
    if(!isInSpill() || 
       (events+pedevents-oldspillevts)==(maxoscevts-1)){
      setTriggerVeto();
      setPedTriggerVeto();
      usleep(200);
      spills++;

      // while(isInSpill());
  
      myFIFOUpdateSpillNr();
        
      if(isTriggerPresent(&trigmask)){

        timestr = time(NULL);
        sprintf(message,"o.o.s. Trigger %d", totevents);
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
        cout << "Oscilloscope samples " << samples << " differ from VME: "
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

      oldspillevts=events+pedevents;

      while(isInSpill()) ;
      unlockTrigger();

      while(!isInSpill() && !abort_run){
	usleep(100);
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
    cout << "Oscilloscope samples " << samples << " differ from VME: "
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

  int i=5;
  while (i)
   {
    std::cout << i-- << "..";
    sleep(1);
   }
  std::cout << i << std::endl;
  return 0;
}

int main(int argc, char** argv)
/****************************************/
{
  uint32_t *theBuffer = new uint32_t[100000];

  v775 ntdc(0x20000000,"/dev/vmedrvb32d16");
  ntdc.romPrint();
  ntdc.print();
  ntdc.swReset();
  ntdc.setFullScale(800);
  std::cout << "PicoSecPerCount = " << ntdc.getPicoSecPerCount() << endl;
  ntdc.printRegisters();

  cout << "\n++++++++++++++++++++++++++++++++++++++++++\n" << endl;

  dlkey();
  for(int i=0; i<4; i++) 
  {
    while( ! ntdc.isDataReady() )
      usleep(200000);
    ntdc.printRegisters();
    uint32_t dataSz = ntdc.readSingleEvent(theBuffer);
    if(!dataSz)
    {
       cerr << "Error from v775" << endl;
       exit(6);
    }
    cout << ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,," << endl;
    for(int j=0; j<dataSz; j++)
      cout << "[" << j << "]\t" << HEXWORD(theBuffer[j]) 
           << " | val =" << v775::dataDecodeValue(theBuffer[j]) << endl; 
    cout << "''''''''''''''''''''''''''''''''''''''''" << endl;

    cout << "DataSize = " << dataSz << "\t NumOfEvent = " << ntdc.evtCnt() << endl;
  }
 /* 
  dlkey();
  xmainx(argc,argv);
 */
  return 0;
}
