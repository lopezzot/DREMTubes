#include "myFIFOWriter.h"
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include "myFIFO-IOp.h"

using namespace std;

bool abort_run=false;

void cntrl_c_handler(int sig)
{
  cout << endl << "Got control-c, end of current run" << endl << endl;
  abort_run = true;
}


int main(int argc, char ** argv){

  unsigned int data[100000];
  unsigned int full=0;
  unsigned int spill=0;
  unsigned int size;
  unsigned int totalsize=0;

  time_t t0,t1;

  EventHeader * header = (EventHeader *)data;
  header->evmark=0xBABABABA;

  signal(SIGINT, cntrl_c_handler);      // Control-C handler

  myFIFOWriter * fifo = new myFIFOWriter(0x1970);
  
  if(fifo->isvalid())
    cout << "Fifo is valid " << endl;
  else
    cout << "Fifo is not valid" << endl;

  cout<<"Waiting for reader" << endl;
  fifo->waitReader();
  cout<<"Reader found" << endl;
  
  cout<<"Waiting for lock" << endl;
  fifo->waitlock();
  cout<<"FIFO locked" << endl;

  t0=time(NULL);

  for(unsigned int i=0;spill<5 && !abort_run;i++){
    //usleep(1); // data colleting time
    
    do
      size=(random() % 10000);
    while(size<=sizeof(EventHeader) || size%(sizeof(unsigned int))!=0);

    //size=57*sizeof(unsigned int);

    //cout<<"Event "<< i <<" size " << size << endl;

    header->evhsiz=spill;
    header->evsiz=size;
      
    myFIFO::result result = fifo->write(data);
    
    //cout<<"Event "<< i <<" result " << result << endl;

    if(result==myFIFO::FIFOFULL){
      //cout<<"Event "<< i <<" FIFO full" << endl;
      i--;
      full++;
      fifo->unlock();
      usleep(1000);
      fifo->waitlock();
      //cout<<"FIFO locked" << endl;
    }else if(result==myFIFO::SUCCESS){
      //cout<<"Event "<< i <<" Size " << size << endl;
      totalsize+=size;
    }else if(result==myFIFO::RDLOCKED){
    //  cout<<"Event "<< i <<" RDLOCKED" << endl;
      i--;
    //  usleep(1000);
    }

    t1=time(NULL);
    if(t1-t0>=2){
      cout<<"Event "<< i << endl;
      cout<<"Spill done " << spill << " Unlocking"<< endl;
      fifo->unlock();
      t0=t1;
      while(t1-t0<10 && !abort_run){
	sleep(1);
	t1=time(NULL);
      }
      t0=t1;
      
      if(!abort_run){
	//cout<<"Spill starting. Locking"<< endl;
	fifo->waitlock();
	fifo->updateSamplingPoint();
	//cout<<"Spill starting. Locked"<< endl;
      }
      spill++;
    }
  }

  fifo->unlock();

  delete fifo;

  cout << "Total size " << totalsize << endl;
  cout << "Full times " << full << endl;
}
