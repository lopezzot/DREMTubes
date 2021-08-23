
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <iostream>
#include <iomanip>

#include "myRawFile.h"

// #include "myV775.h"
#define v775_DecodeType(VAL)       (((VAL) >> 24) & 0x7     )
#define v775_DecodeValue(VAL)      ( (VAL)        & 0xFFF   )
#define v775_DecodeChannel(VAL)    (((VAL) >> 17) & 0xF     )
#define v775_DecodeValidity(VAL)   (((VAL) >> 14) & 0x1     ) // Bit 14

using namespace std;

FILE* rawdatafp;
int32_t beginningOfRun;

/* Run related variables */
RunHeader rh;

/* Event related variables */
EventHeader eh;
int32_t stRel;  // Time of event (sec.) relative to time of first event
int32_t ustRel; // Time of event (usec.) relative to time of first event
int32_t toverp;

/* SubEvent related variables */
SubEventHeader seh;

/********************************************/
void abortOnReadError()
     /********************************************/
{
  printf("abortOnReadError\n");
  exit(ABRTREAD);
}

/********************************************/
void abortOnBadMagicWord()
     /********************************************/
{
  printf("abortOnBadMagicWord\n");
  exit(ABRTMAGIC);
}

/********************************************/
void abortOnBadEventMarker(int32_t mark)
     /********************************************/
{
  printf("abortOnBadEventMarker: marker is 0x%x, expected 0x%x\n", mark, EVENTMARKER);
  exit(ABRTEVENT);
}

/********************************************/
void abortOnBadSubEventMarker(int32_t mark)
     /********************************************/
{
  printf("abortOnBadSubEventMarker: marker is 0x%x, expected 0x%x\n", mark, SUBEVENTMARKER);
  exit(ABRTSUBEVENT);
}

/********************************************/
int32_t RawFileOpen(const char* fname)
     /********************************************/
{
  int32_t rc = 0;

  printf("opening %s\n",fname);

  rawdatafp = fopen(fname, "r");
  if (!rawdatafp) {
    //fprintf(stderr, "RawFileOpen: cannot open file %s\n", fname);
    rc = -1;
  }
  beginningOfRun = 1;
  return rc;
}

/********************************************/
int32_t RawFileClose()
     /********************************************/
{
  return fclose(rawdatafp);
}

/********************************************/
uint32_t GetRunNumber() { return rh.runnumber; }
/********************************************/

/********************************************/
uint32_t GetTotEvts() { return rh.evtsinrun; }
/********************************************/

/********************************************/
uint32_t GetBegTime() { return rh.begtim; }
/********************************************/

/********************************************/
uint32_t GetEndTime() { return rh.endtim; }
/********************************************/

/********************************************/
uint32_t GetEventTimes() { return eh.tsec; }
/********************************************/

/********************************************/
uint32_t GetEventTimeu() { return eh.tusec; }
/********************************************/

/********************************************/
/********************************************/

/********************************************/
uint32_t GetEventNumber() { return eh.evnum; }
/********************************************/

/********************************************/
uint32_t GetSpillNumber() { return eh.spill; }
/********************************************/

/********************************************/
uint32_t GetEventHeaderSize() { return eh.evhsiz; }
/********************************************/

/********************************************/
uint32_t GetEventSize() { return eh.evsiz; }
/********************************************/

/********************************************/
uint32_t GetEventMarker() { return eh.evmark; }
/********************************************/

/********************************************/
uint32_t GetSubEventMarker() { return seh.semk; }
/********************************************/

/********************************************/
uint32_t GetSubEventHeaderSize() { return seh.sevhsiz; } 
/********************************************/

/********************************************/
uint32_t GetSubEventSize() { return seh.size; } 
/********************************************/

/********************************************/
uint32_t GetModuleId() { return seh.id; }
/********************************************/

/********************************************/
void UpdateEventTime()
     /********************************************/
{
  static int32_t first = 1;
  static int32_t sbeg, usbeg;
  int32_t tdiff;

  if (first) { first = 0; sbeg = eh.tsec; usbeg = eh.tusec; }
  tdiff = (1000000*eh.tsec+eh.tusec)-(1000000*sbeg+usbeg);
  stRel  = tdiff/1000000;
  ustRel = tdiff%1000000;
}

