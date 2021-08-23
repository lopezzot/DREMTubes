//$Id: myCBD8210.h,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
/*****************************************

  myCBD8210.h
  -----------

  Definition of the CES Camac Branch Driver 8210.
  This class, called "cbd8210", is derived from the class vme.
  This module works in A24 mode only and the base
  address is 0x800000. In this class the VME device
  which is opened is "/dev/vme24d32".

  In this version only branch 1 and crate 1 are defined, but
  since all CAMAC access is based on "noesone.h" it should be
  quite trivial to modify it...

  A. Cardini / INFN Cagliari

  Version 0.1: 29/5/2001

*****************************************/

// Standard C header files
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include "noesone.h"
}

// C++ header files
#include <iostream>
using namespace std;

/*****************************************/
// The cbd8210 class 
/*****************************************/
class cbd8210 : public vme {

 public:

  // Constructor
  cbd8210();    

  // Generic Commands
  inline void clrinh();     // Clears inhibit
  inline void setinh();     // Sets inhibit
  inline void cccc();       // Clears Camac Crate
  inline void cccz();       // Initialize Camac Crate

  // Access Functions
  void r16(int b,int c,int n,int a,int f,unsigned short * data);
  void w16(int b,int c,int n,int a,int f,unsigned short data);
  void r24(int b,int c,int n,int a,int f,unsigned int * data);
  void w24(int b,int c,int n,int a,int f,unsigned int data);

  // CBD8210 Front Panel LEMO Connector I/O functions
  int getInt2();               // Read Status of INT2 input
  int getInt4();               // Read Status of INT4 input
  void resetAck();             // Set to logical 0 both ACKs output
  void pulseAck2();            // 2us width NIM pulse on ACK2
  void pulseAck4();            // 2us width NIM pulse on ACK4
  void setAck2(int level);     // Set level on ACK2
  void setAck4(int level);     // Set level on ACK4

  // Crap
  void print();             // Simple dump function

 protected:

 private:

};

/*****************************************/
// Generic Commands
/*****************************************/
inline void cbd8210::clrinh()     { unsigned short i; r16(1,1,30,9,24,&i); }
inline void cbd8210::setinh()     { unsigned short i; r16(1,1,30,9,26,&i); }
inline void cbd8210::cccc()       { unsigned short i; r16(1,1,28,8,26,&i); }
inline void cbd8210::cccz()       { unsigned short i; r16(1,1,28,9,26,&i); }

/*****************************************/
// Access Functions
/*****************************************/
inline void cbd8210::r16(int b,int c,int n,int a,int f,unsigned short * data) { *data = static_cast<short>(CAM_ACC16(b,c,n,a,f)); }
inline void cbd8210::w16(int b,int c,int n,int a,int f,unsigned short data)  { CAM_ACC16(b,c,n,a,f) = data; }
inline void cbd8210::r24(int b,int c,int n,int a,int f,unsigned int * data)  { *data = static_cast<int>(CAM_ACC24(b,c,n,a,f)); }
inline void cbd8210::w24(int b,int c,int n,int a,int f,unsigned int data)   { CAM_ACC24(b,c,n,a,f) = data; }

/*****************************************/
// CBD8210 Front Panel LEMO Connector I/O functions
/*****************************************/
inline int cbd8210::getInt2()           { unsigned short i; r16(1,0,29,0,0,&i); return i&0x2; }
inline int cbd8210::getInt4()           { unsigned short i; r16(1,0,29,0,0,&i); return i&0x1; }
inline void cbd8210::resetAck()         { w16(1,0,29,0,4,0x0); }
inline void cbd8210::pulseAck2()        { w16(1,0,29,0,4,0x0); w16(1,0,29,0,4,0x2); w16(1,0,29,0,4,0x0); }
inline void cbd8210::pulseAck4()        { w16(1,0,29,0,4,0x0); w16(1,0,29,0,4,0x1); w16(1,0,29,0,4,0x0); }
inline void cbd8210::setAck2(int level) { short i4=getInt4(); short lo=0x0|i4; short hi=0x2|i4; w16(1,0,29,0,4,(level==0?lo:hi)); }
inline void cbd8210::setAck4(int level) { short i2=getInt2(); short lo=0x0|(i2<<1); 
                                          short hi=0x1|(i2<<1); w16(1,0,29,0,4,(level==0?lo:hi)); }
     
/*****************************************/
// Constructor
/*****************************************/
cbd8210::cbd8210():vme(0x0, 0x1000000, "/dev/vme24d32") {MAPCAMAC(vbuf);}

/*****************************************/
void cbd8210::print() 
/*****************************************/
{
  cout << "*************************************" << endl;  
  cout << "CES CBD8210 Module / base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "*************************************" << endl;
  return;
}
