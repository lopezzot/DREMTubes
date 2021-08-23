//$Id: mySIS3320.h,v 1.4 2008/07/18 11:11:51 dreamdaq Exp $

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include "myModules.h"
}
// C++ header files
#include <iostream>

//#include "myV262.h"

#define SIS3320_EVT_INF_ALL_OFFSET (0x01000000)
#define SIS3320_EVT_INF_OFFSET(GR) (0x02000000+0x00800000*((GR-1)%2)+0x01000000*((GR-1)/2))

#define SIS3320_CTRL (0x0)
#define SIS3320_ID (0x4)
#define SIS3320_IRQ_CONF (0x8)
#define SIS3320_IRQ_CTRL (0xC)
#define SIS3320_ACQ_CTRL (0x10)
#define SIS3320_START_DELAY (0x14)
#define SIS3320_STOP_DELAY (0x18)
#define SIS3320_MAX_EVTS (0x20)
#define SIS3320_EVT_COUNTER (0x24)
#define SIS3320_MEM_PAGE (0x34)
#define SIS3320_DAC_CTRL (0x50)
#define SIS3320_DAC_DATA (0x54)
#define SIS3320_ADC_GAIN (0x58)
#define SIS3320_RESET (0x400)
#define SIS3320_ARM (0x410)
#define SIS3320_DISARM (0x414)
#define SIS3320_VME_START (0x418)
#define SIS3320_VME_STOP (0x41C)
#define SIS3320_RESET_DDR2 (0x428)

#define SIS3320_EVT_CONF (0x0)
#define SIS3320_SAMPLE_LENGTH (0x4)
#define SIS3320_START_ADDR (0x8)
#define SIS3320_ADC_INPUT (0xC)
#define SIS3320_NEXT_ADDR(ADC) (0x10+(ADC-1)*0x4)
#define SIS3320_ACTUAL_SAMPLE (0x20)
#define SIS3320_TRIGGER_CONF(ADC) (0x30+(ADC-1)*0x8)
#define SIS3320_TRIGGER_THR(ADC) (0x34+(ADC-1)*0x8)
#define SIS3320_TR_CLEAR_COUNT (0x2C)
#define SIS3320_EVT_DIR(ADC) (0x10000+(ADC-1)*0x8000)



#define SIS3320_DATA_OFFSET(GR,ADC) (0x04000000+0x01000000*(GR-1)+0x00800000*(ADC-1))

#define SIS3320_SETJKBITS(OFF,BITS) { \
conf->write32phys(OFF,BITS); \
}

#define SIS3320_GETBITS(OFF,BITS,SHIFT){ \
unsigned int val; \
conf->read32phys(OFF,&val); \
volatile unsigned int vol=val; \
return (vol & BITS) >> SHIFT; \
}

#define SIS3320_GROUPSWITCHALL(GROUP,POINTER){ \
  if(GROUP<0 || GROUP>4){ \
    cout << "SIS3320 Group " << GROUP \
	 << " does not exist for " << __PRETTY_FUNCTION__  <<endl; \
    exit(1); \
  } \
  switch(GROUP){ \
  case 0: \
    POINTER = eventInfoAll; \
    break; \
  case 1: \
    POINTER = eventInfo1; \
    break; \
  case 2: \
    POINTER = eventInfo2; \
    break; \
  case 3: \
    POINTER = eventInfo3; \
    break; \
  case 4: \
    POINTER = eventInfo4; \
    break; \
  } \
}

#define SIS3320_GROUPSWITCH(GROUP,POINTER){ \
  if(GROUP<1 || GROUP>4){ \
    cout << "SIS3320 Group " << GROUP \
	 << " does not exist for " << __func__ <<endl; \
    exit(1); \
  } \
  switch(GROUP){ \
  case 1: \
    POINTER = eventInfo1; \
    break; \
  case 2: \
    POINTER = eventInfo2; \
    break; \
  case 3: \
    POINTER = eventInfo3; \
    break; \
  case 4: \
    POINTER = eventInfo4; \
    break; \
  default: \
    POINTER = NULL; \
    break; \
  } \
}



#define SIS3320_ADCCHECK(NADC){\
if(adc<1 || adc>2){ \
   cout << "SIS3320 ADC " << NADC \
	 << " does not exist for " << __PRETTY_FUNCTION__  <<endl; \
    exit(1); \
} \
}

