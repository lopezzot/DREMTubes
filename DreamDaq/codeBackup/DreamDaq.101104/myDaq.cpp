//$Id: myDaq.cpp,v 1.3 2007/06/13 10:02:23 dreamdaq Exp $
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
#include "myIO.h"             // Needed for my I/O stuff
#include "myBar.h"            // Needed for the progress bar
#include "myThread.h"    // Needed to read Slow Control Data
}

/****************************************/
// C++ header files
/****************************************/
#include <iostream>
#include <fstream>
#include <signal.h>
using namespace std;

/****************************************/
// My header files with all classes
/****************************************/
#include "myVme.h"       // VME Base class
#include "myV767A.h"     // CAEN TDC V767A class derived from VME
#include "myV488.h"      // CAEN TDC V488 class derived from VME
#include "myV260.h"      // CAEN scaler V260 class derived from VME
#include "myV513.h"      // CAEN Input Output Register V513 class derived from VME
#include "myV792AC.h"    // CAEN ADC V792AC class derived from VME


/****************************************/
// Declare all the hardware
/****************************************/
v488    tdc0(0x0D0000,"/dev/vmedrv24d16");
v767a   tdc1(0x010000,"/dev/vmedrv24d16");
v767a   tdc2(0x020000,"/dev/vmedrv24d16");
v767a   tdc3(0x030000,"/dev/vmedrv24d16");
v260    sca1(0x0B0000,"/dev/vmedrv24d16");
v513    dio1(0x0C0000,"/dev/vmedrv24d16");
v792ac  adc1(0x040000,"/dev/vmedrv24d16");

/****************************************/

// Global variables
unsigned int buf[10000];        // Probably really too big...
int evsiz = 0;
int nevt = 0;
int abort_run = 0;

/****************************************/
void cntrl_c_handler(int sig)
/****************************************/
{
  cout << endl << "Got control-c, end of current run" << endl << endl;
  abort_run = 1;
}

/****************************************/
void bufdump(unsigned int *buf, int size)
/****************************************/
{
  for (int i=0; i<size; i++) 
    cout << dec << i << " Data " << hex << buf[i] << dec << endl;
  return;
}

/****************************************/
void myTDC0Init()
/****************************************/
{
  int wr = 0x70;   // Time Range for tdc0, 0x0 = 90ns, 0xE0 = 770 ns

  tdc0.reset();
  tdc0.setCommonStop();
  usleep(100);
  tdc0.setHalfFullModeOB();
  usleep(100);
  tdc0.enableChannel(0);
  tdc0.enableChannel(1);
  tdc0.enableChannel(2);
  tdc0.enableChannel(3);
  for(int i=4;i<8;i++) {
    tdc0.disableChannel(i);
  }
  usleep(100);
  tdc0.setRange(wr);
  usleep(100);
  tdc0.setHighThr(0xC0);
  tdc0.setLowThr(0x0);  
}
 



