//$Id: myV260.h,v 1.4 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myV260.h
  --------

  Definition of the CAEN V260 Scaler.
  This class is a class derived from the class vme.

  A. Cardini / INFN Cagliari

  Version 0.1: 28/5/2001

*****************************************/

// Standard C header files
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
}

#define V260_CLEAR (0x50)
#define V260_INH_SET (0x52)
#define V260_INH_RESET (0x54)
#define V260_SCALE_INC (0x56)
#define V260_COUNTER(CH) (0x10+CH*4)
#define V260_DISABLE_IRQ (0xA)

// C++ header files
#include <iostream>
using namespace std;

/*****************************************/
// The v260 class 
/*****************************************/
class v260 : public vme {

 public:

  // Constructor
  v260(unsigned int base, const char *dev);    

  // Generic Commands
  int readEvent(unsigned int * evbuff);         // Read event from hardware and stores it in user-provided buffer
  int readEvent(unsigned int * evbuff, unsigned int ncha);         // Read event from hardware and stores it in user-provided buffer
  int readEventMasked(unsigned int * evbuff, unsigned int mask);   // Read event from hardware and stores it in user-provided buffer
  inline void clear();                          // Clears scales, removes int. req. and disable int. generation
  inline void inhibitSet();                     // Sets inhibit
  inline void inhibitReset();                   // Resets inhibit
  inline void scaleIncrement();                 // Increments scales
  inline int  inhibitGet();                     // Check if ihibit is ON by looking only at MSB of channel 0...
  inline void  disableIRQ();                    // disables irqs
  inline unsigned int id();                     // Module ID

  // Crap
  void print();                                 // Simple dump function

 protected:

 private:

};

/*****************************************/
// Generic Commands
/*****************************************/
inline void v260::disableIRQ()            {write16phys(V260_DISABLE_IRQ, 0x0);}
inline void v260::clear()                 { write16phys(V260_CLEAR, 0x0); }
inline void v260::inhibitSet()            { write16phys(V260_INH_SET, 0x0); }
inline void v260::inhibitReset()          { write16phys(V260_INH_RESET, 0x0); }
inline void v260::scaleIncrement()        { write16phys(V260_SCALE_INC, 0x0); }
inline int  v260::inhibitGet()            { unsigned short data; read16phys(V260_COUNTER(0), &data); return (data>>15)==1?0:1; }
inline unsigned int v260::id()            { return ba | ID_V260; }

int v260::readEvent(unsigned int * evbuff)
{
  union {
    unsigned int ldata;
    unsigned short sdata[2];
  } _u;
  unsigned int i;

  for (i=0; i<16; i++) {   // Loop on all channels
    read16phys(V260_COUNTER(i), &(_u.sdata[1]));
    read16phys(V260_COUNTER(i)+0x2, &(_u.sdata[0]));
    *evbuff++ = _u.ldata & 0xF0FFFFFF | (i<<24); 
  }

  return 16;
}

int v260::readEvent(unsigned int * evbuff, unsigned int ncha)
{
  union {
    unsigned int ldata;
    unsigned short sdata[2];
  } _u;
  unsigned int i;

  for (i=0; i<ncha; i++) {   // Loop on all channels
    read16phys(V260_COUNTER(i), &(_u.sdata[1]));
    read16phys(V260_COUNTER(i)+0x2, &(_u.sdata[0]));
    *evbuff++ = _u.ldata & 0xF0FFFFFF | (i<<24); 
  }

  return ncha;
}

int v260::readEventMasked(unsigned int * evbuff, unsigned int mask)
{
  union {
    unsigned int ldata;
    unsigned short sdata[2];
  } _u;
  int sz = 0;
  unsigned int i;
  
  for (i=0; i<16; i++) {   // Loop on all channels
    if((mask&(1<<i))){
      read16phys(V260_COUNTER(i), &(_u.sdata[1]));
      read16phys(V260_COUNTER(i)+0x2, &(_u.sdata[0]));
      *evbuff++ = _u.ldata & 0xF0FFFFFF | (i<<24); 
      sz++;
    }
  }
  return sz;
}

/*****************************************/
// Constructor
/*****************************************/
v260::v260(unsigned int base, const char *dev):vme(base, 0x100, dev) {};

/*****************************************/
void v260::print() 
/*****************************************/
{
  cout << "*************************************" << endl;  
  cout << "CAEN Scaler V260 Module / base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "*************************************" << endl;
  return;
}

/*****************************************/
// V260 Programming Functions
/*****************************************/