using namespace std;

/*****************************************/
// The sis3320 class 
/*****************************************/
class sis3320{
  
 public:
  
  // Constructor
  sis3320(unsigned int base, const char *dev, const char *dev2);//, v262 * aIo);    
  
  ~sis3320();

  //Generic Commands
  inline unsigned int id();             // Module ID 
  inline void reset();                  // Module Reset

  //User LED
  void userLEDOn();                     //switch ON user LED
  void userLEDOff();                    //switch OFF user LED

  //Module ID & Firmware
  unsigned int getIdFirmware();         //return module Id & firmware rev


  //IRQ
  void disableIRQ();                    //Disable VME IRQ
 
  // Acquisition modes
  unsigned int dumpAcqReg();
  void enableAutostart();               //Enable autostart mode
  void disableAutostart();              //Disable autostart mode
  void enableMultiEvent();              //MultiEvent mode 
  void disableMultiEvent();             //disable MultiEvent mode
  void enableInternalTriggerStop();     //enable internal trigger as stop
  void disableInternalTriggerStop();    //disable internal trigger as stop
  void enableLEMOStartStop();           //enable front panel start/stop logic
  void disableLEMOStartStop();          //disable front panel start/stop logic
  void setClockInt200();                //set internal clock 200MHz
  void setClockInt100();                //set internal clock 100MHz
  void setClockInt50();                 //set internal clock 50MHz
  void setExternalClockTimes5();        //set external clock x 5
  void setExternalClockTimes2();        //set external clock x 2
  void setExternalRandomClock();        //set external random clock mode
  void setExternalClock();              //set external clock
  unsigned int isSamplingArmed();       //arming status
  unsigned int isSamplingBusy();        //busy status

  //Delays
  void setStartDelay(unsigned int delay); //set delay of the start signal 
  unsigned int getStartDelay();           //return delay of the start signal 
  void setStopDelay(unsigned int delay); //set delay of the stop signal 
  unsigned int getStopDelay();           //return delay of the stop signal 

  //Various conf
  void setMaxNEvents(unsigned int events);//set max events in MultiEvent mode
  unsigned int getMaxNEvents();   //return max events in MultiEvent mode
  unsigned int getActualEventCounter();   //number of events in multi mode
  void setADCPage(unsigned int page);  //select the memory page to be shown on the VME
  unsigned int getADCPage();     //return the memory page available on the VME


  //DAC
  void clearDACs();            //set to middle range DACs
  void loadDACs(unsigned int * values);  //set the DAC values (8 DACs)
  
  //ADC Gain
  void setADCFullRange(unsigned int ch);   //set full input range
  void setADCHalfRange(unsigned int ch);   //set half input range
  unsigned int getADCRange(unsigned int ch);  //1=half 0=full

  //Commands
  void armSampling();      //arms sampling logic
  void disarmSampling();   //disarm sampling logic
  void VMEStartSampling();     //start sampling (if sampling is armed)
  void VMEStopSampling();   //stop sampling
  void resetDDR2();       //reset DDR2 Memory
  
  //Event configuration    group=0 --> all
  unsigned int dumpEventConf(unsigned int group);
  void setMemoryWrap(unsigned int group);      //wrap around memory
  void setPageWrap(unsigned int group);        //wrap around page
  void disableAccumulator(unsigned int group);    //disable accomulator mode
  void enableStopbyLength(unsigned int group);   //enable stop when count==event length reg
  void disableStopbyLength(unsigned int group);   // no stop from sample length
  void setPageSize16M(unsigned int group);      //set apge size to 16MSamples
  void setPageSize4M(unsigned int group);      //set apge size to 4MSamples
  void setPageSize1M(unsigned int group);      //set apge size to 1MSamples
  void setPageSize256k(unsigned int group);      //set apge size to 256kSamples
  void setPageSize64k(unsigned int group);      //set apge size to 64kSamples
  void setPageSize16k(unsigned int group);      //set apge size to 16kSamples
  void setPageSize4k(unsigned int group);      //set apge size to 4kSamples
  void setPageSize1k(unsigned int group);      //set apge size to 1kSamples
  void setPageSize512(unsigned int group);      //set apge size to 512Samples
  void setPageSize256(unsigned int group);      //set apge size to 256Samples
  void setPageSize128(unsigned int group);      //set apge size to 128Samples
  void setPageSize64(unsigned int group);      //set apge size to 64Samples

