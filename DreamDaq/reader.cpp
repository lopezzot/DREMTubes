#include <unistd.h>
#include <stdint.h>

#include <iostream>
#include <csignal>
#include <cstdio>
#include <cstdlib>

#include "myFIFOReader.h"

using namespace std;

bool abort_run=false;

void cntrl_c_handler(int32_t /* signum */)
{
  cout << "\nGot control-c, end of current run\n" << endl;
  abort_run = true;
}

int32_t main(int32_t , char** ){

  uint32_t i=0,failed=0;
  char datafile[50];
 
  signal(SIGINT, cntrl_c_handler);      // Control-C handler

  FILE *file=fopen("datafile", "w");
  if(file==NULL){
    cout << "Cannot open file " << datafile << endl;
    exit(1);
  }

  myFIFOReader *fifo = new myFIFOReader(0x1970);

  if(fifo->isvalid())
    cout << "Fifo is valid" << endl;
  else{
    cout << "Fifo is not valid" << endl;
    abort_run=true;
  }
  
  while(!abort_run){
   
    fifo->waitlock();
    myFIFO::result result =fifo->dumpData(file,1048576);
    fifo->unlock();

    if(result==myFIFO::FIFOEMPTY){
      // cout << "FIFO empty" << endl;
      if(!fifo->isWriterPresent())
	break;
      usleep(1000);
    }else if(result==myFIFO::WRLOCKED){
      // cout << "Write locked" << endl;
      usleep(1000);
    }else if(result==myFIFO::FAILED){
      cout << "Alignment error" << endl;
      failed++;
      usleep(1000);
    }else if(result==myFIFO::SUCCESS){
      // cout << "Data written " << i << endl;
      i++;
    }
   
  }
 
  fclose(file);
  
  cout << "Write access " << i;
  cout << "\nFailed " << failed << endl;
   
  delete fifo;
}