/****************************************/
int checkTDCRef(unsigned int *buf, int size)
/****************************************/
{
  unsigned int data, type;
  int ch, tm, st, ed, ge=0, er=0, ev, sz;
  int ref0, ref1, err;
  int rc = 0;           // This means no errors
  int hd = 0;

  ref0 = 0;
  ref1 = 0;
  err  = 0;

  for (int i=0; i<size; i++) {

    data = buf[i];
    type = (data)>>21&0x3;
    
    switch(type) {
    case 0:             /* Datum */
      ch = (data>>24)&0x7F;
      tm = data&0xFFFFF;
      st = (data>>23)&0x1;
      ed = (data>>20)&0x1;
      if ((ch == 63) && (ed == 1)) ref1 = 1;       // Ref (ch63): leading edge found
      if ((ch == 63) && (ed == 0)) ref0 = 1;       // Ref (ch63): trailing edge found
      //      cout << "<DATA> Ch " << ch << " Time " << tm << " Status " << st << " Edge " << ed << endl;
      break;
    case 1:             /* Trailer */
      ge = (data>>27);
      er = (data>>24)&0x7;
      sz = data&0xFFFF;
      //      cout << "<TRAILER> Geo " << ge << " Error " << er << " Size " << sz << endl;
      break;
    case 2:             /* Header */
      ge = data>>27;
      ev = data&0xFFF;
      hd++;
      //      cout << "<HEADER> Geo " << ge << " Event " << ev << endl;
      break;
    }
  }
      
  // Data taken in some conditions are just crap, so print some warnings...

  if (hd>1) {
    cout << "Found " << hd << " events on TDC at geo " << ge << " at event " << nevt << endl;
    rc = -1;
  }

  //  if (!(ref0*ref1)) {
  if (!ref1) {
    cout << "No reference hit found on TDC at geo " << ge << " at event " << nevt << endl;
    rc = ge;
    switch(ge) {
    case 4:
       tdc1.clear();          // necessario ????????????
      break;
    case 6:
       tdc2.clear();          // necessario ????????????
      break;
    case 31:
       tdc3.clear();          // necessario ????????????
      break;
    }
  }

  if (er) {
    cout << "Bad Status (0x" << er << ") in data from TDC at geo " << ge << " at event " << nevt << endl;
    unsigned int ec;
    rc = -1*ge;
    switch(ge) {
    case 4:
      ec = tdc1.readTdcErrorCode();
      tdc1.clear();
      break;
    case 6:
      ec = tdc2.readTdcErrorCode();
      tdc2.clear();
      break;
    case 31:
      ec = tdc3.readTdcErrorCode();
      tdc3.clear();
      break;
    default: ec = 0;
    }

    cout << "Error on TDCs at geo " << ge << " at event " << nevt << " Error Code " << hex << ec << dec << " - CLEARED!" << endl;
  }
  return rc;
}


/****************************************/
int checkTDC0(unsigned int *buf, int sz)
/****************************************/
{
  int i, tdc, ch, nch, evn, hd, inv, det, dat;
  unsigned int data;

  dat=0;
  det=0;
  hd=0;
  evn=0;
  inv=0;
  for (i=0; i<sz; i++) {
    data = buf[i];
    switch((data&0x8000)>>15) {
    case 1:
      nch=((data&0x7000)>>12)+1;
      evn=(data&0xFFF);
      //      cout << "<HEADER> Number of ch converted: " << nch  <<", ev number: " << evn << endl;
      hd++;
      break;     
    case 0:
      ch=(data&0x7000)>>12;
      tdc=(data&0xFFF);    
      //      cout << "<DATA> TDC0 on Ch: " << ch << " | tdc: " << tdc << endl;
      dat++;
      //      cout << dat << endl;
      break;
    default:
      //      cout << "<INVALID> data "<<hex<<data<<endl; 
      inv++;
      break;
    }
  }


  //  if ( hd  != 1)                                                 det |= 0x1; 
  //  if ( evn != 1)                                                 det |= 0x2;
  //  if ( dat != 3)                                                 det |= 0x4; 
  if ( inv != 0)                                                 det |= 0x8; 

  if (sz < 1) {
    cout << "TDC0: size is " << sz << " at event " << nevt << endl;
  }
  return det;
}





