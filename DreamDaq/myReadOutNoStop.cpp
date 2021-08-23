
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <csignal>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "myModules.h"                         // VME Module Id.s
#include "myV2718.h"                           // CAEN V2718 VME Bridge class
#include "myV513.h"                            // CAEN V513 I/O REG class
#include "myV792AC.h"                          // CAEN V792AC QDC class
#include "myV775N.h"                           // CAEN V775N TDC class
#include "myV775.h"                            // CAEN V775 TDC class


using namespace std;

#define TRIGGER_IN_CHAN 7                    // channel 7
#define TRIGGER_MASK 7                       // channels 0, 1, 2
#define TRIGGER_DISA_ALL 15                  // channel 15
#define TRIGGER_UNLK_CHAN 14                 // channel 14
#define SCALER_RESET 13                      // channel 13
#define PEDESTAL_VETO 12                     // channel 12

#define NSLEEPT 1000                         // for sleeping 1000 ns

#define V775_35ps 35                         // 35 ps as scale for V775
#define V775_100ps 100                       // 100 ps as scale for V775
#define V775_140ps 140                       // 140 ps as scale for V775

const uint32_t nQDC = 3;
const uint32_t QDCchans = 32;
const uint32_t QDCevsize = QDCchans+2;
const uint32_t QDCtotsize = QDCevsize*nQDC;

const uint32_t V775Nchans = 16;

void dlwait( char* msg )
 {
  cout << "Press any key to continue ... " << msg;
  getchar();
  cout << endl;
 }

const uint32_t ONEK = 1000;
const uint32_t ONEM = ONEK*ONEK;
const uint32_t ONEG = ONEK*ONEM;

void nsleep ( uint32_t ns )
 {
  uint32_t secs = ns / ONEG;
  uint32_t nsrem = ns % ONEG;
  struct timespec ndelay = { secs, nsrem };
  nanosleep ( &ndelay, NULL );
 }

void myusleep (uint32_t us )
 {
  uint32_t secs = us / ONEM;
  uint32_t usrem = us % ONEM;
  struct timespec ndelay = { secs, usrem*ONEK };
  nanosleep ( &ndelay, NULL );
 }

inline void enableTriggers( v513& ioreg ) { ioreg.clearOutputBit( TRIGGER_DISA_ALL ); }
inline void disableTriggers( v513& ioreg ) { ioreg.setOutputBit( TRIGGER_DISA_ALL ); }

inline void enablePedestal( v513& ioreg ) { ioreg.clearOutputBit( PEDESTAL_VETO ); }
inline void disablePedestal( v513& ioreg ) { ioreg.setOutputBit( PEDESTAL_VETO ); }

inline uint32_t ioregValue( v513& ioreg ) { return ioreg.readInputRegister(); }

inline bool isTrig( v513& ioreg ) { return (ioreg.readInputRegister() & 1<<TRIGGER_IN_CHAN); }
inline uint32_t getTrigMask( v513& ioreg ) { return (ioreg.readInputRegister() & TRIGGER_MASK); }

inline void resetScaler( v513& ioreg )
 {
  nsleep(NSLEEPT);
  ioreg.setOutputBit( SCALER_RESET );
  nsleep(NSLEEPT);
  ioreg.clearOutputBit( SCALER_RESET );
 }

inline void unlockTrigger( v513& ioreg )
 {
  nsleep(NSLEEPT);
  ioreg.setOutputBit( TRIGGER_UNLK_CHAN );
  nsleep(NSLEEPT);
  ioreg.clearOutputBit( TRIGGER_UNLK_CHAN );
 }

