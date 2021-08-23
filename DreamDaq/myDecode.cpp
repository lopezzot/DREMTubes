#include <cstdio>
#include <iostream>

#include "myDecode.h"
#include "myModules.h"
#include "myFIFO-IOp.h"

using namespace std;

#define ADDR_FADC (0x20000000)

#define ADDR_ADC5 (0x10000)
#define ADDR_ADC4 (0x20000)
#define ADDR_ADC3 (0x30000)
#define ADDR_ADC2 (0x40000)
#define ADDR_ADC1 (0x50000)
#define ADDR_ADC0 (0x60000)

#define ADDR_SCALER0 (0x200000)

#define ADDR_TDC0 (0x300000)

#define ADDR_TDC1 (0x7C000000)

int32_t verb;
static uint32_t _evsz_;

int32_t mySubEvent(uint32_t* buffer, int32_t frag){

  SubEventHeader* head = (SubEventHeader*)buffer;

  if(head->semk!=0xacabacab){
    cout << "Cannot find the subevent marker. Something is wrong." << endl;
    return 1;
  }

  if(verb){
    cout << "\n---- SubEvent " << frag << " -----";
    cout << "\nSubHeader size " << head->sevhsiz;
    cout << "\nSubEvent size " << head->size;
    cout << "\nModule ID 0x" << hex << head->id << dec << endl;
  }

  uint32_t result=0;

  switch(head->id){
  default:
    cout << "Unknown module ID 0x" << hex 
	 << head->id << dec << endl;
    result=1;
  }


  if(verb){
    cout << "---- End of SubEvent " << frag << " ----" << endl;
  }
  return result;
}

int32_t myEvent(uint32_t* buffer, int32_t aVerb){
  
  uint32_t i;

  verb=aVerb;

  EventHeader * head=(EventHeader *) buffer;

  if(head->evmark!=0xCAFECAFE){
    cout << "Cannot find the event marker. Something is wrong." << endl;
    return 1;
  }
  
  if(verb){
    cout << "\n##### Event " << head->evnum << " #####"
         << "\nHeader size " << head->evhsiz
         << "\nEvent size " << head->evsiz
         << "\nSpill " << head->spill
         << "\nTime " << head->tsec << " sec + " 
	 << head->tusec << " usec" << endl;
  }
  _evsz_ = *(buffer+2)/sizeof(uint32_t);
  cout << hex << " " << *buffer  << " evsz " << _evsz_
       << "\n##### FADC 7 " << *(buffer+_evsz_-105) << " #8 "
       << *(buffer+_evsz_-95) << dec << endl;
  uint32_t evsizeword=head->evsiz/sizeof(uint32_t);
  i=head->evhsiz/sizeof(uint32_t);
  uint32_t frags=0;

  while(i<evsizeword){
    SubEventHeader* subhead;
    uint32_t subsizeword;
    uint32_t endpoint;
    
    if(buffer[i]==0xACABACAB){
      subhead=(SubEventHeader*)&buffer[i];
      subsizeword=subhead->size/sizeof(uint32_t);
      endpoint=i+subsizeword;
      if(endpoint>evsizeword){
	cout << "Subevent " << frags << " has a wrong size" << endl;
	return 1;
      }
      if(mySubEvent(&buffer[i],frags)!=0){
	cout << "Something is wrong in subevent " << frags 
	     << " stopping here" << endl;
	return 1;
      }
    }else{
      cout << "Cannot find the subevent marker. Something is wrong." << endl;
      return 1;
    }

    frags++;
    i=endpoint;
  }


  if(verb){
    cout << "##### End of Event " << head->evnum << " #####" << endl;
  }

  return 0;
}