  void setSampleLength(unsigned int group,
		       unsigned int length);   //set maximum number of samples if max sample count is enabled
  unsigned int getSampleLength(unsigned int group);

  unsigned int setStartAddress(unsigned int group,
			       unsigned int address);   //set memory start address

  unsigned int dumpADCInputMode(unsigned int group);
  void setTestDataMode16(unsigned int group);  //set test mode 16bit
  void setTestDataMode32(unsigned int group);  //set test mode 32bit
  void enableTestMode(unsigned int group);    //enable test mode
  void disableTestMode(unsigned int group);    //disable test mode
  unsigned int setTestStartData(unsigned int group,
				unsigned int data);   //test start data

  unsigned int getNextSampleAddress(unsigned int group,
				    unsigned int adc);  //return next sample address

  unsigned int getActualSample(unsigned int group);  //on the fly value

  //Trigger setup
  void setPeakTime(unsigned int group,
		   unsigned int adc,
		   unsigned int time);//change the peaking time with FIR filter
  
  unsigned int getPeakTime(unsigned int group,
			   unsigned int adc);  //return the peaking time with FIR filter
  void setSumGTime(unsigned int group,
		   unsigned int adc,
		   unsigned int time);//change the SumG time with FIR filter

  unsigned int getSumGTime(unsigned int group,
			   unsigned int adc);  //return the SumG time with FIR filter

  unsigned int getTriggerLength(unsigned int group,
				unsigned int adc);  //return the trigger length with FIR filter

  void setThreshold(unsigned int group,
		    unsigned int adc,
		    unsigned int thr);  //set trigger threshold

  void enableLTTrigger(unsigned int group,
		       unsigned int adc);  //enable lower than trigger

  void disableLTTrigger(unsigned int group,
			unsigned int adc);  //disable lower than trigger

  void enableGTTrigger(unsigned int group,
		       unsigned int adc);  //enable greater than trigger

  void disableGTTrigger(unsigned int group,
			unsigned int adc);  //disable greater than trigger
  
  void setTriggerClearCount(unsigned int group,
			    unsigned int counts);  //set clear trigger register

  unsigned int readData(unsigned int group,
			unsigned int adc,
			unsigned int *buf);

  void  print();

  
 protected:

 private:
  int fd;
  unsigned int ba;
  
  vme * conf;
  vme * eventInfoAll;
  
  vme * eventInfo1;
  vme * eventInfo2;
  vme * eventInfo3;
  vme * eventInfo4;

  //v262 * io;

  /*vme * ADCmem1;
  vme * ADCmem2;
  vme * ADCmem3;
  vme * ADCmem4;
  vme * ADCmem5;
  vme * ADCmem6;
  vme * ADCmem7;
  vme * ADCmem8;*/
};


/**************************************************************/
//  Module ID & Firmware
/**************************************************************/

unsigned int sis3320::getIdFirmware(){
  unsigned int val;  
  conf->read32phys(SIS3320_ID,&val);
  return val; 
}


/**************************************************************/
//  User LED
/**************************************************************/

void sis3320::userLEDOn(){
  SIS3320_SETJKBITS(SIS3320_CTRL,0x1);
}

void sis3320::userLEDOff(){
  SIS3320_SETJKBITS(SIS3320_CTRL,0x10000);
}

/**************************************************************/
//  IRQ
/**************************************************************/

void sis3320::disableIRQ(){
 unsigned int val;
 conf->read32phys(SIS3320_IRQ_CONF,&val);
 val &= ~(0x800);
 conf->write32phys(SIS3320_IRQ_CONF,val);
}

/**************************************************************/
//  Acquisition modes
/**************************************************************/

unsigned int sis3320::dumpAcqReg(){
  unsigned int val;
  conf->read32phys(SIS3320_ACQ_CTRL,&val);
  return val;
}

void sis3320::enableAutostart(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x10);
}
 
void sis3320::disableAutostart(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x100000);
}

void sis3320::enableMultiEvent(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x20);
}
 
void sis3320::disableMultiEvent(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x200000);
}

