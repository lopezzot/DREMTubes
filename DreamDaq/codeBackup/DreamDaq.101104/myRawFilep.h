#ifndef _MYRAWFILEPH_
#define _MYRAWFILEPH_

#include <stdio.h>
#include "myFIFO-IOp.h"

FILE *rawdatafp;
int beginningOfRun;

/* Run related variables */
RunHeader rh;

/* Event related variables */
EventHeader eh;
int stRel;  // Time of event (sec.) relative to time of first event
int ustRel; // Time of event (usec.) relative to time of first event
int toverp;

/* SubEvent related variables */
SubEventHeader seh;

/* Definitions */

#define IMAGIC           0xAABBCCDD
#define IMAGIC_SWAPPED   0xBBAADDCC
#define EVENTMARKER      0xCAFECAFE
#define SUBEVENTMARKER   0xACABACAB

/* Private Function declaration */

void abortOnReadError(void);
void abortOnBadMagicWord(void);
void abortOnBadEventMarker(int);
void abortOnBadSubEventMarker(int);

#endif
