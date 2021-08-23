#include "sched.h"
#include <iostream>

#include "myV262.h"     // CAEN V262 I/O REG
#include "myV513.h"     // CAEN V513 I/O REG

v262 io(0x400000,"/V2718/cvA24_U_DATA/1");
v513 ion(0xa00000,"/V2718/cvA24_U_DATA/1");

inline void enableTriggers() { ion.clearOutputBit(15); } // { CLEAR_IO_PORT(0); } // to enable triggers clear (=0) out 0
inline void disableTriggers() { ion.setOutputBit(15); } // { SET_IO_PORT(0); } // to disable triggers set (=1) out 0

inline void enablePedTriggers() { ion.clearOutputBit(14); } // { CLEAR_IO_PORT(3); } // to enable pedestal triggers clear (=0) out 3
inline void disablePedTriggers() { ion.setOutputBit(14); } // { SET_IO_PORT(3); } // to disable pedestal triggers set (=1) out 3

inline void send_signal() { io.NIMPulse(2); };
inline void unlockTrigger() { io.NIMPulse(0); io.NIMPulse(3); } // to re-enable the trigger logic that goes auto-vetoed after any trigger
inline void resetNIMScaler() { io.NIMPulse(1); } // to reset the NIM scaler

inline bool isT1T2Trig() { return (ion.readInputRegister() & 1<<7); /* io.getNIMIn(0); */ }
inline bool isHWBusy() { return (ion.readInputRegister() & 1<<4); /* io.getNIMIn(1); */ } // return (v2718m.getInputRegister() & 1);
inline bool isInSpill() { return (ion.readInputRegister() & 1<<5); /* io.getNIMIn(2); */ }
inline bool isPedTrig() { return (ion.readInputRegister() & 1<<6); /* io.getNIMIn(3); */ }

using namespace std;

int initV513( v513& ioreg )
 {
  uint16_t w;
  ioreg.read16phys(0xFE, &w);
  sched_yield();
  cout << hex << " V513 FE " << w << endl;
  ioreg.read16phys(0xFC, &w);
  cout << hex << " V513 FC " << w << endl;
  sched_yield();
  ioreg.read16phys(0xFA, &w);
  cout << hex << " V513 FA " << w << endl;
  sched_yield();
  w=0;
  ioreg.write16phys(0x48, w);
  sched_yield();
  ioreg.write16phys(0x46, w);
  sched_yield();
  ioreg.write16phys(0x42, w);
  sched_yield();

  ioreg.reset();
  sched_yield();

  ioreg.read16phys(0x04, &w);
  cout << hex << " V513 0x4 " << w << endl;
  sched_yield();

  for (int i=0; i<8; i++)
   {
    int reg = 0x10+i*2;
    ioreg.setChannelInput(i);
    sched_yield();
    ioreg.read16phys(reg, &w);
    cout << hex << " V513 " << reg << " " << w << endl;
    sched_yield();
   }

  for (int i=8; i<16; i++)
   {
    int reg = 0x10+i*2;
    ioreg.setChannelOutput(i);
    sched_yield();
    ioreg.read16phys(reg, &w);
    cout << hex << " V513 " << reg << " " << w << endl;
    sched_yield();
   }

  return 0;
 }

int main( int argc, char** argv )
 {
  unsigned long spill_nr(0), phytr(0), pedtr(0);

  //Init I/O Registers
  io.reset(); initV513( ion );
  sched_yield();
  
  //Set vetoes
  disableTriggers();
  sched_yield();
  disablePedTriggers();
  sched_yield();

  unlockTrigger();
  resetNIMScaler();

  cout << " starting " << dec << endl;
  cout << " spill nr " << spill_nr << " phytr " << phytr << " pedtr " << pedtr << endl;

  while (1)
   {
    while (!isInSpill()) sched_yield();
    cout << " got a spill " << spill_nr << endl;
    enableTriggers();
    do
     {
      sched_yield();
      bool isPe = isPedTrig();
      bool isPh = isT1T2Trig();
      if (isPh) { phytr ++; if (phytr > pedtr*10+9) enablePedTriggers(); }
      else if (isPe) { pedtr ++; disablePedTriggers(); }
      if (isPh || isPe) unlockTrigger();
     }
    while (isInSpill());
    disableTriggers();
    cout << " spill nr " << spill_nr << " phytr " << phytr << " pedtr " << pedtr << endl;
    sched_yield();
    spill_nr ++;
   }

  return 0;
 }
