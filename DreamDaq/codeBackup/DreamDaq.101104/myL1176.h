//$Id: myL1176.h,v 1.4 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  myL1176.h
  ---------

  Definition of the LeCroy 1176 TDC.
  This class is a class derived from the class vme.

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

// C++ header files
#include <iostream>

#define L1176_TEST_HITS_1 (0x0)
#define L1176_TEST_HITS_2 (0x1)
#define L1176_TEST_HITS_4 (0x2)
#define L1176_TEST_HITS_8 (0x3)

#define L1176_TEST_HITDUR_625 (0x0)
#define L1176_TEST_HITDUR_125 (0x1)
#define L1176_TEST_HITDUR_500 (0x2)
#define L1176_TEST_HITDUR_1000 (0x3)

#define L1176_COMMSTART_TIMEOUT_5 (0x0)
#define L1176_COMMSTART_TIMEOUT_1 (0x1)
#define L1176_COMMSTART_TIMEOUT_2 (0x2)
#define L1176_COMMSTART_TIMEOUT_4 (0x4)
#define L1176_COMMSTART_TIMEOUT_16 (0x5)
#define L1176_COMMSTART_TIMEOUT_32 (0x6)
#define L1176_COMMSTART_TIMEOUT_FRONT (0x7)

#define L1176_FCLWIN_1 (0x0)
#define L1176_FCLWIN_2 (0x1)
#define L1176_FCLWIN_4 (0x2)
#define L1176_FCLWIN_16 (0x4)
#define L1176_FCLWIN_32 (0x5)
#define L1176_FCLWIN_64 (0x6)
#define L1176_FCLWIN_128 (0x7)

#define L1176_CSR0 (0x8000)
#define L1176_EVENT(N) (0x400*N)

#define L1176_SETBITS(OFF,BITS){ \
unsigned int val; \
read32phys(OFF,&val); \
val |= BITS; \
write32phys(OFF,val); \
}

#define L1176_RESETBITS(OFF,BITS){ \
unsigned int val; \
read32phys(OFF,&val); \
val &= (~(BITS)); \
write32phys(OFF,val); \
}

#define L1176_GETBITS(OFF,BITS,SHIFT){ \
unsigned int val; \
read32phys(OFF,&val); \
return ((val&BITS)>>SHIFT); \
}

using namespace std;

/*****************************************/
// The l1176 class 
/*****************************************/
class l1176 : public vme {

 public:

  // Constructor
  l1176(unsigned int base, const char *dev);    

  // Crap
  void print();                                 // Simple dump function

  inline unsigned int id();                     // Module ID
 
  //Control register
  void testCycle();      //Execute test cycle
  void enableEvenChTest();   //Enable test for even channels
  void disableEvenChTest();   //disable test for even channels
  void enableOddChTest();   //Enable test for odd channels
  void disableOddChTest();   //disable test for odd channels
  void setTestNHits(unsigned int hits);      //Set number of hists during test
  void setTestHitDuration(unsigned int time); //Set duration of test hits
  void setCommStartTimeout(unsigned int time);  //Set common start timeout
  void enableAuxCommonHit();                   //??????
  void disableAuxCommonHit();                   //??????
  void enableFallingEdge();     //Enable falling edge registration
  void disableFallingEdge();     //disable falling edge registration
  void enableRisingEdge();     //Enable rising edge registration
  void disableRisingEdge();     //disable rising edge registration
  void setTriggerPulse();   //Each hit
  void setTriggerLatch();   //Latch first hit
  void setCommStart();     //Common start
  void setCommStop();      //Common stop
  void setFastCLWindow(unsigned int time); //set fast clear window
  void enableFastClear();    //enable fast clear
  void disableFastClear();    //disable fast clear
  void enableAcq();        //enable data acquisition
  void disableAcq();       //disable data acquisition
  unsigned int isAcqEnabled();      //acquisition status
  unsigned int roip();          //read-out in progress
  void clear();         // clear
  unsigned int evtNum();    //return number of events
  unsigned int isBufferFull();  //return bufer full bit
  unsigned int getCSR0();

  unsigned int readSingleEvent(unsigned int *buf);  //read data
  unsigned int readData(unsigned int *buf);  //read data

 protected:

 private:

};

/*****************************************/
// Generic Commands
/*****************************************/


inline unsigned int l1176::id() {return ba | ID_L1176;}


/*****************************************/
// Constructor
/*****************************************/
l1176::l1176(unsigned int base, const char *dev):vme(base, 0x800F, dev) {};

/*****************************************/
void l1176::print() 
/*****************************************/
{
  cout << "*************************************" << endl;  
  cout << "LeCroy TDC L1176 Module / base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "*************************************" << endl;
  return;
}

/*****************************************/
// L1176 Programming Functions
/*****************************************/

inline unsigned int l1176::getCSR0(){
  L1176_GETBITS(L1176_CSR0,0xffffffff,0);
}

