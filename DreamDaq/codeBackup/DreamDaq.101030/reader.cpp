//$Id: reader.cpp,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#include "myFIFOReader.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

using namespace std;

bool abort_run=false;

void cntrl_c_handler(int sig)
{
  cout << endl << "Got control-c, end of current run" << endl << endl;
  abort_run = true;
}

int main(int argc, char ** argv){

  unsigned int i=0,failed=0;
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
      //cout<< "FIFO empty" << endl;
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
      //cout << "Data written "<< i << endl;
      i++;
    }
   
  }
 
  fclose(file);
  
  cout << "Write access " << i << endl;
  cout << "Failed " << failed << endl;
   
  delete fifo;
}
