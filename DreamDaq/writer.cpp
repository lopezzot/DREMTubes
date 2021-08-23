#include <unistd.h>
#include <ctime>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include "myFIFOWriter.h"
#include "myFIFO-IOp.h"

using namespace std;

bool abort_run=false;

void cntrl_c_handler(int32_t /* signum */)
{
  cout << "\nGot control-c, end of current run\n" << endl;
  abort_run = true;
}

int32_t main(int32_t , char** ){

  uint32_t data[100000];
  uint32_t full=0;
  uint32_t spill=0;
  uint32_t size;
  uint32_t totalsize=0;

  time_t t0,t1;

  EventHeader* header = (EventHeader*)data;
  header->evmark=0xBABABABA;

  signal(SIGINT, cntrl_c_handler);      // Control-C handler

  myFIFOWriter* fifo = new myFIFOWriter(0x1970);
  
  if(fifo->isvalid())
    cout << "Fifo is valid" << endl;
  else
    cout << "Fifo is not valid" << endl;

  cout << "Waiting for reader" << endl;
  fifo->waitReader();
  cout << "Reader found" << endl;
  
  cout << "Waiting for lock" << endl;
  fifo->waitlock();
  cout << "FIFO locked" << endl;

  t0=time(NULL);

  for(uint32_t i=0;spill<5 && !abort_run;i++){
    
    do
      size=(random() % 10000);
    while(size<=sizeof(EventHeader) || size%(sizeof(uint32_t))!=0);

    header->evhsiz=spill;
    header->evsiz=size;
      
    myFIFO::result result = fifo->write(data);
    
    //cout << "Event " << i << " result " << result << endl;

    if(result==myFIFO::FIFOFULL){
      //cout << "Event " << i << " FIFO full" << endl;
      i--;
      full++;
      fifo->unlock();
      usleep(1000);
      fifo->waitlock();
      //cout << "FIFO locked" << endl;
    }else if(result==myFIFO::SUCCESS){
      //cout << "Event " << i << " Size " << size << endl;
      totalsize+=size;
    }else if(result==myFIFO::RDLOCKED){
    //  cout << "Event " << i << " RDLOCKED" << endl;
      i--;
    }

    t1=time(NULL);
    if(t1-t0>=2){
      cout << "Event " << i;
      cout << "\nSpill done " << spill << " Unlocking" << endl;
      fifo->unlock();
      t0=t1;
      while(t1-t0<10 && !abort_run){
	usleep(1000);
	t1=time(NULL);
      }
      t0=t1;
      
      if(!abort_run){
	//cout << "Spill starting. Locking" << endl;
	fifo->waitlock();
	fifo->updateSamplingPoint();
	//cout << "Spill starting. Locked" << endl;
      }
      spill++;
    }
  }

  fifo->unlock();

  delete fifo;

  cout << "Total size " << totalsize;
  cout << "\nFull times " << full << endl;
}