inline void l1176::testCycle(){
  L1176_SETBITS(L1176_CSR0,0x0);
}

inline void l1176::enableEvenChTest(){
  L1176_SETBITS(L1176_CSR0,0x4);
}

inline void l1176::disableEvenChTest(){
  L1176_RESETBITS(L1176_CSR0,0x4);
}

inline void l1176::enableOddChTest(){
  L1176_SETBITS(L1176_CSR0,0x8);
}

inline void l1176::disableOddChTest(){
  L1176_RESETBITS(L1176_CSR0,0x8);
}

inline void l1176::setTestNHits(unsigned int hits){
  unsigned int val;
  volatile unsigned int vol;
  read32phys(L1176_CSR0,&val);
  vol=val&(~(0x30));
  val=vol|((hits&0x3)<<4);
  write32phys(L1176_CSR0,val);
}

inline void l1176::setTestHitDuration(unsigned int time){
  unsigned int val;
  volatile unsigned int vol;
  read32phys(L1176_CSR0,&val);
  vol=val&(~(0xC0));
  val=vol|((time&0x3)<<6);
  write32phys(L1176_CSR0,val);
}

inline void l1176::setCommStartTimeout(unsigned int time){
  unsigned int val;
  volatile unsigned int vol;
  read32phys(L1176_CSR0,&val);
  vol=val&(~(0x700));
  val=vol|((time&0x7)<<8);
  write32phys(L1176_CSR0,val);
}

inline void l1176::enableAuxCommonHit(){
  L1176_SETBITS(L1176_CSR0,0x800);
}

inline void l1176::disableAuxCommonHit(){
  L1176_RESETBITS(L1176_CSR0,0x800);
}

inline void l1176::enableFallingEdge(){
  L1176_RESETBITS(L1176_CSR0,0x1000);
}

inline void l1176::disableFallingEdge(){
  L1176_SETBITS(L1176_CSR0,0x1000);
}
 
inline void l1176::enableRisingEdge(){
  L1176_RESETBITS(L1176_CSR0,0x2000);
}

inline void l1176::disableRisingEdge(){
  L1176_SETBITS(L1176_CSR0,0x2000);
}

inline void l1176::setTriggerPulse(){
  L1176_SETBITS(L1176_CSR0,0x4000);
}
 
inline void l1176::setTriggerLatch(){
  L1176_RESETBITS(L1176_CSR0,0x4000);
}
 
inline void l1176::setCommStart(){
  L1176_SETBITS(L1176_CSR0,0x8000);
}
 
inline void l1176::setCommStop(){
  L1176_RESETBITS(L1176_CSR0,0x8000);
}

inline void l1176::setFastCLWindow(unsigned int time){
  unsigned int val;
  volatile unsigned int vol;
  read32phys(L1176_CSR0,&val);
  vol=val&(~(0x70000));
  val=vol|((time&0x7)<<16);
  write32phys(L1176_CSR0,val);
}
 
inline void l1176::enableFastClear(){
  L1176_SETBITS(L1176_CSR0,0x80000);
}
 
inline void l1176::disableFastClear(){
  L1176_RESETBITS(L1176_CSR0,0x80000);
}

inline void l1176::enableAcq(){
  L1176_RESETBITS(L1176_CSR0,0x100000);
}

inline void l1176::disableAcq(){
  L1176_SETBITS(L1176_CSR0,0x100000);
}

inline unsigned int l1176::isAcqEnabled(){
  L1176_GETBITS(L1176_CSR0,0x200000,21);
}

inline unsigned int l1176::roip(){
  L1176_GETBITS(L1176_CSR0,0x400000,22);
}

inline void l1176::clear(){
  L1176_SETBITS(L1176_CSR0,0x800000);
}
 
inline unsigned int l1176::evtNum(){
  L1176_GETBITS(L1176_CSR0,0x1F000000,24);
}

inline unsigned int l1176::isBufferFull(){
  L1176_GETBITS(L1176_CSR0,0x20000000,29);
}
  
unsigned int l1176::readSingleEvent(unsigned int *buf){
  unsigned int val, count=0;

  *buf++ = 0xDEAAED00;
  count=0;
  do {
    // read32phys(L1176_EVENT(0)+0x8*count,&val);
    read32phys(4*count,&val);
    *buf++ = val;
    count ++;
  } while (val&0x800000);

  return count+1;
}

unsigned int l1176::readData(unsigned int *buf){
  unsigned int i,size=0;
  unsigned int evts=evtNum();
  unsigned int val;
  unsigned int count;

  if(evts==0)
    return 0;


  for(i=0;i<evts;i++){
    *buf++ = 0xDEAAED00+i;
    size++;
    count=0;
    do{
      // read32phys(L1176_EVENT(i)+0x8*count,&val);   // BOB ??? come 8 ?
      read32phys(L1176_EVENT(i)+0x4*count,&val);   // BOB
      *buf++ = val;
      size++;
      count++;
    }
    while((val&0x800000));
  }
  
  return size;
}