/****************************************/
int checkADC(unsigned int *buf, int sz)
/****************************************/
{
  int i;
  int adc, un, ov, cha, geo, nch, cra;
  unsigned int data;
  int evn=0;
  int hd =0;
  int det=0;
  int eob=0;
  int dat=0;
  int inv=0;
  int ich0=0, ich1=0, ich2=0, ich3=0;
  //  int ich16=0, ich17, ich18=0, ich19=0, ich20=0, ich21, ich22=0, ich23=0;
  //  int ich24=0, ich25, ich26=0, ich27=0, ich28=0, ich29, ich30=0, ich31=0;

  for (i=0; i<sz; i++) {
    data = buf[i];
    switch((data>>24)&0x7) {
    case 0x0: // DATA
      adc = data&0xFFF;
      ov  = (data>>12)&0x1;
      un  = (data>>13)&0x1;
      cha = (data>>16)&0x3F;
      geo = (data>>27)&0x1F;
      //      cout << "<DATA> geo "<<dec<<geo<<" cha "<<cha<<" adc "<<adc<<" ov "<<ov<<" un "<<un<<endl; 
      if (cha == 0) ich0++; 
      if (cha == 1) ich1++; 
      if (cha == 2) ich2++; 
      if (cha == 3) ich3++; 


      dat++;
      break;
    case 0x2: // HEADER
      nch = (data>>8)&0x3F;
      cra = (data>>16)&0xFF;
      geo = (data>>27)&0x1F;
      //      cout << "<HEAD> geo "<<dec<<geo<<" cra "<<cra<<" nch "<<nch<<endl; 
      hd++; 
      break;
    case 0x4: // EOB
      evn = data&0xFFFF;
      geo = (data>>27)&0x1F;      
      //      cout << "<EOB> geo "<<dec<<geo<<" evn "<<evn<<endl; 
      eob++;
      break;
    default:  // INVALID WORD
      //      cout << "<INVALID> data "<<hex<<data<<endl; 
      inv++;
      break;
    }
  }   // fine loop sui dati

  if (hd  != 1)                                                                                                  det |= 0x1; 
  if (eob != 1)                                                                                                  det |= 0x2; 
  //  if ((dat != 20)||(ich0!= 1)||(ich1!=1)||(ich2!=1)||(ich3!=1)
  //                 ||(ich16!= 1)||(ich17!=1)||(ich18!=1)||(ich19!=1)
  //                 ||(ich20!= 1)||(ich21!=1)||(ich22!=1)||(ich23!=1)
  //                 ||(ich24!= 1)||(ich25!=1)||(ich26!=1)||(ich27!=1)
  //                 ||(ich28!= 1)||(ich29!=1)||(ich30!=1)||(ich31!=1))                                              det |= 0x4; 
  if (inv != 0)                                                                                                  det |= 0x8; 
  
  if (sz != 6) {
    cout << "ADC1: size is " << sz << " at event " << nevt << endl;
  }
  return det;
}


/****************************************/
int checkScaler(unsigned int *buf, int size)
/****************************************/
{
  int rc = 0;           // This means no errors
  if (size != 16) {
    cout << "Error on SCALER at event " << nevt << endl;
    rc = -1;
  }
  return rc;
}

/****************************************/
void myTDCDisable()
/****************************************/
{
  tdc1.disableAllChannels();
  tdc2.disableAllChannels();
  tdc3.disableAllChannels();
}
  
/****************************************/
void myTDCEnable()
/****************************************/
{
  tdc1.enableAllChannels();
  tdc2.enableAllChannels();
  tdc3.enableAllChannels();
}