void sis3320::enableInternalTriggerStop(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x40);
}

void sis3320::disableInternalTriggerStop(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x400000);
}

void sis3320::enableLEMOStartStop(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x100);
}
 
void sis3320::disableLEMOStartStop(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x1000000);
}

void sis3320::setClockInt200(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x70000000);
}

void sis3320::setClockInt100(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x60001000);
}

void sis3320::setClockInt50(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x50002000);
}

void sis3320::setExternalClockTimes5(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x40003000);
}

void sis3320::setExternalClockTimes2(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x30004000);
}

void sis3320::setExternalRandomClock(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x20005000);
}
 
void sis3320::setExternalClock(){
  SIS3320_SETJKBITS(SIS3320_ACQ_CTRL,0x10006000);
}

unsigned int sis3320::isSamplingArmed(){
  SIS3320_GETBITS(SIS3320_ACQ_CTRL,0x10000,16);
}

unsigned int sis3320::isSamplingBusy(){
  SIS3320_GETBITS(SIS3320_ACQ_CTRL,0x20000,17);
}


/*****************************************/
// Delays
/*****************************************/

void sis3320::setStartDelay(unsigned int delay){
  delay=delay&0xFFFFFF;
  conf->write32phys(SIS3320_START_DELAY,delay);
}

unsigned int sis3320::getStartDelay(){
  unsigned int val;
  conf->read32phys(SIS3320_START_DELAY,&val);
  return val;
}
 
void sis3320::setStopDelay(unsigned int delay){
  delay=delay&0xFFFFFF;
  conf->write32phys(SIS3320_STOP_DELAY,delay);
}

unsigned int sis3320::getStopDelay(){
  unsigned int val;
  conf->read32phys(SIS3320_STOP_DELAY,&val);
  return val;
}

/*****************************************/
// Various conf
/*****************************************/

void sis3320::setMaxNEvents(unsigned int events){
  events=events&0xFFFFF;
  conf->write32phys(SIS3320_MAX_EVTS,events);
}

unsigned int sis3320::getMaxNEvents(){
  unsigned int val;
  conf->read32phys(SIS3320_MAX_EVTS,&val);
  return val;
}
 
unsigned int sis3320::getActualEventCounter(){
  unsigned int val;
  conf->read32phys(SIS3320_EVT_COUNTER,&val);
  return val;
}

void sis3320::setADCPage(unsigned int page){
  unsigned int aPage=page&0x7;
  conf->write32phys(SIS3320_MEM_PAGE,aPage);
}

unsigned int sis3320::getADCPage(){
  unsigned int val;
  conf->read32phys(SIS3320_MEM_PAGE,&val);
  return val&0x7;
}

/*****************************************/
// DACs
/*****************************************/

void sis3320::clearDACs(){
  unsigned int val;
  volatile unsigned int volval;

  do{
    conf->read32phys(SIS3320_DAC_CTRL,&val);
    volval=(val&0x8000);
  }while(volval>0);

  conf->write32phys(SIS3320_DAC_CTRL,0x3);

  do{
    conf->read32phys(SIS3320_DAC_CTRL,&val);
    volval=(val&0x8000);
  }while(volval>0);
}

void sis3320::loadDACs(unsigned int * values){
  int i=0;
  unsigned int val;
  volatile unsigned int volval;


  clearDACs();
  
  do{
    conf->read32phys(SIS3320_DAC_CTRL,&val);
    volval=(val&0x8000);
  }while(volval>0);
  
  for(i=0;i<8;i++){
    val=values[7-i]&0xFFFF;
    conf->write32phys(SIS3320_DAC_DATA,val);
    
    do{
      conf->read32phys(SIS3320_DAC_CTRL,&val);
      volval=(val&0x8000);
    }while(volval>0);
    
    conf->write32phys(SIS3320_DAC_CTRL,0x1);
    do{
      conf->read32phys(SIS3320_DAC_CTRL,&val);
      volval=(val&0x8000);
    }while(volval>0);

    conf->write32phys(SIS3320_DAC_CTRL,0x2);

    do{
      conf->read32phys(SIS3320_DAC_CTRL,&val);
      volval=(val&0x8000);
    }while(volval>0);
  }
}
/*****************************************/
// ADCs range
/*****************************************/