int initV513( v513& ioreg )
 {
  uint16_t w;
  ioreg.read16phys(0xFE, &w);
  cout << hex << " V513 FE " << w << endl;
  nsleep(NSLEEPT);
  ioreg.read16phys(0xFC, &w);
  cout << hex << " V513 FC " << w << endl;
  nsleep(NSLEEPT);
  ioreg.read16phys(0xFA, &w);
  cout << hex << " V513 FA " << w << endl;
  nsleep(NSLEEPT);
  w=0;
  ioreg.write16phys(0x48, w);
  nsleep(NSLEEPT);
  ioreg.write16phys(0x46, w);
  nsleep(NSLEEPT);
  ioreg.write16phys(0x42, w);
  nsleep(NSLEEPT);

  ioreg.reset();
  nsleep(NSLEEPT);

  ioreg.read16phys(0x04, &w);
  cout << hex << " V513 0x4 " << w << endl;
  nsleep(NSLEEPT);

  for (int i=0; i<8; i++)
   {
    int reg = 0x10+i*2;
    ioreg.setChannelInput(i);
    nsleep(NSLEEPT);
    ioreg.read16phys(reg, &w);
    cout << hex << " V513 input ch " << i << " reg " << reg << " " << w << endl;
    nsleep(NSLEEPT);
   }

  for (int i=8; i<16; i++)
   {
    int reg = 0x10+i*2;
    ioreg.setChannelOutput(i);
    nsleep(NSLEEPT);
    ioreg.read16phys(reg, &w);
    cout << hex << " V513 output ch " << i << " reg " << reg << " " << w << endl;
    nsleep(NSLEEPT);
   }

  disableTriggers( ioreg );

  return 0;
 }

void initV792( v792ac& v792adc )
 {
  v792adc.print();
  v792adc.reset();
  v792adc.disableSlide();
  v792adc.disableOverflowSupp();
  v792adc.disableZeroSupp();
  v792adc.clearEventCounter();
  v792adc.clearData();
  uint16_t ped;
  ped = v792adc.getI1();
  uint32_t bid = v792adc.id();
  cout << "v792ac addr 0x" << hex << bid << dec
       << ": default ped I1 value is " << ped << endl;

  ped = 255;                  // >~ minimum possible Iped (see manual par. 2.1.2.1)
  v792adc.setI1(ped);       // set I1 current to define pedestal position
  ped =v792adc.getI1();     // in the range [0x00-0xFF] (see manual par. 4.34.2)
  uint16_t thr = 0x00;
  v792adc.setChannelThreshold(thr);
  cout << "v792ac addr 0x" << hex << bid << dec << ": now ped  I1 value is " << ped << " thr " << thr << endl;
  
  v792adc.enableChannels();

  uint16_t reg1 = v792adc.getStatus1();
  uint16_t reg2 = v792adc.getControl1();
  uint16_t reg3 = v792adc.getStatus2();
  uint16_t reg4 = v792adc.getBit1();
  uint16_t reg5 = v792adc.getBit2();
  uint32_t evc = v792adc.eventCounter();
  cout << "v792ac addr 0x" << hex << bid << " status 1 0x" << reg1
       << " control 1 0x" << reg2 << " status 2 0x" << reg3 << " bit 1 0x" << reg4
       << " bit 2 0x" << reg5 << dec << endl;
  cout << "v792ac event counter " << evc << endl;
 }

void initV775N( v775n& xtdc )
 {
  xtdc.swReset();
  xtdc.setCommon ( v775::commonStart );
  xtdc.zeroSuppression(false);
  xtdc.overSuppression(false);
  xtdc.setLSB(V775_140ps);   // set the LSB (ps)  full scale = 4096*LSB ==> 100 ps * 4096 ~ 410 ns
  xtdc.storeEmpty();
  xtdc.evCntReset();
  xtdc.dataReset();
  xtdc.printRegisters();
  cout << "v775 mode " << xtdc.getMode() << endl;
  cout << "v775 fsr (ns) " << 4096*8.9/xtdc.fullScale() << endl;
  cout << "v775 LSB (ns) " << 8.9/xtdc.fullScale() << endl;
 }

void initV775( v775& xtdc )
 {
  xtdc.swReset();
  xtdc.setCommon ( v775::commonStart );
  xtdc.zeroSuppression(false);
  xtdc.overSuppression(false);
  xtdc.setLSB(V775_35ps);   // set the LSB (ps)  full scale = 4096*LSB ==> 100 ps * 4096 ~ 410 ns
  xtdc.storeEmpty();
  xtdc.evCntReset();
  xtdc.dataReset();
  xtdc.printRegisters();
  cout << "v775 mode " << xtdc.getMode() << endl;
  cout << "v775 fsr (ns) " << 4096*8.9/xtdc.fullScale() << endl;
  cout << "v775 LSB (ns) " << 8.9/xtdc.fullScale() << endl;
 }

