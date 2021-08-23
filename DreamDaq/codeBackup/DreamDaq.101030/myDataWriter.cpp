//$Id: myDataWriter.cpp,v 1.12 2008/07/20 16:06:47 dreamdaq Exp $

/****************************************/
// Standard C header files
/****************************************/
extern "C" {
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/times.h>
#include <math.h>

#include "myRunNumber.h"
}

#define IMAGIC 0xAABBCCDD

#include "myFIFOReader.h"
#include "myFIFO-IOp.h"
#include <iostream>
using namespace std;

bool abort_run=false;

int myUpdateHeader(FILE * file){
  RunHeader myRH;
  EventHeader myEH;
  unsigned int read;
  unsigned int events=0;

  unsigned int fsize = ftell(file);
  fseek(file,0,SEEK_SET);
  
  fread(&myRH,sizeof(RunHeader),1,file);

  myRH.endtim=time(NULL);

  unsigned int pos0, pos1;
  do{
    pos0 = ftell(file);
    read=fread(&myEH,sizeof(EventHeader),1,file);
    fseek(file,myEH.evsiz-myEH.evhsiz,SEEK_CUR);

    events++;
    pos1 = ftell(file);

  }while(read>0);
  
  events--;
  
  cout << "Events in file : " << events << endl;

  myRH.evtsinrun=events;

  fseek(file,0,SEEK_SET);

  fwrite(&myRH, sizeof(RunHeader), 1, file);

  return 0;
}

void cntrl_c_handler(int sig)
{
  cout << endl << "Got control-c, end of current run" << endl << endl;
  abort_run = true;
}

char datafile[128];
RunHeader myRH;
bool isPhysData(0);
int runnumber(0);
int nfiles(0);
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

  cout << "Saving data in:   " << datafile << " max size " << MAX_FILE_SIZE << endl;

  file=fopen(datafile, "w+");
  if(file==NULL){
    cout << "Cannot open file " << datafile << endl;
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

int main(int argc, char ** argv){

  unsigned int i=0,failed=0;
  struct tms tmsbuf;
  unsigned int ticks_per_sec;
  double time0, cputime, tottime, percent;
  unsigned int key;

  if(argc<2){
    cout << "Usage: " << argv[0] << " datatype" 
	 << " (pedestal or data)" << endl;
    exit(1);
  }

  if(!strcmp(argv[1],"pedestal")){
    isPhysData=false;
  }else if(!strcmp(argv[1],"data")){
    isPhysData=true;
  }else{
    cout << "Unknown data type: " << argv[1] << endl;
    exit(1);
  }
  

  time0 = times(&tmsbuf);
  ticks_per_sec = sysconf(_SC_CLK_TCK);

  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  
  if(isPhysData)
      key=PHYS_BASE_KEY;
  else
    key=PED_BASE_KEY;
  
  myFIFOReader *fifo = new myFIFOReader(key);

  if(fifo->isvalid())
    cout << "Fifo is valid" << endl;
  else{
    cout << "Fifo is not valid" << endl;
    abort_run=true;
  }
  
  while(!fifo->isWriterPresent() && !abort_run)
    usleep(1000);


  runnumber=readRunNumber();

  if(runnumber==-1) exit(1);

  cout << "Run number is:    " << runnumber << endl;


  if (getenv("WORKDIR") == NULL)
    workdir = getenv("PWD");
  else
    workdir = getenv("WORKDIR");

  initFile();

  char zipcommand[128];

  while(!abort_run){
   
    fifo->waitlock();
    myFIFO::result result =fifo->dumpData(file,1048576);
    fifo->unlock();

    if(result==myFIFO::FIFOEMPTY){
      //cout<< "FIFO empty" << endl;
   uint32_t fsize = ftell(file);
   // cout << i << " file " << datafile << " has size " << fsize << endl;
   if (fsize >= MAX_FILE_SIZE){
     myUpdateHeader(file);
     fclose(file);
     cout << "file " << datafile << " closed (MAX SIZE REACHED)" << endl;
     snprintf(zipcommand, 128, "bzip2 -9 %s &", datafile);
     // system(zipcommand);
     initFile();
   }
      if(!fifo->isWriterPresent())
	break;
      usleep(1000);
    }else if(result==myFIFO::WRLOCKED){
      //cout<< "Write locked" << endl;
      usleep(1000);
    }else if(result==myFIFO::FAILED){
      cout<< "Alignment error" << endl;
      failed++;
      usleep(1000);
    }else if(result==myFIFO::SUCCESS){
   uint32_t fsize = ftell(file);
      // cout << "Data written "<< i << endl;
      i++;
    }
  }

  //cout << "Out of the loop " << endl;
 
  myUpdateHeader(file);

  //cout << "Header updated " << endl;
  
  fclose(file);
  
  //cout << "File closed " << endl;

  cout << "file " << datafile << " closed (end of run)" << endl;
  snprintf(zipcommand, 128, "bzip2 -9 %s", datafile);
  // system(zipcommand);

  cout << "Write accesses " << i << endl;
  cout << "Failed " << failed << endl;
   
  delete fifo;

  tottime = times(&tmsbuf) - time0;
  tottime /= ticks_per_sec;
  cputime = tmsbuf.tms_utime + tmsbuf.tms_stime;
  cputime /= ticks_per_sec;
  percent = cputime/tottime;
  percent = rint(percent*10000)/100;

  cout << "TIME - TOT: " << tottime << " sec - CPU: " << cputime << " sec ("
      << percent << "%)" << endl;

  i=5;
  while (i)
   {
    std::cout << i-- << "..";
    sleep(1);
   }
  std::cout << i << std::endl;

  return 0;
}