void sis3320::setADCFullRange(unsigned int ch){
  if(ch<1 || ch>8){
    cout << "SIS3320 Channel " << ch << " does not exist!!" << endl;
    exit(1);
  }
  unsigned int val;
  conf->read32phys(SIS3320_ADC_GAIN,&val);
  val &= ~(0x1<<(ch-1));
  conf->write32phys(SIS3320_ADC_GAIN,val);
}

void sis3320::setADCHalfRange(unsigned int ch){
   if(ch<1 || ch>8){
    cout << "SIS3320 Channel " << ch << " does not exist!!" << endl;
    exit(1);
  }
  unsigned int val;
  conf->read32phys(SIS3320_ADC_GAIN,&val);
  val |= (0x1<<(ch-1));
  conf->write32phys(SIS3320_ADC_GAIN,val);
}

unsigned int sis3320::getADCRange(unsigned int ch){
   if(ch<1 || ch>8){
    cout << "SIS3320 Channel " << ch << " does not exist!!" << endl;
    exit(1);
  }
  unsigned int val;
  conf->read32phys(SIS3320_ADC_GAIN,&val);
  val &= (0x1<<(ch-1));
  return val;
}

/*****************************************/
//  Event configuration       
/*****************************************/

unsigned int sis3320::dumpEventConf(unsigned int group){
  vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCH(group,access);
  
  access->read32phys(SIS3320_EVT_CONF,&val);
  return val;
}

void sis3320::setMemoryWrap(unsigned int group){
  vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);

  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0x10);
  access->write32phys(SIS3320_EVT_CONF,val);

}

void sis3320::setPageWrap(unsigned int group){
   vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);
  
  access->read32phys(SIS3320_EVT_CONF,&val);
  val |= 0x10;
  access->write32phys(SIS3320_EVT_CONF,val);
}

void sis3320::disableAccumulator(unsigned int group){
  vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);

  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0x100);
  access->write32phys(SIS3320_EVT_CONF,val);
}

void sis3320::enableStopbyLength(unsigned int group){
   vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);
  
  access->read32phys(SIS3320_EVT_CONF,&val);
  val |= 0x20;
  access->write32phys(SIS3320_EVT_CONF,val);
}

void sis3320::disableStopbyLength(unsigned int group){
  vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);

  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0x20);
  access->write32phys(SIS3320_EVT_CONF,val);
}
 
void sis3320::setPageSize16M(unsigned int group){
  vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);
  
  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0xF);
  access->write32phys(SIS3320_EVT_CONF,val);
}

void sis3320::setPageSize4M(unsigned int group){
  vme * access;
  unsigned int val;
  
  SIS3320_GROUPSWITCHALL(group,access);
  
  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0xF);
  val |= (0x1);
  access->write32phys(SIS3320_EVT_CONF,val);
}

void sis3320::setPageSize1M(unsigned int group){
   vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);

  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0xF);
  val |= (0x2);
  access->write32phys(SIS3320_EVT_CONF,val);
}

void sis3320::setPageSize256k(unsigned int group){
   vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);
  
  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0xF);
  val |= (0x3);
  access->write32phys(SIS3320_EVT_CONF,val);
}
 
void sis3320::setPageSize64k(unsigned int group){
   vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);
  
  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0xF);
  val |= (0x4);
  access->write32phys(SIS3320_EVT_CONF,val);
}
 
void sis3320::setPageSize16k(unsigned int group){
  vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);

  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0xF);
  val |= (0x4);
  access->write32phys(SIS3320_EVT_CONF,val);
}

void sis3320::setPageSize4k(unsigned int group){
   vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);
  
  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0xF);
  val |= (0x6);
  access->write32phys(SIS3320_EVT_CONF,val);
}
 
void sis3320::setPageSize1k(unsigned int group){
   vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);

  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0xF);
  val |= (0x7);
  access->write32phys(SIS3320_EVT_CONF,val);
}
 
void sis3320::setPageSize512(unsigned int group){
   vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);
    
  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0xF);
  val |= (0x8);
  access->write32phys(SIS3320_EVT_CONF,val);
}
 
void sis3320::setPageSize256(unsigned int group){
   vme * access;
  unsigned int val;

  SIS3320_GROUPSWITCHALL(group,access);
  
  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0xF);
  val |= (0x9);
  access->write32phys(SIS3320_EVT_CONF,val);
}
 