volatile bool abort_run(false);
volatile bool pause_run(false);

void cntrl_c_handler ( int32_t sig )
 {
  time_t timestr = time(NULL);
  char * stime = ctime(&timestr);
  stime[24] = 0;
  fprintf(stderr,"%s cntrl_c_handler: sig%d\n\n", stime, sig);
  fprintf(stderr,"aborting run\n");
  abort_run = true;
 }

void sigusr1_handler ( int32_t sig )
 {
  time_t timestr = time(NULL);
  char * stime = ctime(&timestr);
  stime[24] = 0;
  fprintf(stderr,"%s sigusr1_handler: sig%d  pause_run is %d\n\n", stime, sig, pause_run);
  pause_run = not pause_run;
  pause_run ?  fprintf(stderr,"pausing run\n") : fprintf(stderr,"resuming run\n");
 }

uint32_t resetHardware ( v792ac& qdc1, v792ac& qdc2, v792ac& qdc3, v775n& xtdc1, v775& xtdc2 )
 {
  initV792 ( qdc1 );
  sched_yield();

  initV792 ( qdc2 );
  sched_yield();

  initV792 ( qdc3 );
  sched_yield();

  initV775N ( xtdc1 );
  sched_yield();

  initV775 ( xtdc2 );
  sched_yield();

  return 0;
 }

uint32_t alignEventCounter ( uint32_t evtnumber, v792ac& qdc1, v792ac& qdc2, v792ac& qdc3, v775n& xtdc1, v775& xtdc2 )
 {
  qdc1.eventCounterOffset ( evtnumber );
  qdc2.eventCounterOffset ( evtnumber );
  qdc3.eventCounterOffset ( evtnumber );
  xtdc1.eventCounterOffset ( evtnumber );
  xtdc2.eventCounterOffset ( evtnumber );

  return 0;
 }

uint32_t readEvent ( v792ac& qdc1, v792ac& qdc2, v792ac& qdc3, v775n& xtdc1, v775& xtdc2, uint32_t ntriggers, uint32_t* buffer, uint32_t* qdcsize, uint32_t* tdcsize1, uint32_t* tdcsize2 )
 {
  nsleep(NSLEEPT);
  qdc1.setEvents( ntriggers );
  uint32_t size1 = qdc1.readEvent( buffer );
  buffer += size1;
  qdc1.setEvents( ntriggers );
  uint32_t size2 = qdc2.readEvent( buffer );
  buffer += size2;
  qdc1.setEvents( ntriggers );
  uint32_t size3 = qdc3.readEvent( buffer );
  buffer += size3;
  uint32_t sizetdc1 = xtdc1.readSingleEvent( buffer );
  buffer += sizetdc1;
  uint32_t sizetdc2 = xtdc2.readSingleEvent( buffer );
  *qdcsize = size1+size2+size3;
  *tdcsize1 = sizetdc1;
  *tdcsize2 = sizetdc2;
  uint32_t nevqdc1, nevqdc2, nevqdc3, nevtdc1, nevtdc2;
  nevqdc1 = qdc1.eventCounter();
  nevqdc2 = qdc2.eventCounter();
  nevqdc3 = qdc3.eventCounter();
  nevtdc1 = xtdc1.eventCounter();
  nevtdc2 = xtdc2.eventCounter();
  uint32_t errcode = (ntriggers != nevqdc1) | (ntriggers != nevqdc2) << 1
	| (ntriggers != nevqdc3) << 2 | (ntriggers != nevtdc1) << 3 | (ntriggers != nevtdc2) << 4;
  if ((errcode == 0x1f) && (nevqdc1 == ntriggers+1)) errcode = 0xff;
  if (errcode)
   {
    time_t rtime = time(NULL);
    struct tm myrtime;
    localtime_r( &rtime, &myrtime );
    cerr << " time is " << dec << rtime << " " << myrtime.tm_year+1900 << "." << myrtime.tm_mon+1 << "." << myrtime.tm_mday << "-" << myrtime.tm_hour << ":" << myrtime.tm_min << ":" << myrtime.tm_sec << endl;
    cerr << dec << ntriggers << " nq1 " << nevqdc1 << " nq2 " << nevqdc2 << " nq3 " << nevqdc3 << " nt1 " << nevtdc1 << " nt2 " << nevtdc2 << " - error code " << hex << errcode << endl;
   }
  if (size1*size2*size3 == 0)
   {
    errcode |= 1 << 7;
    time_t rtime = time(NULL);
    struct tm myrtime;
    localtime_r( &rtime, &myrtime );
    cerr << " time is " << dec << rtime << " " << myrtime.tm_year+1900 << "." << myrtime.tm_mon+1 << "." << myrtime.tm_mday << "-" << myrtime.tm_hour << ":" << myrtime.tm_min << ":" << myrtime.tm_sec;
    cerr << dec << ntriggers << " size1 " << size1 << " size2 " << size2 << " size3 " << size3 << " sizetdc1 " << sizetdc1 << " sizetdc2 " << sizetdc2 << " - error code " << hex << errcode << endl;
   }
  return errcode;
 }

