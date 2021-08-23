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
#include "myTH03.h"
#include "myTekOsc.h"
#include "myV258.h"
#include "myV512.h"
#include "myV513.h"
#include "myTriggerNumber.h"
#include "dreamudp.h"
#include "myV775.h"
#include "myV862.h"

#include <vector>
//#define PROFILING
//#define USE_TEK_TDS7254B
//#define UDPSYNC
//#define USE_TH03

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

v792ac adc0(0x4000000,"/dev/vmedrv32d32");
//v792ac adc1(0x5000000,"/dev/vmedrv32d32");
//v792ac adc2(0x6000000,"/dev/vmedrv32d32");
/*v792ac adc3(0x7000000,"/dev/vmedrv32d32");
v792ac adc4(0x8000000,"/dev/vmedrv32d32");
v792ac adc5(0x9000000,"/dev/vmedrv32d32");
v792ac adc6(0xA000000,"/dev/vmedrv32d32");
v792ac adc7(0xB000000,"/dev/vmedrv32d32");*/

v792ac* v792s[] = {&adc0,};//,&adc3,&adc4,&adc5,&adc6,&adc7};
unsigned int n_v792 = 1;

v260 scaler0(0x200000,"/dev/vmedrv24d16");
//l1176 tdc0(0x300000,"/dev/vmedrvb24d32");
//v258 disc(0xFF0000, "/dev/vmedrv24d16");
//v512 plu(0x20000000,"/dev/vmedrv32d16");

v775 ntdc(0x20000000,"/dev/vmedrv32d16");

//v862 nadc0(0x40000000,"/dev/vmedrvb32d16");

#ifndef IOREG_IS_V513
#define IOREG_IS_V262
 v262  io(0x400000,"/dev/vmedrv24d16");
 v262  io2(0x800000,"/dev/vmedrv24d16");
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
struct th03_data th03;
unsigned int trigpipo;

dreamudp dsrv;

uint32_t evinfo[2]={0,0};
char message[256];
time_t timestr;
bool tryonce = true;
void cntrl_c_handler(int sig)
{
#ifdef XXX_UDPSYNC
  if (tryonce && (strstr(message,"Waiting DRS") == message)){
    tryonce = false;
    dsrv.sendcommand(&evinfo,2*sizeof(uint32_t));
    return;
  }
#endif // UDPSYNC
  char * stime = asctime(localtime(&timestr));
  stime[24] = 0;
  fprintf(stdout,"%s cntrl_c_handler: %s\n\n", stime, message);
  fprintf(stdout,"aborting run\n");
  abort_run = true;
}

void storemessage(const char* msg){
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
  //adcl.clear();
}

/*
void l1176Init(){
  
  //clear
  tdc0.clear();
  //sets common stop mode
  //tdc0.setCommStop();
  //sets common start mode
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

  }*/

void initV792(){
  
  for(size_t i=0; i < n_v792; ++i){
    v792s[i]->reset();
    v792s[i]->disableSlide();
    v792s[i]->disableOverflowSupp();
    v792s[i]->disableZeroSupp();

    unsigned short ped;
    ped = v792s[i]->getI1();
    cout << "adc" << i << ": Default pedestal value is " << ped << endl;
    
    ped = 255;
    v792s[i]->setI1(ped);  // Set I1 current to define pedestal position
    ped =v792s[i]->getI1();// in the range [0x00-0xFF] (see manual par. 4.34.2)
    cout << "Now adc" << i << " ped is set to " << ped << endl;
  }
  
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

  //l1176Init();

  clearADCs();
  
  //sets ADC gate from fron panel
  //adcl.selectFrontPanelGate();
  
  
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

  
  ntdc.swReset();
  ntdc.setFullScale(1000);
  ntdc.printRegisters();
  

  /*  nadc0.swReset();
  for(size_t i=1; i<32; ++i)
    nadc0.channel(i,false);
  
  nadc0.overflowSuppression(false);
  nadc0.zeroSuppression(false);
  nadc0.iPed(255);
  */
#ifdef EVENT_NUMBER_IS_ON
  evtnum.reset();
#endif

}

