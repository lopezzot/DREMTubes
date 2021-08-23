//$Id: myDecode.cpp,v 1.3 2006/10/31 15:08:50 cvsdream Exp $
#include <iostream>
#include <stdio.h>

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

int verb;
static unsigned int _evsz_;

int mySIS3320Decode(unsigned int *buffer, unsigned int datasize){

  unsigned int i;

  if(verb){
    cout<< endl;
    cout << "SIS3320 Fast ADC Data" << endl;
  }

  unsigned olddata=0;
  unsigned int channels=0;
  i=0;
  while(i<datasize){
    if((buffer[i]&0xFFFF0000)!=0xABCD0000){
      cout << "Cannot find the channel marker. Something is wrong." << endl;
      return 1;
    }
    unsigned int channel=buffer[i]&0xFFFF;
    if(verb){
      cout << "Channel " << channel << " data:"<< endl;
    }
    unsigned int data=0;
    i++;
    while(i<datasize && buffer[i]!=0xABCDDBCA){
      if(verb){
	unsigned int value=buffer[i]&0xFFF;
	cout << "Value " << data <<": " << value << endl;
	value=(buffer[i]&0x0FFF0000)>>16;
	cout << "Value " << data+1 <<": " << value << endl;
      }
      data+=2;
      i++;
    }
    i++;
    if(channel%2==0 && olddata!=data){
       cout << "Channel have different number of samples. Strange..." << endl;
       return 1;
    }
    olddata=data;
    channels++;
    if(verb){
      cout << endl;
    }
  }

  if(buffer[i-1]!=0xABCDDBCA){
    cout << "Cannot find channel trailer" << endl;
    return 1;
  }

  return 0;
}

int myL1182Decode(unsigned int *buffer, unsigned int datasize,
		  unsigned int number){
  unsigned int i;

  if(verb){
    cout<< endl;
    cout << "LeCroy ADC Number " << number <<" Data" << endl;
  }

  if(datasize>120){
     cout << "Wrong datasize" << endl;
     return 1;
  }

  
    
  i=0;
  while(i<datasize){
    unsigned int evt=buffer[i]&0xFF;
    unsigned int mark=buffer[i]&0xFFFFFF00;
    if(mark!=0xFCBBCF00){
      cout << "Cannot find the data header" << endl;
      return 1;
    }
    i++;
    unsigned int ch=0;
    while(i<datasize && 
	  (buffer[i]&0xFFFFFF00)!=0xABCCBA00 
	  && ch<8){
      unsigned int channel=(buffer[i]&0xF000)>>12;
      unsigned int data=buffer[i]&0xFFF;
      
      if(verb){
	cout << "Event " << evt 
	     << " Channel " << channel
	     << " Data " << data << endl;
      }
      i++;
      ch++;
    }
    
    if((buffer[i]&0xFFFFFF00)!=0xABCCBA00){	
      cout << "More than 16 channel in an event??" 
	   << "Probably the trailer is missing" << endl;
      return 1;
    }
    
    if((buffer[i]&0xFF)!=evt){
      cout << "The trailer belongs to a different event" << endl;
      return 1;
    }
    
    i++;
  }
  

  if(verb){
    cout<< endl;
  }
  return 0;
}

int myV260Decode(unsigned int *buffer, unsigned int datasize){

  unsigned int i;

  if(verb){
    cout<< endl;
    cout << "Caen V260 Scaler Data" << endl;
  }

  if(datasize>16){
    cout << "Wrong datasize " << datasize << endl;
    return 1;
  }

  if(verb){
    i=0;
    while(i<datasize){
      unsigned int channel=(buffer[i]&0xF000000)>>24;
      unsigned int data=buffer[i]&0xFFFFFF;
      cout << "Channel " << channel << " Data " << data << endl;
      i++;
    }
  }

  if(verb){
    cout<< endl;
  }

  return 0;
}


int myL1176Decode(unsigned int *buffer, unsigned int datasize){

  unsigned int i;

  if(verb){
    cout<< endl;
    cout << "LeCroy TDC Data" << endl;
  }
  
  if(datasize>0x7FFF){
    cout << "Wrong datasize" << endl;
    return 1;
  }

      
  i=0;
  while(i<datasize){
    unsigned int evt=buffer[i]&0xFF;
    unsigned int mark=buffer[i]&0xFFFFFF00;
    if(mark!=0xDEAAED00){
      cout << "Cannot find the data header" << endl;
      return 1;
    }
    
    i++;
    unsigned int last=1;
    while(i<datasize && last==1){
      unsigned int data=buffer[i]&0xFFFF;
      unsigned int edge=(buffer[i]&0x10000)>>16;
      unsigned int channel=(buffer[i]&0x1E0000)>>17;
      unsigned int valid=(buffer[i]&0x200000)>>21;
      last=(buffer[i]&0x800000)>>23;

      if(verb){
	cout << "Event " << evt 
	     << " Channel " << channel
	     << " Data " << data 
	     << " Valid "<< valid //0=good 1=corrupt
	     << " Edge " << edge << endl; //0=falling 1=rising
      }
      
      i++;
    }
  }
  

  if(verb){
    cout<< endl;
  }

  return 0;
}

