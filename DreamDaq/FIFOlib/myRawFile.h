#ifndef _MYRAWFILEH_
#define _MYRAWFILEH_

#include <stdint.h>

#include "myFIFO-IOp.h"

#define MAXTDC   64
#define MAXKTDC  64
#define MAXADC  120  
#define MAXFADC 128
#define MAXSCA   16
#define MAXADCN 32 
#define MAXTRIGNUM 2
/* Definitions */

#define IMAGIC           0xAABBCCDD
#define IMAGIC_SWAPPED   0xBBAADDCC
#define EVENTMARKER      0xCAFECAFE
#define SUBEVENTMARKER   0xACABACAB

#define MARKER_V775      0x20000024

typedef struct {
  uint32_t index; 
  uint32_t counter;
  uint32_t triggnumb;
}myTRIGNUM;

typedef struct {
  uint32_t index;               // The number of the count in ADCN (from 0 to MAXADCN)
  uint32_t data[MAXADCN];       //Value of the cout in ADCN
                                    // (data[i] is the  value of the i-th cout )
  uint32_t channel[MAXADCN];    // Channel of the count in ADCN
                                    // (channel[i] is the channel of the i-th count )
  uint32_t ov[MAXADCN];         //  1 if overflow cout 
  uint32_t un[MAXADCN];         //  1 if underflow cout
} myADCN;

typedef struct {
  uint32_t index;              //The number of the cout in TDC (from 0 to MAXTDC)
  uint32_t data[MAXTDC];       //Value of the cout in the TDC
                                   // (data[i] is the  value of the i-th cout ) 
  uint32_t channel[MAXTDC];    // Channel of the count in the TDC 
                                   // (channel[i] is the channel of the i-th count )
  uint32_t valid[MAXTDC];      // 1 = Good, (in 2009 it was: 0 Good hit, 1 Corrupted)
  uint32_t edge[MAXTDC];       // Edge in TDC (0=falling 1=rising)
} myTDC;

typedef struct {
  int32_t index;
  int32_t data[MAXKTDC];
  int32_t channel[MAXKTDC];
	  int32_t over[MAXKTDC];
  int32_t edge[MAXKTDC];
} myKTDC;

typedef struct {
  uint32_t index;              // The number of the count in ADC (from 0 to MAXADC)
  uint32_t charge[MAXADC];     //    Value of the cout in ADC
                                   // (data[i] is the  value of the i-th cout )
  uint32_t channel[MAXADC];    // Channel of the count in ADC
                                   // (channel[i] is the channel of the i-th count )
  uint32_t ovfl[MAXADC];       //  1 if overflow cout 
  uint32_t unth[MAXADC];       //  1 if underflow cout
} myADC;

typedef struct {
  uint32_t index;
  int32_t value[MAXFADC];
  uint32_t channel[MAXFADC];
  uint32_t num[MAXFADC]; 
} myFADC;

typedef struct {
  uint32_t index;               // The number of the count in ADC (from 0 to MAXSCA)    
  uint32_t counts[MAXSCA];      //    Value of the cout in SCALER 
  uint32_t channel[MAXSCA];     //  Channel of the count in SCALER 
} mySCA;

/* Error codes */

#define     RAWDATAEOF             -7
#define     RAWDATAUNEXPECTEDEOF   -8
#define     ABRTREAD              -10
#define     ABRTMAGIC             -11
#define     ABRTEVENT             -12
#define     ABRTSUBEVENT          -13

#define     NODATA                -20
#define     BADDATA               -21
#define     BADDATAADC            -210
#define     BADDATAFADC           -211
#define     BADDATATDC            -212
#define     BADDATAKTDC           -213
#define     BADDATASCALER         -214 
#define     BADDATATH03           -215
#define     BADDATAADCN           -217  
#define     MANYEVENTS            -22
#define     BADDATATRIGNUM         -23
/* Function declaration */
                                                                                                       
extern "C"{               
  int32_t RawFileReadEventData(uint32_t* buf);
  int32_t RawFileEventNumber(void);
  int32_t RawFileOpen(const char*);
  int32_t RawFileClose(void);
  uint32_t GetRunNumber(void);
  uint32_t GetTotEvts(void);
  uint32_t GetBegTime(void);
  uint32_t GetEndTime(void);
  uint32_t GetEventNumber(void);
  uint32_t GetSpillNumber(void);
  uint32_t GetEventTimes(void);
  uint32_t GetEventTimeu(void);
  uint32_t GetEventHeaderSize(void);
  uint32_t GetEventSize(void);
  uint32_t GetEventMarker(void);
  uint32_t GetSubEventMarker(void);
  uint32_t GetSubEventHeaderSize(void);
  uint32_t GetSubEventSize(void);
  uint32_t GetModuleId(void);
  void UpdateEventTime(void);
  int32_t RawFileReadEvent(uint32_t*);
  uint32_t* SubEventSeek(uint32_t, const uint32_t*);
  uint32_t* SubEventSeek2(uint32_t, uint32_t*, uint32_t);
  int32_t DecodeTriggerNumber(const uint32_t*, myTRIGNUM*);
  int32_t DecodeV792AC(const uint32_t*, myADCN*);
  int32_t DecodeV862(const uint32_t*, myADCN*);
  int32_t DecodeV775 (const uint32_t* buf, myTDC* tdc);
  void abortOnReadError(void);
  void abortOnBadMagicWord(void);
  void abortOnBadEventMarker(int32_t);
  void abortOnBadSubEventMarker(int32_t);
}
#endif
