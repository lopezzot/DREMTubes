/****************************************/
// Standard C header files
/****************************************/
#include <sys/times.h>
#include <unistd.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "myFIFOSampler.h"
#include "myFIFO-IOp.h"

using namespace std;

uint32_t events(0), evtnum(0), spillNr(0), nevtTotal(0);
myFIFOSampler* fifo(NULL);

string tprint()
 {
  time_t tt(time(NULL));
  char tbuf[32];
  ctime_r( &tt, tbuf);
  tbuf[24] = '\0';
  return string(tbuf);
 }

void loginfo()
 {
  uint32_t nevtaken = (fifo) ? fifo->getNevt() : nevtTotal;
  double percent = (nevtaken) ? events/(1.0*nevtaken) : 0.0;
  percent = rint(percent*10000)/100;
  if (nevtaken) cout << "\nSpill: "<< spillNr
       <<" - sampled "<< events << " events over " << nevtaken 
       << " (" << percent << "%) - time is " << tprint() << endl;
 }

class firstlastMessages
 {
  public:
   firstlastMessages() { cout << __FILE__ " compiled on " __DATE__ " " __TIME__ "\n**************************\n"; loginfo(); cout << "DREAM RUN STARTING" << endl; }
   ~firstlastMessages() { cout << "DREAM RUN CLOSED\n"; loginfo(); cout << "**************************\n" << endl; }
 };

firstlastMessages _firstlastMsgs;

extern "C" {
  int32_t dreammon_init( char** argv, uint32_t run_nr, bool drs, int32_t drs_setup, bool phys_h );
  int32_t dreammon_event( uint32_t doSingle, uint32_t events, uint32_t* buf, bool drs, int32_t drs_setup );
  int32_t dreammon_exit( int32_t i, bool drs, int32_t drs_setup );
  int32_t dreammon_sync( int32_t i, bool drs, int32_t drs_setup );
}
                                                                                                        
bool abort_run=false;

void cntrl_c_handler(int32_t /* signum */)
{
  cerr << "\nGot control-c, end of current run\n" << endl;
  abort_run = true;
}

int32_t main(int32_t argc, char** argv){

  uint32_t run_nr;
  uint32_t size;
  uint32_t buf[1000000];
  int32_t evt_interval=1000;
  uint32_t oldSpillNr = 0;
  uint32_t doSingle;
  struct tms tmsbuf;
  uint32_t ticks_per_sec;
  double time0;

  bool drs;
  bool drs_fb_lk;
  bool drs_matrix;

  drs=false;
  drs_fb_lk=false;
  drs_matrix=false;

  bool phys_h;
  int32_t drs_setup=0;

  phys_h=true;
  if (argc<2) {
   drs=false;
   drs_fb_lk=false;
   drs_matrix=false;
   drs_setup=0;
  } else {
    if(!strcmp(argv[0],"drs_fb_lk")){
      --argc; ++argv;
      drs=true;
      drs_fb_lk=true;
      drs_matrix=false;
      drs_setup=1;
    }
    if(!strcmp(argv[0],"nodrs")){
      --argc; ++argv;
      drs=false;
      drs_fb_lk=false;
      drs_matrix=false;
      drs_setup=0;
    }
    if(!strcmp(argv[0],"drs_matrix")){
      --argc; ++argv;
      drs=true;
      drs_fb_lk=false;
      drs_matrix=true;
      drs_setup=2;
    }

  }

  if ((drs==true)&&(drs_fb_lk==true)&&(drs_matrix==false)) drs_setup=1;
  if ((drs==true)&&(drs_fb_lk==false)&&(drs_matrix==true)) drs_setup=2;
  if (drs==false) drs_setup=0;

  int32_t xrunnr(0);
  while (--argc > 0)
   {
    ++argv;
    if ((strcmp(*argv, "-runnr") == 0) && (argc > 1))
     {
      --argc; ++argv;
      xrunnr = strtol(*argv, NULL, 10);
     }
   }

  run_nr = xrunnr;
  if (run_nr <= 0)
   {
    cerr << "****************** ERROR ****************** run number not found "
         << run_nr << " exiting now\n" << endl;
    exit(1);
   }
  cout << "Run number is:    " << run_nr << endl;

  time0 = times(&tmsbuf);
  ticks_per_sec = sysconf(_SC_CLK_TCK);
  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  
  fifo = new myFIFOSampler(PHYS_BASE_KEY);

  if(fifo->isvalid()){
    cout << "Fifo is valid" << endl;
    fifo->waitReader();
  }else{
    cout << "Fifo is not valid" << endl;
    abort_run=true;
  }

  dreammon_init(argv, run_nr, drs, drs_setup, phys_h);  

  int32_t lastSyncSpill=-10, lastSyncEvt=-900; //force the sample to write the histogram file after the first 100 events.

  while(!abort_run)
  {
    fifo->waitlock();
    myFIFO::result result =fifo->read(buf,&size);
    spillNr=fifo->getSpillNr();
    // uint32_t nevtaken = fifo->getNevt();
    fifo->unlock();
    
    if (/* (nevtaken) && */ (result==myFIFO::FIFOEMPTY)) {
      if(!fifo->isWriterPresent()) 
        break;
      usleep(1000);
    }
    if(result==myFIFO::FIFOEMPTY){
     if (oldSpillNr+2 <= spillNr) 
	{
	  oldSpillNr = spillNr;
	  loginfo();
	  if(spillNr-lastSyncSpill>=2 && events-lastSyncEvt>100){
	    lastSyncSpill=spillNr;
	    lastSyncEvt=events;
	    dreammon_sync(1,drs,drs_setup);
	  }
 	}
    }else if(result==myFIFO::WRLOCKED || result==myFIFO::RDLOCKED){
      usleep(1000);
    }else if(result==myFIFO::FAILED){
      cout << "Alignment error" << endl;
      exit(1);
    }else if(result==myFIFO::SUCCESS){
      evtnum = ((EventHeader *)buf)->evnum+1;
      doSingle = ((evtnum % evt_interval) == 1);
      dreammon_event(doSingle,events++,buf,drs,drs_setup);  
    }
    
  }
  loginfo();

  dreammon_exit(0,drs,drs_setup);  

  nevtTotal = fifo->getNevt();
  delete fifo;
  fifo = NULL;

  double tottime = times(&tmsbuf) - time0;
  tottime /= ticks_per_sec;

  double cputime = tmsbuf.tms_utime + tmsbuf.tms_stime;
  cputime /= ticks_per_sec;

  double percent = cputime/tottime;
  percent = rint(percent*10000)/100;

  cout << "TIME - TOT: " << tottime << " sec - CPU: " << cputime << " sec (" << percent << "%)" << endl;

  return 0;
}