int myKLOETDCDecode(unsigned int *buffer, unsigned int datasize){

  unsigned int i;

  if(verb){
    cout<< endl;
    cout << "KLOE TDC Data" << endl;
  }

  unsigned int last=0;
  unsigned int datapresent=0;
  i=0;
  while(i<datasize){
    if((buffer[i]&(1<<23))!=1){
      cout << "Cannot find the event number word" << endl;
      return 1;
    }
    unsigned int evn=buffer[i]&0xFFFF;
   datapresent=buffer[i]&(1<<25);
    
   i++;
    if(datapresent){
      last=1;
      while(i<datasize && last){
	unsigned int data=buffer[i]&0xFFFF;
	unsigned int edge=buffer[i]&(1<<16);
	unsigned int over=buffer[i]&(1<<17);
	unsigned int ch=buffer[i]&0x7C0000;
	
	last=buffer[i]&(1<<24);
	
	if(verb){
	  cout << "Event " << evn
	       << " Channel " << ch
	       << " Data " << data
	       << " Edge " << edge
	       << " Overflow " << over << endl;
	}
	
	i++;
      }
    }else{
      if(verb){
	cout << "Event "<< evn << " contains no data" << endl;
      }
    }
  }

  if(last && datapresent){
    cout << "Wrong end of dtat buffer" << endl;
    return 1;
  }

  if(verb){
    cout<< endl;
  }

  return 0;
}


int mySubEvent(unsigned int *buffer, int frag){

  SubEventHeader * head=(SubEventHeader *) buffer;

  if(head->semk!=0xacabacab){
    cout << "Cannot find the subevent marker. Something is wrong." << endl;
    return 1;
  }

  if(verb){
    cout << endl;
    cout << "---- SubEvent " << frag << " -----" << endl;
    cout << "SubHeader size " << head->sevhsiz << endl;
    cout << "SubEvent size " << head->size << endl;
    cout << "Module ID 0x" << hex << head->id << dec << endl;
  }

  unsigned int evsizeword=head->size/sizeof(unsigned int);
  unsigned int shevsizeword=head->sevhsiz/sizeof(unsigned int);
  unsigned int datasizeword=evsizeword-shevsizeword;

  unsigned int result=0;

  switch(head->id){
  case ADDR_FADC+ID_SIS3320:
    result=mySIS3320Decode(&buffer[shevsizeword],datasizeword);
    break;
  case ADDR_ADC0+ID_L1182:
    result=myL1182Decode(&buffer[shevsizeword],datasizeword,0);
    break;
  case ADDR_ADC1+ID_L1182:
    result=myL1182Decode(&buffer[shevsizeword],datasizeword,1);
    break;
  case ADDR_ADC2+ID_L1182:
    result=myL1182Decode(&buffer[shevsizeword],datasizeword,2);
    break;
  case ADDR_ADC3+ID_L1182:
    result=myL1182Decode(&buffer[shevsizeword],datasizeword,3);
    break;
  case ADDR_ADC4+ID_L1182:
    result=myL1182Decode(&buffer[shevsizeword],datasizeword,4);
    break;
  case ADDR_ADC5+ID_L1182:
    result=myL1182Decode(&buffer[shevsizeword],datasizeword,5);
    break;
  case ADDR_SCALER0+ID_V260:
    result=myV260Decode(&buffer[shevsizeword],datasizeword);
    break;
  case ADDR_TDC0+ID_L1176:
    result=myL1176Decode(&buffer[shevsizeword],datasizeword);
    break;
  case ADDR_TDC1+ID_KLOETDC:
    result=myKLOETDCDecode(&buffer[shevsizeword],datasizeword);
    break;
  default:
    cout<< "Unknown module ID 0x"<< hex 
	<<head->id << dec<< endl;
    result=1;
  }


  if(verb){
    cout << "---- End of SubEvent " << frag <<" ----" << endl;
  }
  return result;
}

int myEvent(unsigned int * buffer,int aVerb){
  
  unsigned int i;

  verb=aVerb;

  EventHeader * head=(EventHeader *) buffer;

  if(head->evmark!=0xCAFECAFE){
    cout << "Cannot find the event marker. Something is wrong." << endl;
    return 1;
  }
  
  if(verb){
    cout << endl;
    cout << "##### Event " << head->evnum << " #####" << endl;
    cout << "Header size " << head->evhsiz << endl;
    cout << "Event size " << head->evsiz << endl;
    cout << "Spill " << head->spill << endl;
    cout << "Time " << head->tsec << " sec + " 
	 << head->tusec << " usec" << endl;
  }
  _evsz_ = *(buffer+2)/sizeof(unsigned int);
  cout << hex << " " << *buffer  << " evsz " << _evsz_ << endl;
  cout << hex << "##### FADC 7 " << *(buffer+_evsz_-105) << " #8 "
	<< *(buffer+_evsz_-95) << dec << endl;
  unsigned int evsizeword=head->evsiz/sizeof(unsigned int);
  i=head->evhsiz/sizeof(unsigned int);
  unsigned int frags=0;

  while(i<evsizeword){
    SubEventHeader * subhead;
    unsigned int subsizeword;
    unsigned int endpoint;
    
    if(buffer[i]==0xACABACAB){
      subhead=(SubEventHeader*)&buffer[i];
      subsizeword=subhead->size/sizeof(unsigned int);
      endpoint=i+subsizeword;
      if(endpoint>evsizeword){
	cout << "Subevent " << frags << " has a wrong size" << endl;
	return 1;
      }
      if(mySubEvent(&buffer[i],frags)!=0){
	cout << "Something is wrong in subevent "<< frags 
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
    cout << "##### End of Event " << head->evnum <<" #####" << endl;
  }

  return 0;
}