void print_event ( ofstream& ofs, uint32_t trignum, uint32_t* Tcts, uint32_t tmask, uint32_t* myBuffer, uint32_t qdcsize, uint32_t tdcsize1, uint32_t tdcsize2 )
 {
  ofs << dec << " ev # " << trignum << " tow cts " << Tcts[0] << " " << Tcts[1] << " " << Tcts[2] << " trigger mask " << hex << tmask << " values:";
  for (uint32_t kqdc=0; kqdc<nQDC; kqdc++)
   {
    uint32_t offs = kqdc*QDCevsize;
    uint32_t ch_offs = kqdc*QDCchans;
    for (uint32_t j=offs+1; j<offs+QDCevsize-1; j++)
     {
      uint32_t chan = (myBuffer[j] >> 16) & 0x1f;
      chan += ch_offs;
      ofs << " " << dec << chan << " " << hex << (myBuffer[j] & 0x3fff);
     }
   }
  uint32_t tdcsize = tdcsize1 + tdcsize2;
  ofs << " TDC size " << dec << tdcsize << " val.s ";
  uint32_t* tdcbuffer = &myBuffer[qdcsize];
  if (tdcsize1 != 0xffffffff) for (uint32_t j=0; j<tdcsize1; j++)
   {
    uint32_t _v = tdcbuffer[j];
    uint32_t chan = dataDecodeNChannel(_v);
    uint32_t mk = dataDecodeNFlags(_v);
    uint32_t val = dataDecodeNValue(_v);
    ofs << " " << chan << " " << mk << " " << val;
   }
  tdcbuffer = &myBuffer[qdcsize+tdcsize1];
  if (tdcsize2 != 0xffffffff) for (uint32_t j=0; j<tdcsize2; j++)
   {
    uint32_t _v = tdcbuffer[j];
    uint32_t chan = dataDecodeChannel(_v);
    chan += V775Nchans;
    uint32_t mk = dataDecodeFlags(_v);
    uint32_t val = dataDecodeValue(_v);
    ofs << " " << chan << " " << mk << " " << val;
   }
  ofs << endl;
  return;
 }

void print_fake_event ( ofstream& ofs, uint32_t trignum, uint32_t* Tcts, uint32_t* myBuffer )
 {
  ofs << dec << " ev # " << trignum << " tow cts " << Tcts[0] << " " << Tcts[1] << " " << Tcts[2] << " trigger mask " << hex << 0xffffffff << " values:";
  for (uint32_t kqdc=0; kqdc<nQDC; kqdc++)
   {
    uint32_t offs = kqdc*QDCevsize;
    uint32_t ch_offs = kqdc*QDCchans;
    for (uint32_t j=offs+1; j<offs+QDCevsize-1; j++)
     {
      uint32_t chan = (myBuffer[j] >> 16) & 0x1f;
      chan += ch_offs;
      ofs << " " << dec << chan << " " << 0;
     }
   }
  ofs << " TDC size " << dec << 0 << " val.s ";
  ofs << endl;
  return;
 }