/****************************************/
void myInit()
/****************************************/
{
  int ww = 9;    // Time Window Width (0.5 us) for tdc1 and tdc2
  int wo = -6;    // Time Window Offset for tdc1 and tdc2
  int i;

  // const int RELOAD_TDC_CONFIG = myReloadTDCConfig();       // Reload TDC Configuration
  const int RELOAD_TDC_CONFIG = 1;                         // Always reload TDC Configuration

  if (RELOAD_TDC_CONFIG) {    // Reload TDC Configuration
  
    cout << "Reloading TDC configuration..." << endl;

    // TDC0 Init (V488)

    myTDC0Init();

    tdc1.disableAutoLoad();
    tdc2.disableAutoLoad();
    tdc3.disableAutoLoad();

    // TDC Init


    tdc1.setStopTriggerMatching();
    tdc1.enableStartTimeReadout();
    tdc1.setWindowWidth(ww);
    tdc1.setWindowOffset(wo);
    tdc1.setDataReadyEvent();
    //    tdc1.saveUserConfiguration();
    //usleep(100000);
    //tdc1.enableAutoLoad();
    //usleep(100000);

    tdc2.setStopTriggerMatching();
    tdc2.enableStartTimeReadout();
    tdc2.setWindowWidth(ww);
    tdc2.setWindowOffset(wo);
    tdc2.setDataReadyEvent();
    //tdc2.saveUserConfiguration();
    //usleep(100000);
    //tdc2.enableAutoLoad();
    //usleep(100000);

    //    cout << "Start Init TDC3 " << endl;


    tdc3.setStopTriggerMatching();
    tdc3.enableStartTimeReadout();
    tdc3.setWindowWidth(ww);
    tdc3.setWindowOffset(wo);
    tdc3.setDataReadyEvent();
    //tdc3.saveUserConfiguration();
    //usleep(100000);
    //tdc3.enableAutoLoad();
    //usleep(100000);
    
  }  // End of RELOAD_TDC_CONFIG

  // The following setting have to be performed always...

  //tdc1.setRisingEdgeOnly();
  tdc1.setBothEdges();
  //tdc2.setRisingEdgeOnly();
  tdc2.setBothEdges();
  //tdc3.setRisingEdgeOnly();
  tdc3.setBothEdges();

  myTDCEnable();

  // Disable noisy channels (as of 9/5/2003)
  //cout << "Disabling channels 39 on TDC1" << endl;
//    for(int k=20; k<62; k++) {
//      tdc1.disableChannel(k);
//    }
//    for(int k=20; k<62; k++) {
//      tdc2.disableChannel(k);
//    }
//    for(int k=20; k<62; k++) {
//      tdc3.disableChannel(k);
//    }
//    for(int k=64; k<128; k++) {
//      tdc3.disableChannel(k);
//    }

  tdc1.clear();
  tdc2.clear();
  tdc3.clear();

  // ADC Init
  adc1.setGeoAddress(8);        // Now in slot 8
  adc1.reset();                 // Needed to enable the new geo address
  adc1.clearData();

  for (i=0; i<32; i++) { adc1.setChannelThreshold(i, 0); }
  //adc1.disableOverflowSuppression();
  //adc1.disableZeroSuppression(); 
  adc1.enableChannel(0); 
  adc1.enableChannel(1);
  adc1.enableChannel(2); 
  adc1.enableChannel(3);
  //  for (i=1; i<32; i++) { adc1.enableChannel(i);  }
  for (i=4 ; i<32; i++) { adc1.disableChannel(i); }



  //cout << "adc1: slide status is " << adc1.getSlide() << endl;
  //adc1.disableSlide();
  cout << "adc1: slide status is " << adc1.getSlide() << endl;

  {                               // ACa 09042004: added to understand pedestal settings...
    unsigned short ped;
    ped = adc1.getI1();
    cout << "adc1: Default pedestal value is " << ped << endl;
    adc1.setI1(ped);              // Set I1 current to define pedestal position 
    ped = adc1.getI1();            // in the range [0x00-0xFF] (see manual par. 4.34.2)
    cout << "adc1: Now pedestal value is " << ped << endl;
  }                               

  //Start SlowControl Thread
  startSlowControlThread();
  sleep(2);


  // Scaler Init
  sca1.clear();
  sca1.inhibitReset();

  // CBD8210 Init + real Camac stuff if needed...
//    camac.resetAck();      // Resets both ACK2 and ACK4 outputs,
//                           // which also VETOs events from coming
}

/****************************************/
void myDisableAllTDCChannel()
/****************************************/
{
   for(int k=0; k<63; k++) {
     tdc1.disableChannel(k);
   }
   for(int k=0; k<63; k++) {
     tdc2.disableChannel(k);
   }
   for(int k=0; k<127; k++) {
     tdc3.disableChannel(k);
   }
}

