//$Id: newDaq.cpp,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
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
//#include "myV767A.h"     // CAEN TDC V767A class derived from VME
#include "myV488.h"      // CAEN TDC V488 class derived from VME
#include "myCorbo.h"      // Creative Electronic Systems RCB 8047 CORBO class derived from VME
//#include "myV260.h"      // CAEN scaler V260 class derived from VME
//#include "myV513.h"      // CAEN Input Output Register V513 class derived from VME
//#include "myV792AC.h"    // CAEN ADC V792AC class derived from VME


/****************************************/
// Declare all the hardware
/****************************************/
v488    tdc0(0x030000,"/dev/vmedrv24d16");
v488    tdc1(0x030000,"/dev/vmedrvb24d16");
corbo   corbo0(0xfff000,"/dev/vmedrv24d16");

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
  int wr = 0xE0;   // Time Range for tdc0, 0x0 = 90ns, 0xE0 = 770 ns

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
  tdc0.setHighThr(0xC7);
  tdc0.setLowThr(0x0);  
}
 

/****************************************/
void myTDC1Init()
/****************************************/
{
  int wr = 0xE0;   // Time Range for tdc0, 0x0 = 90ns, 0xE0 = 770 ns

  tdc1.reset();
  tdc1.setCommonStop();
  usleep(100);
  tdc1.setHalfFullModeOB();
  usleep(100);
  tdc1.enableChannel(0);
  tdc1.enableChannel(1);
  tdc1.enableChannel(2);
  tdc1.enableChannel(3);
  for(int i=4;i<8;i++) {
    tdc1.disableChannel(i);
  }
  usleep(100);
  tdc1.setRange(wr);
  usleep(100);
  tdc1.setHighThr(0xC7);
  tdc1.setLowThr(0x0);  
}
 



/****************************************/
int checkTDCRef(unsigned int *buf, int size)
/****************************************/
{
  unsigned int data, type;
  int ch, tm, st, ed, ge, er, ev, sz;
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
      break;
    case 6:
      break;
    case 31:
      break;
    }
  }

  if (er) {
    cout << "Bad Status (0x" << er << ") in data from TDC at geo " << ge << " at event " << nevt << endl;
    unsigned int ec;
    rc = -1*ge;
    switch(ge) {
    case 4:
      break;
    case 6:
      break;
    case 31:
      break;
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
int checkTDC1(unsigned int *buf, int sz)
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
      //      cout << "<DATA> TDC1 on Ch: " << ch << " | tdc: " << tdc << endl;
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
    cout << "TDC1: size is " << sz << " at event " << nevt << endl;
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
}
  
/****************************************/
void myTDCEnable()
/****************************************/
{
}


/****************************************/
void myInit()
/****************************************/
{
    cout << "Reloading TDC configuration..." << endl;

    // TDC0 Init (V488)

    myTDC0Init();
}

/****************************************/
void myDisableAllTDCChannel()
/****************************************/
{
}

/****************************************/
void myEnbleAllTDCChannel()
/****************************************/
{
}

  
/****************************************/
void myReset()
/****************************************/
{
  // Reset all hardware

  tdc0.reset();  
}

/****************************************/
void myTriggerEnable()
/****************************************/
{
  // dio1.set1(15);
}
  
/****************************************/
void myTriggerDisable()
/****************************************/
{
  // dio1.set0(15);
}
  
/****************************************/
void myClearBusy()
/****************************************/
{
  // dio1.singlePulse(14);
}
  
/****************************************/
void myScalerReset()
/****************************************/
{
  // dio1.singlePulse(13);
}
  
/****************************************/
int myWaitEvent()
/****************************************/
{
  int rc = 0;

  // Wait Event driven by first TDC, but other modules are checked also...

  while (!tdc0.dready() && !abort_run)              {usleep(1);}  // Wait Data Ready

  if (abort_run) rc = -1;
  return rc;
}

static int TDCnr = 0;
/****************************************/
int myReadEvent()
/****************************************/
{
  int sz;
  int rc0, rc1, rc2, rc3, rc4;

  // Reset myIO local buffer

  myNewEvent();

  // All Modules should have something to be readout...

  if (TDCnr == 0) {

  cout << "TDC 0 : ";
  sz = tdc0.readEvent(buf);
  rc0 = checkTDC0(buf, sz);
  myFormatSubEvent(buf, sz, tdc0.id());

  //  cout << "TDC Buffer Size: " << sz << "\t" << buf[1] << "\t" << buf[2] << "\t" << buf[3] << endl;
  if(rc0==1){  myTDC0Init(); usleep(10);}

  if (rc0 != 0) cout << ">>> Error " << rc0 << " while reading TDC0..." << endl;  
  } else {

  cout << "TDC 1 : ";
  sz = tdc1.readEvent(buf);
  rc0 = checkTDC1(buf, sz);
  myFormatSubEvent(buf, sz, tdc1.id());

  if(rc0==1){  myTDC1Init(); usleep(10);}

  if (rc0 != 0) cout << ">>> Error " << rc0 << " while reading TDC1..." << endl;  
  }

  TDCnr ^= 1;

  return rc0;
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
  // dio1.reset();
  usleep(500000);
  // dio1.initStatusRegister();
  // dio1.setChannelStatusRegister(13, 0x4);   // Output/Negative/normal/Trasparent
  // dio1.setChannelStatusRegister(14, 0x4);   // Output/Negative/normal/Trasparent
  // dio1.setChannelStatusRegister(15, 0x4);   // Output/Negative/normal/Trasparent
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

  cout << "Progress bar init...done!" << endl;
  
  nevt = 0;
  ntot = 0;

  do {
    if (abort_run) break;
    if (nevt==0) { myClearBusy(); myTriggerEnable(); myScalerReset();}


    if (myWaitEvent()==0) {  // Good event found...
      cout <<"Event found...reading it"<< endl;
      err = myReadEvent();
      cout <<"Event read"<< endl;
      if (err){ 
	errtdc++;   // Found a VME module error condition
	cout <<"Error in reading operation"<< endl;
      }
      ntot++;

      if (!(ntot%DWNSCALE)) {
	if (err == 0) {
	  cout <<"No errors in reading...start to write the event"<< endl;
	  myWriteEvent(); 
	  cout <<"Event written!"<< endl;
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
