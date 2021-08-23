//$Id: myFIFO-IOp.h,v 1.2 2007/06/01 17:13:14 cvsdream Exp $
#ifndef _MYIOP_
#define _MYIOP_
/*************************************************************************

        myIOp.h
        -------

        This file contains event structures definitions

        Version 0.1,      A.Cardini 6/6/2001
        Version 0.2,      D.Raspino 27/7/2005 Add data from SLow Control

*************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>


#define EVMARK 0xCAFECAFE
#define SUBEVMARK 0xACABACAB

#define EVOFFTOSIZE 2
#define EVOFFTONUM 3

#define SUBEVOFFTOID 2
#define SUBEVOFFTOSIZE 3

// Data Structures Definitions

typedef struct {
  unsigned int magic;            // Magic word - 0xaabbccdd
  unsigned int ruhsiz;           // Run Header size in bytes
  unsigned int runnumber;        // Run number
  unsigned int evtsinrun;        // Events in this run (if available)
  unsigned int begtim;        // Time at beginning of run
  unsigned int endtim;        // Time at end of run
} RunHeader;

typedef struct {
  unsigned int evmark;           // Beginning of event marker - 0xcafecafe
  unsigned int evhsiz;           // Event Header size in bytes
  unsigned int evsiz;            // Event size in bytes (including header)
  unsigned int evnum;            // Event number
  unsigned int spill;            // Spill number
  unsigned int tsec;             // Seconds in the day from gettimeofday
  unsigned int tusec;            // Microseconds in the day from gettimeofday
} EventHeader;

typedef struct {
  unsigned int semk;    // Beginning of subevent marker - 0xacabacab
  unsigned int sevhsiz;          // Subevent Header size in bytes
  unsigned int id;      // Module ID
  unsigned int size;    // Subevent size in bytes
} SubEventHeader;

/************************************************************************/
#endif












