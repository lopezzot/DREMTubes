//$Id: myIO.c,v 1.4 2006/08/11 18:08:15 cvsdream Exp $
/*************************************************************************

        myIO.c
        ------

        This file contains all related I/O stuff.

        Version 0.1,      A.Cardini 3/6/2001
        Version 0.2,      D.Raspino 27/7/2005 Data From Slow Control

*************************************************************************/
#include "myIOp.h"
#include "myIO.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

// Some definitions

#define IMAGIC 0xAABBCCDD
#define SMAGIC 0xDDCCBBAA

#define RUNNUMBERFILENAME "run.number"

// Global variables

int myRunNumber = 0;
int myEventNumber = 0;
char myRunNumberFile[128];
FILE *dfp;
RunHeader   myRH;
EventHeader myEH;
char datafile[128];
unsigned int mybuff[10000];    // This is the buffer to temporary store all data
int buffsize;                 // Size of how much memory is used

/************************************************************************/
int myMaxEvents()
/************************************************************************/
{
  // Return max events according to 
  // environnment variable MAXEVT
  
  int maxevt;

  if (getenv("MAXEVT") == NULL) 
    maxevt = 10000;                     // This is the default
  else {
    maxevt = atol(getenv("MAXEVT"));
  }
  return maxevt;
}

/************************************************************************/
int myDownscaleFactor()
/************************************************************************/
{
  // Return downscale factor according to
  // environnment variable DWNSCALE
  
  int dwnscale;

  if (getenv("DWNSCALE") == NULL) 
    dwnscale = 1;                     // Default is no downscale...
  else {
    dwnscale = atol(getenv("DWNSCALE"));
  }
  return dwnscale;
}

/************************************************************************/
int myReloadTDCConfig()
/************************************************************************/
{
  // Return reload TDC Configuration factor according to
  // environnment variable RELOAD_TDC_CONFIG
  
  int rtc;
  
  if (getenv("RELOAD_TDC_CONF") == NULL) 
    rtc = 1;                     // Default is reload
  else {
    rtc = atol(getenv("RELOAD_TDC_CONF"));
  }
  return rtc;
}

/************************************************************************/
int myOpenRun()
/************************************************************************/
{
  FILE *ifp;
  int tmp, n;
  char * myDataDirectory;
  
  // First find current run number...
  if (getenv("WORKDIR") == NULL) 
    myDataDirectory = getenv("PWD");
  else
    myDataDirectory = getenv("WORKDIR");

  sprintf(myRunNumberFile, "%s/%s", myDataDirectory, RUNNUMBERFILENAME);

  ifp = fopen(myRunNumberFile, "r+");
  if (!ifp) {
    printf("myOpenRun: cannot open file %s\n", myRunNumberFile);
    exit(-1);
  }
  n = fscanf(ifp, "%10d\n", &tmp);  // read the only thing which is written...
  if (n != 1) {
    printf("myOpenRun: wrong format of file %s\n", myRunNumberFile);
    exit(-1);
  }
  fseek(ifp, 0, 0);             // go back at the beginning...
  tmp++;
  myRunNumber = tmp;
  fprintf(ifp, "%10d\n", tmp);  // ...and write back current run number
  fclose(ifp);
  
  if (getenv("WORKDIR") == NULL) 
    myDataDirectory = getenv("PWD");
  else
    myDataDirectory = getenv("WORKDIR");

  sprintf(datafile, "%s/data/run_%d.raw", myDataDirectory, myRunNumber);
  printf("Now opening file %s\n", datafile);
  dfp = fopen(datafile, "w");
  if (!dfp) {
    printf("myOpenRun: cannot open data file %s\n", datafile);
    exit(-1);
  }
  return 0;
}

/************************************************************************/
int myCloseRun()
/************************************************************************/
{
  myUpdateRunEventNumber();              // Write few info in run header...
  printf("Now closing file %s\n", datafile);
  fclose(dfp);
  return 0;
}

/************************************************************************/
int myUpdateRunEventNumber()
/************************************************************************/
{
  fseek(dfp, 0, 0);                  // go back to beginning...
  myRH.evtsinrun = myEventNumber;    // ...and add the following values to event header
  myRH.endtim = time(NULL);
  fwrite(&myRH, sizeof(RunHeader), 1, dfp);  // ...and rewrite it...
  return 0;
}

/************************************************************************/
int myWriteEvent()
/************************************************************************/
{
  struct timeval tv;
  struct timezone tz;
  static int isfirst = 1;
  
  // Event time with microsecond resolution...

  gettimeofday(&tv, &tz);
  
  // Write Run Header the first time only
  
  if (isfirst != 0) {
    myRH.magic = IMAGIC;                          // Magic word
    myRH.ruhsiz = sizeof(RunHeader);
    myRH.runnumber = myRunNumber;                 // Current run number
    myRH.evtsinrun = -1;                          // Not defined, yet...
    myRH.begtim = time(NULL);                     // Start time
    myRH.endtim = -1;                             // Not defined, yet...
    fwrite(&myRH, sizeof(RunHeader), 1, dfp);
    isfirst = 0;
  }

  // Write Event Header and data
  
  myEH.evmark   =  0xCAFECAFE;
  myEH.evhsiz   =  sizeof(EventHeader);
  myEH.evsiz    =  sizeof(EventHeader) + buffsize * sizeof(unsigned int);
  myEH.evnum    =  ++myEventNumber;
  myEH.tsec     =  tv.tv_sec;                       // Event time in seconds
  myEH.tusec    =  tv.tv_usec;                      // ...and in microseconds
  
  fwrite(&myEH, sizeof(EventHeader), 1, dfp);
  fwrite(mybuff, sizeof(unsigned int), buffsize, dfp);    // Write data
  return 0;
}

/************************************************************************/
int myNewEvent()
/************************************************************************/
{
  buffsize = 0;         // Reset total event length
  return 0;
}

/************************************************************************/
int myFormatSubEvent(unsigned int *src, int size, unsigned int moduleId)
/************************************************************************/
{
  SubEventHeader mySEH;

  mySEH.semk = 0XACABACAB;
  mySEH.sevhsiz = sizeof(SubEventHeader);
  mySEH.id   = moduleId;
  mySEH.size = sizeof(SubEventHeader) + size*sizeof(unsigned int);

  memcpy(&mybuff[buffsize], &mySEH, sizeof(SubEventHeader));
  buffsize += sizeof(SubEventHeader)/sizeof(unsigned int);

  memcpy(&mybuff[buffsize], src, size*sizeof(unsigned int));
  buffsize += size;

  return 0;
}

