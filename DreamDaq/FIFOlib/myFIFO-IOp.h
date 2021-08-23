#ifndef _MYFIFO_IOP_
#define _MYFIFO_IOP_
/*************************************************************************

        myFIFO-IOp.h
        ------------

        This file contains event structures definitions

        Version 2014 25.11.2014 roberto.ferrari@pv.infn.it

*************************************************************************/

#include <stdint.h>

#define EVMARK 0xCAFECAFE
#define SUBEVMARK 0xACABACAB

#define EVOFFTOSIZE 2
#define EVOFFTONUM 3

#define SUBEVOFFTOID 2
#define SUBEVOFFTOSIZE 3

// Data Structures Definitions

typedef struct {
  uint32_t magic;            // Magic word - 0xaabbccdd
  uint32_t ruhsiz;           // Run Header size in bytes
  uint32_t runnumber;        // Run number
  uint32_t evtsinrun;        // Events in this run (if available)
  uint32_t begtim;        // Time at beginning of run
  uint32_t endtim;        // Time at end of run
} RunHeader;

typedef struct {
  uint32_t evmark;           // Beginning of event marker - 0xcafecafe
  uint32_t evhsiz;           // Event Header size in bytes
  uint32_t evsiz;            // Event size in bytes (including header)
  uint32_t evnum;            // Event number
  uint32_t spill;            // Spill number
  uint32_t tsec;             // Seconds in the day from gettimeofday
  uint32_t tusec;            // Microseconds in the day from gettimeofday
} EventHeader;

typedef struct {
  uint32_t semk;    // Beginning of subevent marker - 0xacabacab
  uint32_t sevhsiz;          // Subevent Header size in bytes
  uint32_t id;      // Module ID
  uint32_t size;    // Subevent size in bytes
} SubEventHeader;

/************************************************************************/
#endif
