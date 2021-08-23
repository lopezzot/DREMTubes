//$Id: myDump.c,v 1.2 2006/08/05 15:32:51 cvsdream Exp $
/*************************************************************************

        myDump.c
        --------

	Simple Program to dump myDaq-produced data

        Version 0.1,      A.Cardini 6/6/2001

*************************************************************************/

#include "myIOp.h"
#include "myIO.h"
#include "myModules.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

// Some definitions

#define IMAGIC         0xAABBCCDD
#define SMAGIC         0xDDCCBBAA
#define EVENTMARKER    0xCAFECAFE
#define SUBEVENTMARKER 0xACABACAB

/************************************************************************/
void printHelp()
/************************************************************************/
{
  printf("\nmyDump: usage is myDump [datafile]\n\n"); 
  return;
}

/************************************************************************/
void abortOnReadError()
/************************************************************************/
{
  printf("\nmyDump: error while reading from datafile - aborting\n\n"); 
  exit(-1);
}

/************************************************************************/
void abortOnBadMagicWord(unsigned int x)
/************************************************************************/
{
  printf("myDump: magic word found is %x, expected %x\n", x, IMAGIC);
  exit(-1);
}

/************************************************************************/
void abortOnBadEventMarker(unsigned int x)
/************************************************************************/
{
  printf("myDump: event marker found is %x, expected %x\n", x, EVENTMARKER);
  exit(-1);
}

/************************************************************************/
void abortOnBadSubEventMarker(unsigned int x)
/************************************************************************/
{
  printf("myDump: sub-event marker found is %x, expected %x\n", x, SUBEVENTMARKER);
  exit(-1);
}

/************************************************************************/
void dumpV767A(unsigned int *buf, int nw)
/************************************************************************/
{
  SubEventHeader seh;
  unsigned int ugo, data, ch, tm, st, ed, ge, sz, ev;
  int i;
  
  memcpy(&seh, buf, sizeof(SubEventHeader));
  

  for (i=seh.sevhsiz/4; i<seh.size/4; i++) {
    
    data = buf[i];
    ugo = (data)>>21&0x3;
    
    switch(ugo) {
    case 0:             /* Datum */
      ch = (data>>24)&0x7F;
      tm = data&0xFFFFF;
      st = (data>>23)&0x1;
      ed = (data>>20)&0x1;
      printf("<DATA> Channel %d Time %d Start %d Edge %d\n", ch, tm, st, ed);
      break;
    case 1:             /* Trailer */
      ge = (data>>27);
      st = (data>>24)&0x7;
      sz = data&0xFFFF;
      printf("<END> Geo %d Status %d Size %d\n", ge, st, sz);
      break;
    case 2:             /* Header */
      ge = data>>27;
      ev = data&0xFFF;
      printf("<BEG> Geo %d Event %d\n", ge, ev);
      break;
    default:             /* Not valid datum */
      printf("<INVALID> data %x\n", data);
      break;
    }      
  }
  return;
}


/****************************************/
void dumpV792AC(unsigned int *buf, int sz)
/****************************************/
{
  SubEventHeader seh;

  int i;
  int adc, un, ov, cha, geo, nch, cra, evn;
  unsigned int data;

  memcpy(&seh, buf, sizeof(SubEventHeader));


  for (i=seh.sevhsiz/4; i<seh.size/4; i++) {
    data = buf[i];
    switch((data>>24)&0x7) {
    case 0x0: // DATA
      adc = data&0xFFF;
      ov  = (data>>12)&0x1;
      un  = (data>>13)&0x1;
      cha = (data>>16)&0x3F;
      geo = (data>>27)&0x1F;
      printf("<DATA> Geo %d Channel %d Charge %d OverF %d UnderTh %d\n", geo, cha, adc, ov, un); 
      break;
    case 0x2: // HEADER
      nch = (data>>8)&0x3F;
      cra = (data>>16)&0xFF;
      geo = (data>>27)&0x1F;
      printf("<HEAD> Geo %d Crate %d Nchannels %d\n", geo, cra, nch); 
      break;
    case 0x4: // EOB
      evn = data&0xFFFF;
      geo = (data>>27)&0x1F;      
      printf("<EOB> Geo %d  Event %d\n", geo, evn); 
      break;
    default:  // INVALID WORD
      printf("<INVALID> data %x\n", data);
      break;
    }
  }   // fine loop sui dati
  return;
}

/************************************************************************/
void dumpV513(unsigned int *buf, int nw)
/************************************************************************/
{
  SubEventHeader seh;
  int i;
  
  memcpy(&seh, buf, sizeof(SubEventHeader));
  for (i=seh.sevhsiz/4; i<seh.size/4; i++) {
    printf("<<NOT DECODED> data %x\n", buf[i]);
  }
  return;
}