/****************************************/
void myEnbleAllTDCChannel()
/****************************************/
{
   for(int k=0; k<63; k++) {
     tdc1.disableChannel(k);
   }
   for(int k=0; k<63; k++) {
     tdc2.disableChannel(k);
   }
   for(int k=0; k<127; k++) {
     tdc3.disableChannel(k);
   }
}

  
/****************************************/
void myReset()
/****************************************/
{
  // Reset all hardware

  adc1.reset();
  sca1.clear();

  tdc0.reset();  
  tdc1.reset();
  tdc2.reset();
  tdc3.reset();
  tdc3.resetwait();
}



/****************************************/
void myTriggerEnable()
/****************************************/
{
  dio1.set1(15);
}
  
/****************************************/
void myTriggerDisable()
/****************************************/
{
  dio1.set0(15);
}
  
/****************************************/
void myClearBusy()
/****************************************/
{
  dio1.singlePulse(14);
}
  
/****************************************/
void myScalerReset()
/****************************************/
{
  dio1.singlePulse(13);
}
  
/****************************************/
int myWaitEvent()
/****************************************/
{
  int rc = 0;

  // Wait Event driven by first TDC, but other modules are checked also...

  while (!tdc1.dready(1) && !abort_run)              {;}  // Wait Data Ready
  while (!tdc2.dready(1) && !abort_run)              {;}  // Wait Data Ready
  while (!tdc3.dready(1) && !abort_run)              {;}  // Wait Data Ready
  while (!adc1.dready(1) && !abort_run)              {;}  // Wait Data Ready

  if (abort_run) rc = -1;
  return rc;
}

/****************************************/
int myReadEvent()
/****************************************/
{
  int sz;
  int rc0, rc1, rc2, rc3, rc4;

  // Reset myIO local buffer

  myNewEvent();

  // All Modules should have something to be readout...

  sz = tdc0.readEvent(buf);
  rc0 = checkTDC0(buf, sz);
  myFormatSubEvent(buf, sz, tdc0.id());

  //  cout << "TDC Buffer Size: " << sz << "\t" << buf[1] << "\t" << buf[2] << "\t" << buf[3] << endl;
  if(rc0==1){  myTDC0Init(); usleep(10);}

  sz = tdc1.readEvent(buf); 
  rc1 = checkTDCRef(buf, sz);
  myFormatSubEvent(buf, sz, tdc1.id());

  sz = tdc2.readEvent(buf);
  rc2 = checkTDCRef(buf, sz);
  myFormatSubEvent(buf, sz, tdc2.id());

  sz = tdc3.readEvent(buf);
  rc3 = checkTDCRef(buf, sz);
  myFormatSubEvent(buf, sz, tdc3.id());

  sz = adc1.readEvent(buf); 
  rc4 = checkADC(buf, sz);
  myFormatSubEvent(buf, sz, adc1.id());

  //  if (sca1.inhibitGet()) {               // Only readout the scalers when the module is inhibited...
  //  sz = sca1.readEvent(buf);
  //  rc += checkScaler(buf, sz);
  //  myFormatSubEvent(buf, sz, sca1.id());
  //  sca1.clear();                        // ... then clear it...
  //  }

  if (rc0 != 0) cout << ">>> Error " << rc0 << " while reading TDC0..." << endl;  
  if (rc1 != 0) cout << ">>> Error " << rc1 << " while reading TDC1..." << endl;
  if (rc2 != 0) cout << ">>> Error " << rc2 << " while reading TDC2..." << endl;
  if (rc3 != 0) cout << ">>> Error " << rc3 << " while reading TDC3..." << endl;
  if (rc4 != 0) cout << ">>> Error " << rc4 << " while reading ADC1..." << endl;


  return rc0+rc1+rc2+rc3+rc4;
}

