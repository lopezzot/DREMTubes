/****************************************/
// Standard C header files
/****************************************/
#include <sys/times.h>
#include <unistd.h>
#include <stdint.h>

#include <ctime>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>

#define IMAGIC 0xAABBCCDD

#include "myFIFOReader.h"
#include "myFIFO-IOp.h"

using namespace std;

uint32_t nevtsaved(0);

string tprint()
 {
  time_t tt(time(NULL));
  char tbuf[26];
  ctime_r( &tt, tbuf);
  tbuf[24] = '\0';
  return string(tbuf);
 }

void loginfo( uint32_t events=0)
 {
  cout << "\nEvents in file : " << events
       << " - time is " << tprint() << endl;
 }

class firstlastMessages
 {
  public:
   firstlastMessages() { cout << __FILE__ " compiled on " __DATE__ " " __TIME__ "\n**************************\n"; loginfo(); cout << "DREAM RUN STARTING" << endl; }
   ~firstlastMessages() { cout << "DREAM RUN CLOSED\n"; loginfo(nevtsaved); cout << "**************************\n" << endl; }
 };

firstlastMessages _firstlastMsgs;

bool abort_run=false;

int32_t myUpdateHeader(FILE* file, uint32_t events){
  RunHeader myRH;

  fseek(file,0,SEEK_SET);
  fread(&myRH,sizeof(RunHeader),1,file);
  fseek(file,0,SEEK_SET);

  myRH.endtim=time(NULL);
  myRH.evtsinrun=events;

  fwrite(&myRH, sizeof(RunHeader), 1, file);

  loginfo(events);

  return 0;
}

void cntrl_c_handler(int32_t /* signum */)
{
  cout << "\nGot control-c, end of current run\n" << endl;
  abort_run = true;
}

char datafile[128];
RunHeader myRH;
bool isPhysData(0);
int32_t runnumber(0);
int32_t nfiles(0);
char * workdir(0);
FILE * file(0);

#define MAX_FILE_SIZE 0x60000000
// #define MAX_FILE_SIZE 0x6000000

void initFile()
{
  if(isPhysData)
    snprintf(datafile,128,"%s/data/datafile_run%d_%d.dat",workdir,runnumber,nfiles);
  else
    snprintf(datafile,128,"%s/pedestal/pedestal_run%d_%d.dat",workdir,runnumber,nfiles);

  cout << "Saving data in:   " << datafile << " max size " << MAX_FILE_SIZE << '\n';

  file=fopen(datafile, "w+");
  if(file==NULL){
    cout << "Cannot open file " << datafile << '\n';
    exit(1);
  }

  myRH.magic = IMAGIC;                          // Magic word
  myRH.ruhsiz = sizeof(RunHeader);
  myRH.runnumber = runnumber;                 // Current run number
  myRH.evtsinrun = 0;                          // Not defined, yet...
  myRH.begtim = time(NULL);                     // Start time
  myRH.endtim = 0;                             // Not defined, yet...

  fwrite(&myRH, sizeof(RunHeader), 1, file);

  nfiles ++;
}

int32_t main(int32_t argc, char** argv){

  uint32_t i=0,failed=0;
  struct tms tmsbuf;
  uint32_t ticks_per_sec;
  double time0, cputime, tottime, percent;
  uint32_t key;

  if(argc<2){
    cout << "Usage: " << argv[0] << " datatype" 
	 << " (pedestal or data)\n";
    exit(1);
  }

  --argc; ++argv;
  if(!strcmp(argv[0],"pedestal")){
    isPhysData=false;
  }else if(!strcmp(argv[0],"data")){
    isPhysData=true;
  }else{
    cout << "Unknown data type: " << argv[0] << '\n';
    exit(1);
  }

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

  runnumber = xrunnr;
  if (runnumber <= 0)
   {
    cerr << "****************** ERROR ****************** run number not found "
         << runnumber << " exiting now\n" << endl;
    exit(1);
   }
  cout << "Run number is:    " << runnumber << endl;

  time0 = times(&tmsbuf);
  ticks_per_sec = sysconf(_SC_CLK_TCK);

  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  
  if(isPhysData)
      key=PHYS_BASE_KEY;
  else
    key=PED_BASE_KEY;
  
  myFIFOReader *fifo = new myFIFOReader(key);

  if(fifo->isvalid())
    cout << "Fifo is valid\n";
  else{
    cout << "Fifo is not valid\n";
    abort_run=true;
  }
  
  while(!fifo->isWriterPresent() && !abort_run)
    usleep(1000);

  if (getenv("STORAGEDIR") == NULL)
    workdir = getenv("PWD");
  else
    workdir = getenv("STORAGEDIR");
  cout << "Storage dir is:    " << workdir << '\n';

  initFile();

  char zipcommand[128];

  while(!abort_run){
   
    fifo->waitlock();
    myFIFO::result result =fifo->dumpData(file,1048576);
    fifo->unlock();

    if(result==myFIFO::FIFOEMPTY){
      // cout << "FIFO empty\n";
      uint32_t fsize = ftell(file);
      // cout << i << " file " << datafile << " has size " << fsize << '\n';
      if (fsize >= MAX_FILE_SIZE){
        uint32_t nevts = fifo->getNevt();
        myUpdateHeader(file, nevts-nevtsaved);
        nevtsaved = nevts;
        fclose(file);
        cout << "file " << datafile << " closed (MAX SIZE REACHED)\n";
        snprintf(zipcommand, 128, "bzip2 -9 %s &", datafile);
        // system(zipcommand);
        initFile();
      }
      if(!fifo->isWriterPresent()) break;
      usleep(1000);
    }else if(result==myFIFO::WRLOCKED){
      // cout << "Write locked\n";
      usleep(1000);
    }else if(result==myFIFO::FAILED){
      cout << "Alignment error\n";
      failed++;
      usleep(1000);
    }else if(result==myFIFO::SUCCESS){
      // cout << "Data written "<< i << '\n';
      i++;
    }
  }

  // cout << "Out of the loop\n";
 
  uint32_t nevts = fifo->getNevt();
  myUpdateHeader(file, nevts-nevtsaved);
  nevtsaved = nevts;

  // cout << "Header updated\n";
  
  fclose(file);
  
  // cout << "File closed\n";

  cout << "file " << datafile << " closed (end of run)\n";
  cout << "total number of events " << nevtsaved << '\n';
  snprintf(zipcommand, 128, "bzip2 -9 %s", datafile);
  // system(zipcommand);

  cout << "Write accesses " << i << '\n';
  cout << "Failed " << failed << '\n';
   
  delete fifo;

  tottime = times(&tmsbuf) - time0;
  tottime /= ticks_per_sec;
  cputime = tmsbuf.tms_utime + tmsbuf.tms_stime;
  cputime /= ticks_per_sec;
  percent = cputime/tottime;
  percent = rint(percent*10000)/100;

  cout << "TIME - TOT: " << tottime << " sec - CPU: " << cputime << " sec ("
       << percent << "%)\n";

  return 0;
}