void sis3320::setPageSize128(unsigned int group){
   vme * access;
  unsigned int val;
  
  SIS3320_GROUPSWITCHALL(group,access);
  
  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0xF);
  val |= (0xA);
  access->write32phys(SIS3320_EVT_CONF,val);
}

void sis3320::setPageSize64(unsigned int group){
  vme * access;
  unsigned int val;
  
  SIS3320_GROUPSWITCHALL(group,access);
  
  access->read32phys(SIS3320_EVT_CONF,&val);
  val &= ~(0xF);
  val |= (0xB);
  access->write32phys(SIS3320_EVT_CONF,val);
}

void sis3320::setSampleLength(unsigned int group,
			      unsigned int length){
  vme * access;
  unsigned int val;
  
  SIS3320_GROUPSWITCHALL(group,access);

  val=(length-4)&0xFFFFFC;
  access->write32phys(SIS3320_SAMPLE_LENGTH,val);
}


unsigned int sis3320::getSampleLength(unsigned int group){
  vme * access;
  unsigned int val;
  
  SIS3320_GROUPSWITCH(group,access);

  access->read32phys(SIS3320_SAMPLE_LENGTH,&val);
  return val;
}

inline unsigned int sis3320::setStartAddress(unsigned int group, 
				      unsigned int address){
  vme * access;
  unsigned int val;
    
  SIS3320_GROUPSWITCHALL(group,access);

  val=address&0xFFFFFC;
  access->write32phys(SIS3320_START_ADDR,val);
  return address&0xFFFFFC;
}

unsigned int sis3320::dumpADCInputMode(unsigned int group){
  vme * access;
  unsigned int val;
  
  SIS3320_GROUPSWITCH(group,access);
  access->read32phys(SIS3320_ADC_INPUT,&val);
  return val;
}
 
void sis3320::setTestDataMode16(unsigned int group){
  vme * access;
  unsigned int val;
  
  SIS3320_GROUPSWITCHALL(group,access);
  
  access->read32phys(SIS3320_ADC_INPUT,&val);
  val &= ~(0x20000);
  access->write32phys(SIS3320_ADC_INPUT,val);
}

void sis3320::setTestDataMode32(unsigned int group){
  vme * access;
  unsigned int val;
  
  SIS3320_GROUPSWITCHALL(group,access);

  access->read32phys(SIS3320_ADC_INPUT,&val);
  val |= 0x20000;
  access->write32phys(SIS3320_ADC_INPUT,val);
}

void sis3320::enableTestMode(unsigned int group){
  vme * access;
  unsigned int val;
  
  SIS3320_GROUPSWITCHALL(group,access);
  
  access->read32phys(SIS3320_ADC_INPUT,&val);
  val |= 0x10000;
  access->write32phys(SIS3320_ADC_INPUT,val);
}

void sis3320::disableTestMode(unsigned int group){
  vme * access;
  unsigned int val;
    
  SIS3320_GROUPSWITCHALL(group,access);

  access->read32phys(SIS3320_ADC_INPUT,&val);
  val &= ~(0x10000);
  access->write32phys(SIS3320_ADC_INPUT,val);
}

unsigned int sis3320::setTestStartData(unsigned int group, unsigned int data){
  vme * access;
  unsigned int val;
  
  SIS3320_GROUPSWITCHALL(group,access);

  data &= 0xFFFF;

  if((data&0xFF)>=0xFE){
    data &= ~(0xFF);
    data += 0x100;
  }

  access->read32phys(SIS3320_ADC_INPUT,&val);
  val &= ~(0xFFFF);
  val &= data;
  access->write32phys(SIS3320_ADC_INPUT,val);
  return data;
}


unsigned int sis3320::getNextSampleAddress(unsigned int group,
					   unsigned int adc){
  vme * access;
  unsigned int val;

  SIS3320_ADCCHECK(adc);
  SIS3320_GROUPSWITCH(group,access);
  
  access->read32phys(SIS3320_NEXT_ADDR(adc),&val);
  return val;
}

unsigned int sis3320::getActualSample(unsigned int group){
  vme * access;
  unsigned int val;
  
  SIS3320_GROUPSWITCH(group,access);
  
  access->read32phys(SIS3320_ACTUAL_SAMPLE,&val);
  return val;
}

