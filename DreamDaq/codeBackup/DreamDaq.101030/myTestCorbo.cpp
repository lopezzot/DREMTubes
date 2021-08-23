//$Id: myTestCorbo.cpp,v 1.4 2006/08/15 17:07:05 cvsdream Exp $
//$Id: myTestCorbo.cpp,v 1.4 2006/08/15 17:07:05 cvsdream Exp $
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
#include <unistd.h>           // Needed for usleep
#include "myFIFO-IO.h"            // Needed for my I/O stuff
#include "myBar.h"            // Needed for the progress bar
#include "myThread.h"    // Needed to read Slow Control Data
#include "errno.h"
#include <sys/resource.h>
#include <sys/time.h>
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
#include "myVme.h"       // VME Base class
#include "myCorbo.h"      // Creative Electronic Systems RCB 8047 CORBO class derived from VME
//#include "myV792AC.h"    // CAEN ADC V792AC class derived from VME
#include "myKLOETDC.h"    // 32ch KLOETDC class derived from VME
#include "mySIS3320.h"   // Flash ADC
#include "myL1182.h"      // LeCroy ADC
#include "myV262.h"      //CAEN V262 I/O REG
#include "myV260.h"      //CAEN V260 Scaler
#include "myL1176.h"     //LeCroy L1176 TDC
#include "myModules.h"

using namespace std;

/****************************************/
// Declare all the hardware
/****************************************/
//corbo   corbo0(0xfff000,"/dev/vmedrv24d16");
l1182 adc5(0x010000,"/dev/vmedrvb24d16");
l1182 adc4(0x020000,"/dev/vmedrvb24d16");
l1182 adc3(0x030000,"/dev/vmedrvb24d16");
l1182 adc2(0x040000,"/dev/vmedrvb24d16");
l1182 adc1(0x050000,"/dev/vmedrvb24d16");
l1182 adc0(0x060000,"/dev/vmedrvb24d16");
v262  io(0x400000,"/dev/vmedrvb24d16");
sis3320 fadc(0x20000000,"/dev/vmedrvb32d32","/dev/vmedrvb32d32");//,&io);
kloetdc tdc1(0x7C000000,"/dev/vmedrv32d32");
v260 scaler0(0x200000,"/dev/vmedrvb24d16");
l1176 tdc0(0x300000,"/dev/vmedrvb24d32");

/****************************************/

unsigned int * buf;
unsigned int * bufsize;
unsigned int event,skipped;

struct timeval tv0,tv1;
struct timezone tz;

unsigned int spills;
bool abort_run=false;

void cntrl_c_handler(int sig)
{
  cout << endl << "Got control-c, end of current run" << endl << endl;
  abort_run = true;
}


void initIO(){
  io.reset();
}

void setTriggerVeto(){
  io.enableNIMOut(0);
}

void clearTriggerVeto(){
  io.disableNIMOut(0);
}

unsigned short isInSpill(){
  return io.getNIMIn(2);
}

unsigned short isHWBusy(){
  return io.getNIMIn(1);
}

void unlockTrigger(){
  io.NIMPulse(0);
}

unsigned short isTriggerPresent(){
  return io.getNIMIn(0);
}

