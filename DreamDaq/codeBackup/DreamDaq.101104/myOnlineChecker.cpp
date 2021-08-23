//$Id: myOnlineChecker.cpp,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#include "myFIFOSampler.h"
#include "myDecode.h"

using namespace std;

bool abort_run=false;

void cntrl_c_handler(int sig)
{
  cout << endl << "Got control-c, end of current run" << endl << endl;
  abort_run = true;
}

int main(int argc, char ** argv){

  unsigned int events=0,size;
  unsigned int buf[100000];

  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  
  myFIFOSampler *fifo = new myFIFOSampler(0x1970);

  if(fifo->isvalid()){
    cout << "Fifo is valid" << endl;
    //fifo->waitReader();
  }
  else{
    cout << "Fifo is not valid" << endl;
    abort_run=true;
  }
  
  

  while(!abort_run){

    //cout << "Looking for event" << endl;

    fifo->waitlock();
    //cout<< "FIFO locked" << endl;
    myFIFO::result result =fifo->read(buf,&size);
    //cout<< "FIFO unlocked" << endl;
    fifo->unlock();

    if(result==myFIFO::FIFOEMPTY){
      //cout<< "FIFO empty" << endl;
      if(!fifo->isWriterPresent())
	break;
      usleep(10000);
    }else if(result==myFIFO::WRLOCKED || result==myFIFO::RDLOCKED){
      //cout<< "Write locked" << endl;
      usleep(10000);
    }else if(result==myFIFO::FAILED){
      cout<< "Alignment error" << endl;
      exit(1);
    }else if(result==myFIFO::SUCCESS){
      //cout << "Data written "<< i << " size " << size << endl;
      //usleep(1000);
      
      if(myEvent(buf,0))
	myEvent(buf,1);

      events++;
    }
 
  }
  
  cout << "Sampled events "<< events << endl;

  delete fifo;
}