int main( int argc, char** argv )
 {
  ofstream ofs;
  ifstream ifs;

  uint32_t trignum(0);
  uint32_t numevofs(0);
  uint32_t Tcts[4] = { 0, 0, 0, 0 };
  uint32_t myBuffer[4096];

  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  signal(SIGUSR1, sigusr1_handler);      // Control-C handler

  v2718 v2718_1(0,"/V2718/cvA24_U_DATA/0");            // VME interface
  v513 ion(0xa00000,"/V2718/cvA24_U_DATA/0");          // I/O register

  disableTriggers( ion );

  v792ac qdc1(0x05000000,"/V2718/cvA32_U_DATA/0");      // V792 QDC
  v792ac qdc2(0x06000000,"/V2718/cvA32_U_DATA/0");      // V792 QDC
  v792ac qdc3(0x07000000,"/V2718/cvA32_U_DATA/0");      // V792 QDC
  v775n xtdc1(0x08000000,"/V2718/cvA32_U_DATA/0");      // V775N TDC
  v775 xtdc2(0x09000000,"/V2718/cvA32_U_DATA/0");       // V775 TDC

  // Init I/O Register
  initV513 ( ion );
  sched_yield();

  // Init QDCs and TDCs
  resetHardware ( qdc1, qdc2, qdc3, xtdc1, xtdc2 );

  resetScaler( ion );
  unlockTrigger( ion );
  // dlwait(" ");
  enableTriggers( ion );

  cout << " ****************** start of run ****************** " << endl << endl;
  time_t stime = time(NULL);
  cout << " time is " << stime << endl;
  struct tm mytime;
  localtime_r( &stime, &mytime );

  cout << mytime.tm_year+1900 << "." << mytime.tm_mon+1 << "." << mytime.tm_mday << "." << mytime.tm_hour << ":" << mytime.tm_min << ":" << mytime.tm_sec << endl;
  cout << asctime(&mytime) << endl;

  uint32_t runnbr;
  ifs.open ("/home/dreamtest/working/run.number",std::ofstream::in);
  ifs >> runnbr;
  ifs.close();
  ofs.open ("/home/dreamtest/working/run.number",std::ofstream::out);
  ofs << ++runnbr;
  ofs.close();
  

  stime -= 1600000000;
  string fname = "sps2021data.run" + to_string(runnbr) + ".txt";

  if (argc == 2) fname = argv[1];

  cout << " filename is " << fname << endl;

  ofs.open (fname.c_str(), std::ofstream::out);

  time_t tr0 = time(NULL);
  bool running(true);

  volatile bool exit_now(false);
  volatile bool on_error(false);
  while (1)
   {
    if (abort_run) exit_now = true;
    if (exit_now) disableTriggers( ion );
    if (running and pause_run)
     {
      disableTriggers(ion);
      running = false;
  time_t tr = time(NULL) - tr0;
  cout << dec << tr << "(sec) " << trignum << "(evt) ioreg " << ioregValue( ion ) << " running was true (now false) and pause_run true" << endl;
     }
    else if (not running and not pause_run)
     {
      enableTriggers(ion);
      running = true;
  time_t tr = time(NULL) - tr0;
  cout << dec << tr << "(sec) " << trignum << "(evt) ioreg " << ioregValue( ion ) << " running was false (now true) and pause_run false" << endl;
     }
    bool isPh = isTrig( ion );
    if (isPh)
     {
      uint32_t tmask = getTrigMask( ion );
if ((trignum % 10000) == 0)
 {
  time_t tr = time(NULL) - tr0;
  double rate = trignum/double(tr);
  cout << dec << tr << "(sec) " << trignum << "(evt) "
      << "beam/pede/inSpill/beamInSpill: "
      << Tcts[0] << "/" << Tcts[1] << "/" << Tcts[2] << "/" << Tcts[3] << " "
      << rate << "(evt/sec) last isPh " << isPh << " tmask " << hex << tmask
      << " ioreg " << ioregValue( ion ) << endl;
 }
      if (tmask & 1) Tcts[0] ++;            // physics triggers
      if (tmask & 2) Tcts[1] ++;            // pedestal triggers
      if (tmask & 4) Tcts[2] ++;            // in spill
      if ((tmask & 5) == 5) Tcts[3] ++;    // physics in spill
      (Tcts[1]*10 < Tcts[0]) ? enablePedestal( ion ) : disablePedestal( ion );
      uint32_t qdcsize, tdcsize1, tdcsize2;
      uint32_t retcode = readEvent ( qdc1, qdc2, qdc3, xtdc1, xtdc2, trignum, myBuffer, &qdcsize, &tdcsize1, &tdcsize2 );
      uint32_t head = myBuffer[0];
      uint32_t trail = myBuffer[qdcsize-1] & 0xff000000;
      uint32_t evnum = numevofs + (myBuffer[qdcsize-1] & 0xffffff);
      if (head != 0xfa002000)
       {
        cerr << dec << trignum << " Error in header " << hex << head << endl;
        tmask |= 0xffff0000;
        on_error = true;
        disableTriggers( ion );
       }
      if (trail != 0xfc000000)
       {
        cerr << dec << trignum << " Error in trailer " << hex << trail << endl;
        tmask |= 0xffff0000;
        on_error = true;
        disableTriggers( ion );
       }
      if (qdcsize != QDCtotsize)
       {
	cerr << dec << trignum << " Size error " << qdcsize << endl;
        tmask |= 0xffff0000;
        on_error = true;
        disableTriggers( ion );
       }
      if (evnum != trignum)
       {
        cerr << dec << trignum << " Error in V792 event number " << evnum << endl;
        tmask |= 0xffff0000;
        on_error = true;
        disableTriggers( ion );
       }
      if (retcode)
       {
        cerr << dec << trignum << " Error return code " << retcode << endl;
        tmask |= 0xffff0000;
        on_error = true;
        disableTriggers( ion );
       }
      print_event ( ofs, trignum, Tcts, tmask, myBuffer, qdcsize, tdcsize1, tdcsize2 );
      trignum ++;
      // while (trignum <= evnum) trignum ++; // evt_realign ( ion, qdc, Tcts, ofs, trignum );

      if (retcode == 0xff)     // spurious trigger to all elx -- add one fake event to align VME data with FERS data
       {
        print_fake_event ( ofs, trignum, Tcts, myBuffer );
        trignum ++;
       }

      if (on_error)
       {
        // Re-init QDCs and TDCs
        cerr << dec << trignum << " ********* resetting DAQ ********* " << endl;
        time_t rtime = time(NULL);

        cerr << " time is " << dec << rtime << endl;
        struct tm myrtime;
        localtime_r( &rtime, &myrtime );

        cerr << myrtime.tm_year+1900 << "." << myrtime.tm_mon+1 << "." << myrtime.tm_mday << "." << myrtime.tm_hour << ":" << myrtime.tm_min << ":" << myrtime.tm_sec << endl;
        cerr << asctime(&myrtime) << endl;

        tmask |= 0xffff0000;
        resetHardware ( qdc1, qdc2, qdc3, xtdc1, xtdc2 );
        myusleep ( 1000000 );
        alignEventCounter ( trignum, qdc1, qdc2, qdc3, xtdc1, xtdc2 );
        numevofs = trignum;
        myusleep ( 1000000 );
        cerr << dec << trignum << " ********* reenabling triggers ********* " << endl;
        enableTriggers( ion );
        myusleep ( 1000000 );
        on_error = false;
       }

      if (!exit_now) unlockTrigger( ion );
     }
    if (exit_now) break;
    nsleep( NSLEEPT );
   }
  disableTriggers( ion );
  ofs.close();

  time_t etime = time(NULL);

  cout << " time is " << dec << etime << endl;
  struct tm myetime;
  localtime_r( &etime, &myetime );

  cout << myetime.tm_year+1900 << "." << myetime.tm_mon+1 << "." << myetime.tm_mday << "." << myetime.tm_hour << ":" << myetime.tm_min << ":" << myetime.tm_sec << endl;
  cout << asctime(&myetime) << endl;

  etime -= 1600000000;
  uint32_t tdiff = etime-stime;
  double rate = trignum/double(tdiff);
  cout << dec << " time is " << etime << " diff(sec) " << (etime-stime) << " events " << trignum
	<< " rate(Hz) " << rate << endl;

  cout << " ****************** end of run ****************** " << endl << endl;

  return 0;
 }