void initFlash(){
  
  unsigned int i;

  fadc.reset();
  
  fadc.disableIRQ();
  fadc.setClockInt200();
  
  //Acq conf
  fadc.enableInternalTriggerStop();
  fadc.disableAutostart();
  fadc.enableLEMOStartStop();

  // fadc.clearDACs();

  unsigned int dac[8];
  for(i=0;i<8;i++)
    dac[i]=0xB000;
  fadc.loadDACs(dac);

  fadc.setADCFullRange(1);
  fadc.setADCFullRange(2);
  fadc.setADCFullRange(3);
  fadc.setADCFullRange(4);
  fadc.setADCFullRange(5);
  fadc.setADCFullRange(6);
  fadc.setADCFullRange(7);
  fadc.setADCFullRange(8);

  //Event conf
  fadc.enableStopbyLength(0);
  fadc.setMemoryWrap(0);
  fadc.disableAccumulator(0);
  
  //fadc.setStartDelay(10);
  fadc.setADCPage(0);
  
  fadc.setSampleLength(1,32);
  fadc.setSampleLength(2,32);
  fadc.setSampleLength(3,32);
  fadc.setSampleLength(4,32);
  
  //Input Mode
  //fadc.enableTestMode(1);
  //fadc.setTestDataMode16(1);
  //fadc.setTestStartData(1,0xABCD);
  //cout << "Input mode 0x" << hex << fadc.dumpADCInputMode(1) << dec<< endl; 

  fadc.setStartAddress(1,0);
  fadc.setStartAddress(2,0);
  fadc.setStartAddress(3,0);
  fadc.setStartAddress(4,0);
  
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

unsigned int myReadFlash(unsigned int *buf){

  unsigned int i,j;
  unsigned int size=0;

  for(i=1;i<5;i++)
    for(j=1;j<3;j++)
      size+=fadc.readData(i,j,&buf[size/sizeof(unsigned int)]);


  return size;
}

void clearADCs(){
  adc0.clear();
  adc1.clear();
  adc2.clear();
  adc3.clear();
  adc4.clear();
  adc5.clear();
}

void l1176Init(){
  tdc0.setCommStop();
  tdc0.enableRisingEdge();
  tdc0.disableFallingEdge();
  tdc0.setTriggerPulse();
  //tdc0.enableAuxCommonHit();
  tdc0.enableAcq();
  //cout << "CSR0 0x" << hex << tdc0.getCSR0() << dec << endl;
}

void myReadOutInit(){
  initFlash();

  tdc1.initialise();

  l1176Init();

  clearADCs();
  adc0.selectFrontPanelGate();
  adc1.selectFrontPanelGate();
  adc2.selectFrontPanelGate();
  adc3.selectFrontPanelGate();
  adc4.selectFrontPanelGate();
  adc5.selectFrontPanelGate();

  scaler0.clear();
  scaler0.disableIRQ();
  scaler0.inhibitReset();
}

void myReadOutArm(){
  tdc0.clear();
  
  clearADCs();
  
  fadc.setStartAddress(1,0);
  fadc.setStartAddress(2,0);
  fadc.setStartAddress(3,0);
  fadc.setStartAddress(4,0);
  fadc.armSampling();
}

int myReadEvent(){
  
  unsigned int size=0;

  myNewEvent();
  
  myFormatSubEvent(tdc0.id());

  size=tdc0.readData(&buf[*bufsize]);

  myUpdateSubEvent(size);

  //io.enableNIMOut(2);

  //myFormatSubEvent(tdc1.id());

  //size=tdc1.readEvent(&buf[*bufsize]);

  //myUpdateSubEvent(size);

  myFormatSubEvent(fadc.id());

  size=myReadFlash(&buf[*bufsize]);

  myUpdateSubEvent(size);
  
  //io.disableNIMOut(2);
  
  myFormatSubEvent(adc0.id());
  
  size=adc0.readEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);

  //io.enableNIMOut(2);

  myFormatSubEvent(adc1.id());
  
  size=adc1.readEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);

  //io.disableNIMOut(2);

  myFormatSubEvent(adc2.id());
  
  size=adc2.readEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);

  //io.enableNIMOut(2);

  myFormatSubEvent(adc3.id());
  
  size=adc3.readEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);

  //io.disableNIMOut(2);

  myFormatSubEvent(adc4.id());
  
  size=adc4.readEvent(&buf[*bufsize]);

  myUpdateSubEvent(size);

  //io.enableNIMOut(2);
  

  myFormatSubEvent(adc5.id());
  
  size=adc5.readEventMasked(&buf[*bufsize],0xF);

  myUpdateSubEvent(size);

  //io.disableNIMOut(2);

  myFormatSubEvent(scaler0.id());

  size=scaler0.readEventMasked(&buf[*bufsize],0xB);

  myUpdateSubEvent(size);

  //io.enableNIMOut(2);
  return 0;
}

int myTrigger(){
  //int i,j;

  while(isHWBusy() && !abort_run);
            
  //io.disableNIMOut(2);
  
  if(!abort_run){
    myReadEvent();
    
    myWriteEvent(spills);
  //}else
  //skipped++;
  }
  return 0;
}

/****************************************/
int main()
/****************************************/
{
  unsigned int maxevts;
  //unsigned int i;

  signal(SIGINT, cntrl_c_handler);      // Control-C handler

  ios::sync_with_stdio();          // To syncronize with stdio library (really needed?)
 
  initIO();
  setTriggerVeto();
  
  myReadOutInit();
   
  myOpenRun();

  buf = myBuffer(&bufsize);

  maxevts=myMaxEvents();

  event=0;

  struct timeval  tv0,tv1; 
  struct timezone tz;
  unsigned int sum=0;
  //abort_run=true;
  //maxevts=2;
  
  myReadOutArm();
  unlockTrigger();
  spills=0;
  while(!isInSpill());
  gettimeofday(&tv0,&tz);
  clearTriggerVeto();
    
  while(event<maxevts && !abort_run){
    
    if(isTriggerPresent()){
      //io.enableNIMOut(1);
      //io.enableNIMOut(2);

      myTrigger();
      
      gettimeofday(&tv1,&tz);
      
      sum += (tv1.tv_sec-tv0.tv_sec)*1000000+tv1.tv_usec-tv0.tv_usec;
      
      gettimeofday(&tv0,&tz);
      event++;
      
      //io.disableNIMOut(2);
      //io.disableNIMOut(1);
      myReadOutArm();
      unlockTrigger();
    }
    
    if(!isInSpill()){
      setTriggerVeto();
      spills++;
      
      if(isTriggerPresent()){
	myTrigger();
	event++;
	unlockTrigger();
      }
      
      double occ=100*myFIFOOccupancy();

      myFIFOunlock();
      
      cout << "Events in spill " << spills <<": "<< event 
	   << " - Buffer occupancy "<< occ << "%"<< endl;
      
      while(!isInSpill() && !abort_run)
	usleep(100);
      
      myFIFOlock();

      myReadOutArm();
      clearTriggerVeto();
      gettimeofday(&tv0,&tz);
    }

  }

  myCloseRun();
  
  cout<< endl;
  cout << "Events: " << event << endl;

  cout << "Average time/event: " << sum/event << " usec" <<endl;
  //cout << "count " << count << endl;

  return 0;
}