/********************************************/
int32_t RawFileReadEvent(uint32_t* buf)
     /********************************************/
{
  int32_t rc;
  int32_t sz;
  if (beginningOfRun) { // Only at the beginning of a new run

    // Read Magic Word if beginning of run
    uint32_t tmp;
    fflush(stdout);
    rc = fread(&tmp, sizeof(uint32_t), 1, rawdatafp);
    if (rc != 1) abortOnReadError();
    switch (tmp) {
    case IMAGIC:
      break;
    case IMAGIC_SWAPPED:
    default:
      abortOnBadMagicWord();
    }
    

    // Rewind and read Run Header if beginning of run
    rewind(rawdatafp);
    rc = fread(&rh, sizeof(RunHeader), 1, rawdatafp);
    if (rc != 1) abortOnReadError();
    // Run header found, avoid reading it again
    beginningOfRun = 0;

  }

  // Read Event Header
  rc = fread(&eh, sizeof(EventHeader), 1, rawdatafp);
  if (rc != 1) {
    if (feof(rawdatafp)) 
      return RAWDATAEOF;   // Correct EOF
    else 
      abortOnReadError();
  }
  if (GetEventMarker() != EVENTMARKER) abortOnBadEventMarker(GetEventMarker());

  // Update Event related variables
  UpdateEventTime();

  // Store event header
  memcpy (buf, &eh, sizeof(EventHeader));
  uint32_t eSize = GetEventSize()/4;
  uint32_t hSize = GetEventHeaderSize()/4;
  // Read Event
  sz = eSize - hSize;
  rc = fread(&buf[hSize], sizeof(uint32_t), sz, rawdatafp);
  if (rc != sz) {
    if (feof(rawdatafp))
      return RAWDATAUNEXPECTEDEOF;  // Truncated file?
    else 
      abortOnReadError();
  }

  return eSize;
}

/********************************************/
int32_t RawFileReadEventData(uint32_t* buf)
     /********************************************/
{
  int32_t rc;
  int32_t sz;
  if (beginningOfRun) { // Only at the beginning of a new run

    // Read Magic Word if beginning of run

    rc = fread(buf, sizeof(uint32_t), 1, rawdatafp);
    if (rc != 1) abortOnReadError();
    switch (buf[0]) {
    case IMAGIC:
      break;
    case IMAGIC_SWAPPED:
    default:
      abortOnBadMagicWord();
    }

    // Rewind and read Run Header if beginning of run
    rewind(rawdatafp);
    rc = fread(&rh, sizeof(RunHeader), 1, rawdatafp);
    if (rc != 1) abortOnReadError();

    // Run header found, avoid reading it again
    beginningOfRun = 0;
  }

  // Read Event Header
  rc = fread(&eh, sizeof(EventHeader), 1, rawdatafp);
  if (rc != 1) {
    if (feof(rawdatafp)) 
      return RAWDATAEOF;   // Correct EOF
    else 
      abortOnReadError();
  }
  if (GetEventMarker() != EVENTMARKER) abortOnBadEventMarker(GetEventMarker());

  // Update Event related variables
  UpdateEventTime();

  // Store event header
  uint32_t eSize = GetEventSize()/4;
  uint32_t hSize = GetEventHeaderSize()/4;
  // Read Event
  sz = eSize - hSize;
  rc = fread(buf, sizeof(uint32_t), sz, rawdatafp);
  if (rc != sz) {
    if (feof(rawdatafp))
      return RAWDATAUNEXPECTEDEOF;  // Truncated file?
    else 
      abortOnReadError();
  }

  return sz;
}

/********************************************/
uint32_t* SubEventSeek(uint32_t id, const uint32_t* lbuf)
     /********************************************/
{
  int32_t nw;
  int32_t sz;

  // Loop on subevents
  nw = 0;
  sz = (GetEventSize() - GetEventHeaderSize())/4;
  do {         
    memcpy(&seh, &lbuf[nw], sizeof(SubEventHeader));
    if (GetSubEventMarker() != SUBEVENTMARKER) abortOnBadSubEventMarker(GetSubEventMarker());
    // Look for correct module id
    if (GetModuleId() == id) return const_cast<uint32_t*>(&lbuf[nw]);
    nw += GetSubEventSize()/4;
  }  while (nw < sz);  // End of loop on all subevents
  
  return NULL;
}