/****************************************/
void myEventToDisk(char *fname)
/****************************************/
{
  static int first = 1;

  if (first) {      // Open file only the first time
    cout << "myEventToDisk: opening file " << fname << endl;
    first = 0;
  }

  //  for (int i=0; i<evsiz; i++) 
  //cout << hex << buf[i] << endl;
  //  cout << dec << nevt << " " << evsiz << endl;
  

  return;
}

/****************************************/
int main()
/****************************************
    
 This is the main DAQ program. The programs performs
 the following steps:

 0) 'Active' declaration of all hardware, "a la C++".
 1) Clear of all hardware
 2) Initialization of all hardware, including:
    - coincidence VETO active;
    - VETO timing unit reset.
 3) Event loop: coinc. VETO low, wait for all modules to be ready, then readout; continue loop;
 4) End of Run: coinc. VETO active. End.

 ****************************************/
    
{
  int ntot;        // Total Events which are readout by DAQ
  int err;         // Error on data
  int errtdc = 0;  // Total number of TDC errors...

  const int MAXEVT   = myMaxEvents();                      // Maximum number of events
  const int DWNSCALE = myDownscaleFactor();                // Downscale factor

  signal(SIGINT, cntrl_c_handler);      // Control-C handler

  ios::sync_with_stdio();          // To syncronize with stdio library (really needed?)
 
  cout << endl;
  cout << "                    --------------------------------------" << endl;
  cout << "                    Welcome to the LHCb Muon GEM group DAQ" << endl;
  cout << "                    --------------------------------------" << endl;
  cout << endl;
  cout << endl;
  cout << endl;
  cout << "Disable all TDC Channels" << endl;
  myDisableAllTDCChannel();


  // I/O Register Initialization
  dio1.reset();
  usleep(500000);
  dio1.initStatusRegister();
  dio1.setChannelStatusRegister(13, 0x4);   // Output/Negative/normal/Trasparent
  dio1.setChannelStatusRegister(14, 0x4);   // Output/Negative/normal/Trasparent
  dio1.setChannelStatusRegister(15, 0x4);   // Output/Negative/normal/Trasparent
  myTriggerDisable();                       // Disable Output #15 --> no trigger allowed
  cout << "IOReg initialized / Trigger disabled" << endl;

  myReset(); 
  cout << "Hardware reset done" << endl;

  myInit(); 
  cout << "Hardware initialisation done" << endl;

  cout << endl << "Bitte Warten" << endl << endl;
  //  cout << tdc1.id() << endl;

  //  myTDCEnable();       // Enable All TDC Channels

  usleep(500000);
  myOpenRun();                     // Open data file

  cout << "Trigger enabled, starting acquisition of "<< MAXEVT << " events" << " (trigger downscale factor is " << DWNSCALE << ")" << endl;

  initProgressBar(MAXEVT, 500);  // Progress bar updates every 500ms

  nevt = 0;
  ntot = 0;

  do {
    if (abort_run) break;
    if (nevt==0) { myClearBusy(); myTriggerEnable(); myScalerReset();}


    if (myWaitEvent()==0) {  // Good event found...
      //      cout << readToverP() << endl;
      err = myReadEvent();
      if (err) errtdc++;   // Found a VME module error condition
      ntot++;
      if (!(ntot%DWNSCALE)) {
	if (err == 0) {
	  myWriteEvent(); 
	  nevt++;
	}
      }
    } 
    else {
      cout << "Got End-Of-Run in myWaitEvent() " << endl;
    }

    if (nevt!=(MAXEVT)) myClearBusy();        // Do not clear busy at the last event...
    updateProgressBar(nevt);
  } while (nevt<MAXEVT);

  resetProgressBar();  // done with the progress bar

  myTriggerDisable();
  myDisableAllTDCChannel();
  cout << endl << "Trigger disabled, end of data acquisition, " << dec << nevt << " events acquired (" << errtdc << " events with errors rejected)" << endl;

  myCloseRun();             // Close data file

  cout << "Data acquisition completed successfully" << endl << endl;

  return 0;
}