/*****************************************/
//Trigger setup
/*****************************************/
void sis3320::setPeakTime(unsigned int group,
			  unsigned int adc,
			  unsigned int time){
  vme * access;
  unsigned int val;
  
  SIS3320_ADCCHECK(adc);
  SIS3320_GROUPSWITCH(group,access);

  access->read32phys(SIS3320_TRIGGER_CONF(adc),&val);
  time=time&0x1F;
  val &= ~(0x1F);
  val &= time;
  access->write32phys(SIS3320_TRIGGER_CONF(adc),val);
}
  
unsigned int sis3320::getPeakTime(unsigned int group,
				  unsigned int adc){
  vme * access;
  unsigned int val;

  SIS3320_ADCCHECK(adc);
  SIS3320_GROUPSWITCH(group,access);

  access->read32phys(SIS3320_TRIGGER_CONF(adc),&val);
  return val&0x1F;
}

void sis3320::setSumGTime(unsigned int group,
			  unsigned int adc,
			  unsigned int time){
  vme * access;
  unsigned int val;

  SIS3320_ADCCHECK(adc);
  SIS3320_GROUPSWITCH(group,access);

  access->read32phys(SIS3320_TRIGGER_CONF(adc),&val);
  time=(time&0x1F)<<8;
  val &= ~(0x1F00);
  val &= time;
  access->write32phys(SIS3320_TRIGGER_CONF(adc),val);
}

unsigned int sis3320::getSumGTime(unsigned int group,
				  unsigned int adc){
  vme * access;
  unsigned int val;

  SIS3320_ADCCHECK(adc);
  SIS3320_GROUPSWITCH(group,access);

  access->read32phys(SIS3320_TRIGGER_CONF(adc),&val);
  return (val&0x1F00)>>8;
}

unsigned int sis3320::getTriggerLength(unsigned int group,
				       unsigned int adc){
  vme * access;
  unsigned int val;

  SIS3320_ADCCHECK(adc);
  SIS3320_GROUPSWITCH(group,access);

  access->read32phys(SIS3320_TRIGGER_CONF(adc),&val);
  return (val&0xFF0000)>>16;
}

void sis3320::setThreshold(unsigned int group,
			   unsigned int adc,
			   unsigned int thr){
  vme * access;
  unsigned int val;

  SIS3320_ADCCHECK(adc);
  SIS3320_GROUPSWITCH(group,access);

  access->read32phys(SIS3320_TRIGGER_THR(adc),&val);
  thr &= 0x1FFFF;
  val &= ~(0x1FFFF);
  val |= thr;
  access->write32phys(SIS3320_TRIGGER_THR(adc),val);

}

void sis3320::enableLTTrigger(unsigned int group,
			      unsigned int adc){
  vme * access;
  unsigned int val;

  SIS3320_ADCCHECK(adc);
  SIS3320_GROUPSWITCH(group,access);

  access->read32phys(SIS3320_TRIGGER_THR(adc),&val);
  val |= 0x1000000;
  access->write32phys(SIS3320_TRIGGER_THR(adc),val);
}

void sis3320::disableLTTrigger(unsigned int group,
			       unsigned int adc){
  vme * access;
  unsigned int val;

  SIS3320_ADCCHECK(adc);
  SIS3320_GROUPSWITCH(group,access);

  access->read32phys(SIS3320_TRIGGER_THR(adc),&val);
  val &= ~(0x1000000);
  access->write32phys(SIS3320_TRIGGER_THR(adc),val);
}

void sis3320::enableGTTrigger(unsigned int group,
			      unsigned int adc){
  vme * access;
  unsigned int val;

  SIS3320_ADCCHECK(adc);
  SIS3320_GROUPSWITCH(group,access);

  access->read32phys(SIS3320_TRIGGER_THR(adc),&val);
  val |= 0x2000000;
  access->write32phys(SIS3320_TRIGGER_THR(adc),val);
}

void sis3320::disableGTTrigger(unsigned int group,
			       unsigned int adc){
  vme * access;
  unsigned int val;

  SIS3320_ADCCHECK(adc);
  SIS3320_GROUPSWITCH(group,access);

  access->read32phys(SIS3320_TRIGGER_THR(adc),&val);
  val &= ~(0x2000000);
  access->write32phys(SIS3320_TRIGGER_THR(adc),val);
}
  