/***************************************************/
uint32_t* SubEventSeek2(uint32_t id, uint32_t* lbuf,uint32_t sz)
     /********************************************/
{
  uint32_t nw;
  // Loop on subevents
  nw = 0;
  do {
    memcpy(&seh, &lbuf[nw], sizeof(SubEventHeader));
    //    printf("nw:%d lbufnw=%x\n",nw,lbuf[nw]); 
    if (GetSubEventMarker() != SUBEVENTMARKER) abortOnBadSubEventMarker(GetSubEventMarker());
    // Look for correct module id
    if (GetModuleId() == id) return &lbuf[nw];
    nw += GetSubEventSize()/4;
  }  while (nw < sz);  // End of loop on all subevents
                                                                                                      
  return NULL;
}

/******************************************/
int32_t DecodeTriggerNumber(const uint32_t* buf, myTRIGNUM* trignum)
{
 uint32_t i;
 uint32_t hits = 0;
 trignum->index =0 ;
 if (!buf) return NODATA;    // Protection against non-existing subevent
  memcpy(&seh, buf, sizeof(SubEventHeader));  // already done, but...
  uint32_t sz=(GetSubEventSize()-GetSubEventHeaderSize())/4;
  if (sz>MAXTRIGNUM) return BADDATATRIGNUM;
  i=GetSubEventHeaderSize()/4;
  while (i<GetSubEventSize()/4) {
     if (hits==0) {trignum->counter =buf[i];}
     if (hits==1) {trignum->triggnumb =buf[i];}
     hits++;
     i++;
     }   
    trignum->index = hits;
  return 0;

}
/********************************************/
int32_t DecodeV775 (const uint32_t* buf, myTDC* tdc)
     /********************************************/
{
  uint32_t i;
  uint32_t hits = 0;
  uint32_t sz=0;                                                                                                                        
  tdc->index = 0;
  if (!buf) return NODATA;    // Protection against non-existing subevent
  
  memcpy(&seh, buf, sizeof(SubEventHeader));  // already done, but...
  sz=(GetSubEventSize()-GetSubEventHeaderSize())/4;    
  if(sz>MAXTDC) return BADDATATDC;
  i=GetSubEventHeaderSize()/4;
  while(i<GetSubEventSize()/4)
  {
    if( v775_DecodeType(buf[i]) != 0 )
      cerr << "V775 problem: invalid data type!" << endl;
    tdc->data[hits]   = v775_DecodeValue   (buf[i]);
    tdc->channel[hits]= v775_DecodeChannel (buf[i]);
    tdc->valid[hits]  = v775_DecodeValidity(buf[i]);
    i++;
    hits++;
  }
  tdc->index = hits;
  return 0;

}


/********************************************/
int32_t DecodeV792AC(const uint32_t* buf, myADCN* adcn)
     /********************************************/
{
  uint32_t i;
  uint32_t hits = 0;
  adcn->index = 0;

  if (!buf) return NODATA;    // Protection against non-existing subevent
  memcpy(&seh, buf, sizeof(SubEventHeader));  // already done, but...
  uint32_t sz=(GetSubEventSize()-GetSubEventHeaderSize())/4;
  if (sz>MAXADCN+2) return BADDATAADCN;
  i=GetSubEventHeaderSize()/4;
  i++;
  while (i<((GetSubEventSize()/4)-1)) {
    adcn->data[hits]=(buf[i]&0xFFF);
    adcn->channel[hits]=(buf[i]>>16)&0x1F;
    adcn->ov[hits]=(buf[i]>>12)&0x1;
    adcn->un[hits]=(buf[i]>>13)&0x1;
    i++;
    hits++;
  } 

  adcn->index = hits; 
  return 0;                                                                                     
}


/********************************************/
int32_t DecodeV862(const uint32_t* buf, myADCN* adcn)
     /********************************************/
{
  return DecodeV792AC(buf, adcn);
}
