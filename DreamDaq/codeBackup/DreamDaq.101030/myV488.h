//$Id: myV488.h,v 1.2 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myV488.h
  ---------

  Definition of the CAEN TDC V488 class.
  This class is a class derived from the class vme.

  D. Raspino / INFN Cagliari

  Version 0.1: 25/8/2004

*****************************************/

// Standard C header files
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include "myModules.h"
#include <sys/time.h>
}

// C++ header files
#include <iostream>
using namespace std;

/*****************************************/
// The v488 class 
/*****************************************/
class v488 : public vme {
  
 public:
  
  // Constructor
  v488(unsigned int base, const char *dev);    
  
  
  //Generic Commands
  inline unsigned int id();                       // Module ID
  inline void reset();                            // Module Reset

  // Acquisition Mode
  inline int dready();
  void  setHalfFullModeOB();                      // Module BUSY if number of data > half-size 
  void  setFullModeOB();                          // Module BUSY if number of data > Full-size 
  void  enableChannel(int cha);                   // Enable Channel
  void  disableChannel(int cha);                  // Enable Channel
  int   getOutputBufferStatus();                  //Get Output Buffer Status
  void  setCommonStart();                         // Set Common Start Mode
  void  setCommonStop();                          // Set Common Stop Mode
  int   readEvent(unsigned int *evbuff);         // Read event from hardware and stores it in user-provided buffer
  void  setRange(unsigned int ww);               // Set TDC range for all channels
  unsigned short   getFIFOMode();
  void  setHighThr(int thr);                     // Set the High Threshold
  void  setLowThr(int thr);                     // Set the low Threshold


  void  print();



 protected:

 private:

};
  
  /*****************************************/
  // Generic Commands
  /*****************************************/

inline unsigned int v488::id() { return ba | ID_V488; }
inline void v488::reset()    { write16phys(0x1C, 0x0); }

inline int v488::dready()    { unsigned short i; read16phys(0x1A, &i); return ((i>>14)&1); }
  
/*****************************************/
// Acquisition Mode
/*****************************************/
 
  
void v488::setHalfFullModeOB()  { write16phys(0x1E, 0x0); }
void v488::setFullModeOB()      { write16phys(0x16, 0x0); }
  

/**************************************************************/
//   En/Dis able Channel
/**************************************************************/

void v488::enableChannel(int cha)
  {
    unsigned short val;
    if ((cha < 0)||(cha > 7)) {
      cout << "enableChannel: cannot set - channel " << cha << " does not exist" << endl;
      return;
    }
    read16phys(0x1A, &val);
    write16phys(0x1A, val|(0x1<<cha));
  }	

void v488::disableChannel(int cha)
  {
    unsigned short val;
    if ((cha < 0)||(cha > 7)) {
      cout << "disableChannel: cannot set - channel " << cha << " does not exist" << endl;
      return;
    }
    read16phys(0x1A, &val);
    write16phys(0x1A, val&(~(0x1<<cha)));
  }	
 
/**************************************************************/
//   Get Output Buffer Status
/**************************************************************/

 int v488::getOutputBufferStatus()   
   {
     unsigned short val;
     unsigned short status;
     
     read16phys(0x1A, &val);
     cout <<  "val vale:" << hex << val << endl;
     status=(val&0x7000)>>12;
     
     switch(status) {
     case 6:
       cout << "Output Buffer is Half Full, Status=" << status << endl;
       break;
     case 5:
       cout << "Output Buffer is Full, Status=" << status  << endl;
       break;
     case 3:
       cout << "Output Buffer is Empty, Status=" << status << endl;
       break;
     }
     return status;
   }


/**************************************************************/
//   Common Start/Stop Mode
/**************************************************************/

void v488::setCommonStart()   
{
  unsigned short val;    
  
  read16phys(0x1A, &val);
  write16phys(0x1A, val&(~(0x1<<15)));
}

 void v488::setCommonStop()   
{
  unsigned short val;     
  
  read16phys(0x1A, &val);
  write16phys(0x1A, val|((0x1<<15)));
}


/*****************************************/
// Read Event
/*****************************************/

int v488::readEvent(unsigned *evbuff)
{
  unsigned short data;
  unsigned int sz,i,j;
  struct timespec req;

  req.tv_sec=0;
  req.tv_nsec=100;

  read16phys(0x18, &data);

  *evbuff++ = data;
  sz=((data&0x7000)>>12)+2;    // Ale: +2 //Wainer +2 perche' c'e' anche l'header nel buffer
  
  //We need to wait between the two read operations
  //otherwise data is corrupted. We cannot use usleep 
  //or nanosleep because the waiting time gets too long
  // probably because of the context switching
  j=0;
  for(i=0;i<3000;i++) j++;

  if((data&0x8000)>>15){
    for(i=1; i<sz; i++) { 
      read16phys(0x18, &data); 
      *evbuff++ = data;
      j=0;
      for(i=0;i<3000;i++) j++;
    }
  }else
    sz=0;

  return sz;
}

/*****************************************/
// Select Range
/*****************************************/

void v488::setRange(unsigned int ww)
    {
      if ((ww < 0x0)||(ww > 0xE0)) cout << "ww must be between 0x0 and 0xE0" << endl;
      else  write16phys(0x14, ww);
    }
      
/*****************************************/
// Get FIFO Status Operating Mode
/*****************************************/

  unsigned short v488::getFIFOMode() { 
    //
    unsigned short j;
    read16phys(0x14, &j); 
    j=(j & 0x1000)>>12;
    switch(j) {
    case 0:
      cout << "Fifo is in Half Mode , Status=" << hex << j << endl;
      break;
    case 1:
      cout << "Fifo is in Full Mode, Status=" << hex << j << endl;
      break;
    default:
      cout << "Some error in the Fifo Mode, Status=" << hex << j << endl;
       break;
    }

    //    cout << "status of FIFO: 0x" << hex << j << endl;
    return j;
  }

  

void v488::setHighThr(int thr)      { write16phys(0x12, thr); }
void v488::setLowThr(int thr)      { write16phys(0x10, thr); }


/*****************************************/
// Constructor
/*****************************************/
// v488::v488(unsigned int base, const char *dev):vme(base, 0xD000, dev) {};
v488::v488(unsigned int base, const char *dev):vme(base, 0x100, dev) {};

/*****************************************/
void v488::print() 
/*****************************************/
{
  cout << "************************************" << endl;  
  cout << "  CAEN  TDC  V488    Module  /  base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "************************************" << endl;
  return;
}

/*****************************************/
// V488 Programming Functions
/*****************************************/