/************************************************************************/
void dumpV260(unsigned int *buf, int nw)
/************************************************************************/
{
  SubEventHeader seh;
  int i;
  
  memcpy(&seh, buf, sizeof(SubEventHeader));
  for (i=seh.sevhsiz/4; i<seh.size/4; i++) {
    printf("<DATA> Counts %d Inhibit %d\n", buf[i]&0xFFFFFF, buf[i]>31);
  }
  return;
}

/************************************************************************/
void dumpL1182(unsigned int *buf, int nw)
/************************************************************************/
{
  SubEventHeader seh;
  int i;
  
  memcpy(&seh, buf, sizeof(SubEventHeader));
  for (i=seh.sevhsiz/4; i<seh.size/4; i++) {
    printf("<DATA> Charge %d\n", buf[i]);
  }
  return;
}

/************************************************************************/
int main(int argc, char **argv)
/************************************************************************/
{
  FILE *dfp;
  unsigned int lbuf[10000];
  char bt[100];
  char et[100];
  int nevt, rc;
  int usbeg = 0;
  int first = 1;
  int sz, nw;
  unsigned int tdiff, sdiff, usdiff;

  RunHeader rh;
  EventHeader eh;
  SubEventHeader seh;

  // Check arguments to main - name of data file is expected
  if (argc != 2) { printHelp(); exit(-1); }
  
  // Open Data file
  dfp = fopen(argv[1], "r");
  if (!dfp) {
    printf("myDump: cannot open data file %s\n", argv[1]);
    exit(-1);
  }

  // Check for magic word
  rc = fread(lbuf, sizeof(unsigned int), 1, dfp);
  if (rc != 1) abortOnReadError();
  if (lbuf[0] != IMAGIC) abortOnBadMagicWord(lbuf[0]);
  fseek(dfp, 0, 0);             // go back at the beginning
  
  // Decode Run Header
  rc = fread(&rh, sizeof(RunHeader), 1, dfp);
  if (rc != 1) abortOnReadError();

  strftime(bt, 100, "%x at %X", localtime(&rh.begtim));
  strftime(et, 100, "%x at %X", localtime(&rh.endtim));
  printf("<RunHeader>: Run %d, %d events, started on %s, ended on %s\n", 
	 rh.runnumber, rh.evtsinrun, bt, et);
  
  // Loop on all events
  for (nevt=0; nevt<rh.evtsinrun; nevt++) {

    // Decode Event Header
    rc = fread(&eh, sizeof(EventHeader), 1, dfp);
    if (rc != 1) abortOnReadError();
    if (eh.evmark != EVENTMARKER) abortOnBadEventMarker(eh.evmark);
    if (first) { first = 0; usbeg = eh.tusec; }
    tdiff = (1000*eh.tsec+eh.tusec/1000)-(1000*rh.begtim+usbeg/1000);
    sdiff = tdiff/1000;
    usdiff = (tdiff%1000);
    printf("\n<EventHeader>: Event %07d, Time %07u.%03u s after start, Size %08d bytes\n", 
	   eh.evnum, sdiff, usdiff, eh.evsiz);

    // Read Event
    sz = (eh.evsiz-eh.evhsiz)/4;      // Event size without event header size
    rc = fread(lbuf, sizeof(unsigned int), sz, dfp);
    if (rc != sz) abortOnReadError();

    // Loop on subevents
    nw = 0;
    do {         
      memcpy(&seh, &lbuf[nw], sizeof(SubEventHeader));
      if (seh.semk != SUBEVENTMARKER) abortOnBadEventMarker(eh.evmark);
      printf("<SubEventHeader>: Module ID %08x, Size %08d bytes\n", seh.id, seh.size);

      // Subevent dump

      switch(seh.id&0xFF) {
      case ID_V767A:
	dumpV767A(&lbuf[nw], seh.size);
	break;
      case ID_L1182:
	dumpL1182(&lbuf[nw], seh.size);
	break;
      case ID_V260:
	dumpV260(&lbuf[nw], seh.size);
	break;
      case ID_V792AC:
	dumpV792AC(&lbuf[nw], seh.size);
	break;
      case ID_V513:
	dumpV513(&lbuf[nw], seh.size);
	break;
      default:
	printf("Unknown Module\n");
      }

      nw += seh.size/4;
    }  while (nw < sz);  // End of loop on all subevents
  } // End of loop on all events

  // Close Data File
  fclose(dfp);

  return 0;
}

