//$Id: mySoftScaler.cpp,v 1.1 2006/12/12 10:58:07 cvsdream Exp $

/****************************************/
// Standard C header files
/****************************************/
extern "C" {
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/times.h>
#include <math.h>
}

#include <iostream>

#include "myModules.h"
#include "myFIFOSampler.h"
#include "myFIFO-IOp.h"

#define ADDR_SCALER0 (0x200000)


using namespace std;
                                                                                                        
bool abort_run=false;

void cntrl_c_handler(int sig)
{
  cout << endl << "Got control-c, end of current run" << endl << endl;
  abort_run = true;
}

int main(int argc, char ** argv){

  unsigned int events=0,size,i;
  unsigned int buf[100000];
  struct tms tmsbuf;
  unsigned int ticks_per_sec;
  double time0, cputime, tottime, percent;
  
  time0 = times(&tmsbuf);
  ticks_per_sec = sysconf(_SC_CLK_TCK);

  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  
  myFIFOSampler *fifo = new myFIFOSampler(0x1970);

  if(fifo->isvalid()){
    cout << "Fifo is valid" << endl;
    fifo->waitReader();
  }else{
    cout << "Fifo is not valid" << endl;
    abort_run=true;
  }

  while(!abort_run){

    fifo->waitlock();
    myFIFO::result result =fifo->read(buf,&size);
    fifo->unlock();

    if(result==myFIFO::FIFOEMPTY){
      if(!fifo->isWriterPresent())
	break;
      usleep(10000);
    }else if(result==myFIFO::WRLOCKED || result==myFIFO::RDLOCKED){
      usleep(10000);
    }else if(result==myFIFO::FAILED){
      cout<< "Alignment error" << endl;
      exit(1);
    }else if(result==myFIFO::SUCCESS){
      events++;
    
      EventHeader * head=(EventHeader *) buf;

      //if(head->evmark!=0xCAFECAFE){
	//cout << "Cannot find the event marker. Something is wrong." << endl;
	//return 1;
      //}
      
      unsigned int evsizeword=head->evsiz/sizeof(unsigned int);
      i=head->evhsiz/sizeof(unsigned int);
      unsigned int frags=0;

      while(i<evsizeword){
	SubEventHeader * subhead;
	unsigned int subsizeword;
	unsigned int endpoint;
    
	if(buf[i]==0xACABACAB){
	  subhead=(SubEventHeader*)&buf[i];
	  subsizeword=subhead->size/sizeof(unsigned int);
	  endpoint=i+subsizeword;
	  //if(endpoint>evsizeword){
	  //  cout << "Subevent " << frags << " has a wrong size" << endl;
	  //  return 1;
	  //}
	  
	  SubEventHeader * head=(SubEventHeader *) &buf[i];

	  unsigned int evsizeword=head->size/sizeof(unsigned int);
	  unsigned int shevsizeword=head->sevhsiz/sizeof(unsigned int);
	  unsigned int datasizeword=evsizeword-shevsizeword;

	  if(head->id== ADDR_SCALER0+ID_V260){
	    
	  }
	  //if(mySubEvent(&buffer[i],frags)!=0){
	  //  cout << "Something is wrong in subevent "<< frags 
	  //	 << " stopping here" << endl;
	  //  return 1;
	  //}
	  //}else{
	  //cout << "Cannot find the subevent marker. Something is wrong." << endl;
	  //return 1;
	  //`
	}

	frags++;
	i=endpoint;
      }
    }
 
  }
  
  cout << "Sampled events "<< events << endl;

  delete fifo;

  tottime = times(&tmsbuf) - time0;
  tottime /= ticks_per_sec;
  cputime = tmsbuf.tms_utime + tmsbuf.tms_stime;
  cputime /= ticks_per_sec;
  percent = cputime/tottime;
  percent = rint(percent*10000)/100;

  cout << "TIME - TOT: " << tottime << " sec - CPU: " << cputime << " sec ("       << percent << "%)" << endl;

  return 0;
}
