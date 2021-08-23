#ifndef _DREAM_DAQ_CLASS_H_
#define _DREAM_DAQ_CLASS_H_

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>

#include <vector>

#define LINUX

#include "myFIFO-IO.h"
#include "myV258.h"
#include "myV814.h"
#include "myV262.h"
#include "myV775.h"
#include "myV792AC.h"
#include "myV862AC.h"
#include "myV1742.h"
#include "myV2718.h"

typedef enum { TRIG_UNLOCKED, TRIG_LOCKED } trigState;

class dreamDaq
 {
  public:
   dreamDaq( int32_t argc, char** argv );
   ~dreamDaq();
   int32_t initDaq();
   int32_t stopDaq();
   inline bool inSpill();
   inline int32_t enableDaq();
   inline void disableDaq();
   inline void unlockTrigger();
   inline void exitPlease();
   inline bool exitNow();
   inline bool hasTrigger();
   inline bool  oscFull();
   inline int32_t acquireBuffers();
   inline int32_t releaseBuffers();
   int32_t readEvent();
   int32_t readOscEvent();
   int32_t oscfflush();
   int32_t handleEvent();
   void oscEvLoop();
   void vmeEvLoop();
   void Run();

  private:
    bool m_isrunning, m_exit;
    uint32_t m_runnr, m_maxphy, m_maxped;
    float m_phy2ped;
    bool m_t1t2, m_drsrdo, m_oscrdo;
    trigState m_state;
    uint32_t m_trigmask;

    std::vector <v258*> m_v258;              // discriminator
    std::vector <v814*> m_v814;              // discriminator
    std::vector <v262*> m_v262;              // I/O
    std::vector <v775*> m_v775;              // TDC
    std::vector <v792ac*> m_v792ac;          // QDC
    std::vector <v862ac*> m_v862ac;          // QDC
    std::vector <v1742*> m_v1742;            // DRS4
    std::vector <v2718*> m_v2718;    // optical link bridge

    v262* m_io;
 };

inline bool dreamDaq::inSpill() { return m_io->getNIMIn(2); }
inline int32_t dreamDaq::enableDaq()
 {
  m_io->disableNIMOut(2);                        // disable all triggers
  m_io->NIMPulse(0);                             // unlock trigger
  m_io->NIMPulse(1);                             // reset NIM scaler
  m_io->enableNIMOut(m_t1t2 ? 0 : 1);            // enable phys trigger
  m_io->enableNIMOut(3);                         // enable ped trigger
  m_io->enableNIMOut(2);                         // enable running
  return 0;
 }
inline void dreamDaq::unlockTrigger() { m_io->NIMPulse(0); }
inline void dreamDaq::disableDaq() { m_io->disableNIMOut(2); }
inline void dreamDaq::exitPlease() { m_exit = true; m_io->disableNIMOut(2); }
inline bool dreamDaq::exitNow() { return m_exit; }
inline bool dreamDaq::hasTrigger()
 {
  uint32_t t_T1T2 = m_io->getNIMIn(0);
  uint32_t t_T1T2T3 = m_io->getNIMIn(1);
  uint32_t t_ped = m_io->getNIMIn(3);
  m_trigmask = (t_T1T2T3<<5) | (t_T1T2<<4) | (t_ped<<1) | (t_T1T2|t_T1T2T3);
  return m_trigmask;
 }
inline bool dreamDaq::oscFull() { return false; }
inline int32_t dreamDaq::acquireBuffers() { return myFIFOlock(); }
inline int32_t dreamDaq::releaseBuffers() { return myFIFOunlock(); }

#endif // _DREAM_DAQ_CLASS_H_