void myReadOutArm(){
  //clear memory content
  //tdc0.clear();
    
  clearADCs();
  
  for(size_t i=0; i < n_v792; ++i){
    v792s[i]->clearData();
  }
  
  ntdc.dataReset();

  //nadc0.clearData();

  /*fadc.setStartAddress(1,0);
  fadc.setStartAddress(2,0);
  fadc.setStartAddress(3,0);
  fadc.setStartAddress(4,0);
  fadc.armSampling();*/
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

#ifdef PROFILING
  onoff = !onoff;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif
  */

  for(size_t i=0; i < n_v792; ++i){

    myFormatSubEvent(v792s[i]->id());

    size=v792s[i]->readEvent(&buf[*bufsize]);

    myUpdateSubEvent(size);
  
    #ifdef PROFILING
    onoff = !onoff;
    (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
    #endif
  }

  /*
  myFormatOscSubEvent(nadc0.id());

  size = nadc0.readSingleEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);
  
#ifdef PROFILING
  onoff = !onoff;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
#endif
  */

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

#ifdef USE_TH03
  myFormatSubEvent(0xFFFF);

  buf[*bufsize]=th03.refs;
  buf[*bufsize+1]=th03.ch12;
  buf[*bufsize+2]=th03.ch3;

  myUpdateSubEvent(3);
#endif //USE_TH03


#ifdef PROFILING
  bool onoff = false;
  (onoff) ? io.enableNIMOut(2) : io.disableNIMOut(2);
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
  pthread_t tid;
  bool ped_enabled=true;
  bool phys_enabled=true;
  unsigned int phypedratio;
  unsigned int oldevents=0;

#ifdef USE_TEK_TDS7254B
  unsigned int oscpts;
  unsigned int oscchmask;
#endif
  

#ifdef UDPSYNC
#define PORTNR 60888
  uint16_t portn = (argc > 1) ? atol(argv[1]) : 60888;
  cout << " using portn " << portn << endl;
  dsrv.open("pcpisadrc",portn);
#endif // UDPSYNC

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

#ifdef USE_TH03
  //Starts the TH03 handler
  myTH03Handler(&tid,&th03);
#endif // USE_TH03

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

#ifdef USE_TH03
  myTH03wait(&th03);
#endif //USE_TH03

#ifdef UDPSYNC
  dsrv.connect();

  char command[256];
  do
   {
    cout << " size of get command " << dsrv.getcommand(command,256) << endl;
    cout << " command is " << command << endl;
    if (strcmp(command,"RunNr")==0)
     {
      dsrv.sendcommand(&runnr,sizeof(runnr));
     }
    else if (strcmp(command,"EventNr")==0)
     {
      uint32_t nevts = maxevts+maxpedevts;
      dsrv.sendcommand(&nevts,sizeof(nevts));
     }
    else if (strcmp(command,"Ready")==0)
      break;
   }
  while(1);

#endif // UDPSYNC

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

#ifdef UDPSYNC
      uint32_t evinfo[2];
      evinfo[0] = trigmask | (spills<<16);
      evinfo[1] = totevents;
      dsrv.sendcommand(&evinfo,2*sizeof(uint32_t));
#endif // UDPSYNC

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
#ifdef UDPSYNC
      timestr = time(NULL);
      sprintf(message,"Waiting DRS %d", totevents);
      dsrv.getcommand(command,256);
      if (strcmp(command,"Ready")!=0)
        printf("wrong command from drs %s\n",command);
      timestr = time(NULL);
      sprintf(message,"DRS ready %d", totevents);
#endif // UDPSYNC
      
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

#ifdef UDPSYNC
	uint32_t evinfo[2];
	evinfo[0] = trigmask | (spills<<16);
	evinfo[1] = totevents;
	dsrv.sendcommand(&evinfo,2*sizeof(uint32_t));
#endif // UDPSYNC

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
#ifdef UDPSYNC
        timestr = time(NULL);
        sprintf(message,"o.o.s. Waiting DRS %d", totevents);
        dsrv.getcommand(command,256);
        if (strcmp(command,"Ready")!=0)
          printf("wrong command from drs %s\n",command);
#endif // UDPSYNC
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

#ifdef UDPSYNC
  if(abort_run){
    uint32_t evinfo[2];
    evinfo[0] = 0xFFFF;
    evinfo[1] = totevents;
    dsrv.sendcommand(&evinfo,2*sizeof(uint32_t));
  }
#endif // UDPSYNC  

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

#ifdef USE_TH03
  //Close the TH03 thread
  myTH03Stop(tid,&th03);
#endif // USE_TH03

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