void sis3320::setTriggerClearCount(unsigned int group,
				   unsigned int counts){
  vme * access;
  
  SIS3320_GROUPSWITCH(group,access);

  access->write32phys(SIS3320_TR_CLEAR_COUNT,counts);
}

/*****************************************/
// Commands
/*****************************************/
inline void sis3320::armSampling(){
  conf->write32phys(SIS3320_ARM,0x0);
}
  
inline void sis3320::disarmSampling(){
  conf->write32phys(SIS3320_DISARM,0x0);
}

inline void sis3320::VMEStartSampling(){
  conf->write32phys(SIS3320_VME_START,0x0);
}

inline void sis3320::VMEStopSampling(){
  conf->write32phys(SIS3320_VME_STOP,0x0);
}
 
inline void sis3320::resetDDR2(){
  conf->write32phys(SIS3320_RESET_DDR2,0x0);
}

inline unsigned int sis3320::readData(unsigned int group,
				      unsigned int adc,
				      unsigned int *buf){
  vme * access;
  unsigned int val;
  unsigned int ndata;
  unsigned int rwords;

  SIS3320_ADCCHECK(adc);
  SIS3320_GROUPSWITCH(group,access);

  //read directory to know how many data
  access->read32phys(SIS3320_EVT_DIR(adc),&val);
  ndata=val&0x1FFFFFC;
  if (ndata==0) return 0;

  errno=0;
  lseek(fd,ba+SIS3320_DATA_OFFSET(group,adc), SEEK_SET);         
  if (errno) {
    cout <<  __PRETTY_FUNCTION__ <<" cannot lseek file descriptor " 
	 << fd << endl;
    exit(-1) ;
  }
  
  *buf++=0xABCD0000+(group-1)*2+adc;

  //io->enableNIMOut(1);
  rwords = read(fd,buf,2*ndata)/sizeof(unsigned int);
  //io->disableNIMOut(1);

  *(buf+rwords)=0xABCDDBCA;
  
  return rwords+2;
}

/*****************************************/
// Constructor
/*****************************************/
sis3320::sis3320(unsigned int base, const char *dev, const char *dev2)//,v262 *aIo)
{ 
  //io=aIo;

  ba=base;
  
  conf = new vme(base,0x500,dev);
  
  eventInfoAll = new vme(base+SIS3320_EVT_INF_ALL_OFFSET,0xF,dev);
  
  eventInfo1 = new vme(base+SIS3320_EVT_INF_OFFSET(1),0x20000,dev);
  eventInfo2 = new vme(base+SIS3320_EVT_INF_OFFSET(2),0x20000,dev);
  eventInfo3 = new vme(base+SIS3320_EVT_INF_OFFSET(3),0x20000,dev);
  eventInfo4 = new vme(base+SIS3320_EVT_INF_OFFSET(4),0x20000,dev);

  errno = 0;
  fd = open(dev2, O_RDWR);
  if (errno) {
    cout << __PRETTY_FUNCTION__ <<" cannot open device " << dev2 << endl;
    exit(1);
  }

  lseek(fd, 0, SEEK_SET);         // Set offset of file descriptor to zero
  if (errno) {
    cout <<  __PRETTY_FUNCTION__ <<" cannot lseek file descriptor " 
	 << fd << endl;
    exit(-1) ;
  }
}


sis3320::~sis3320(){

  delete conf;

  delete eventInfoAll;

  delete eventInfo1;
  delete eventInfo2;
  delete eventInfo3;
  delete eventInfo4;

  close(fd);
}

/*****************************************/
// Generic Commands
/*****************************************/

inline unsigned int sis3320::id() { return conf->getBaseAddr() | ID_SIS3320; }
inline void sis3320::reset()    { conf->write32phys(SIS3320_RESET, 0x0); }


/*****************************************/
void sis3320::print() 
/*****************************************/
{
  cout << "************************************" << endl;  
  cout << " SIS3320 Module  /  base " << conf->getBaseAddr() 
       << ", mmap " << conf->getBuffer() << ", length " << conf->getLength() << endl;
  cout << "************************************" << endl;
  return;
}
