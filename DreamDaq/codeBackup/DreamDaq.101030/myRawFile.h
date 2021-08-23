#ifndef _MYRAWFILEH_
#define _MYRAWFILEH_


#include "myFIFO-IOp.h"

#define MAXTDC   64
#define MAXKTDC  64
#define MAXADC  120  
#define MAXFADC 128
#define MAXSCA   16
#define MAXTH03   3 
#define MAXTEKOSC 2128
#define MAXADCN 32 
#define MAXTRIGNUM 2
/* Definitions */

#define IMAGIC           0xAABBCCDD
#define IMAGIC_SWAPPED   0xBBAADDCC
#define EVENTMARKER      0xCAFECAFE
#define SUBEVENTMARKER   0xACABACAB


#define MARKER_V775      0x20000024

extern "C"{
}

typedef struct {
  unsigned  int index; 
  unsigned int counter;
  unsigned int triggnumb;
}myTRIGNUM;

typedef struct {
  unsigned  int index;               // The number of the measurement for TH03 (0,1 or 3)                   
  float data[MAXTH03];               // The value of the measurement  
                                     // ( data[i] is the value of the  ith-measurement )    
  unsigned  int channel[MAXTH03];    // The channel of the measurement
                                     // (channel[i] is the channel of the ith-measurement) 
  unsigned  int rawdata[MAXTH03];
  unsigned  int highref;
  unsigned  int lowref;
} myTH03;


typedef struct {
  unsigned int index;               // The number of the poits for Oscilloscope (from 0 to MAXTEKOSC)
  int data[MAXTEKOSC];              //  Values of the poits in the oscilloscope
                                    // (data[i] is the value of the i-th point)
  unsigned int channel[MAXTEKOSC];  //Channels in the oscilloscope
                                    // CHOSC[i] is the channel of the i-th point ) 
  unsigned int num[MAXTEKOSC];      //Number of the point in the channel (max value =282)
  unsigned int tdiff[4];            //  Time Diff for Oscilloscope (as in myOscDataFile.h)
  int  position[4];                 // Position for Oscilloscope (as in myOscDataFile.h)
  unsigned int  scale[4];           // Scale of the Oscilloscope (as in myOscDataFile.h)  
  unsigned int  samplerate;         //Sampling rate of the Oscilloscope (as in myOscDataFile.h) 
  unsigned int pts;                 //Oscilloscope pts (as in myOscDataFile.h , at the moment =282)
//  unsigned int channelmap;        // Unused (alternative to chfla)
  unsigned int chfla[4];            //Oscilloscope Channel Flag (0= channel enabled, 1 =channel abled)   
} myTEKOSC;

                                                                                                   
typedef struct {
  unsigned int index;               // The number of the count in ADCN (from 0 to MAXADCN)
  unsigned int data[MAXADCN];       //Value of the cout in ADCN
                                    // (data[i] is the  value of the i-th cout )
  unsigned int channel[MAXADCN];    // Channel of the count in ADCN
                                    // (channel[i] is the channel of the i-th count )
  unsigned int ov[MAXADCN];         //  1 if overflow cout 
  unsigned int un[MAXADCN];         //  1 if underflow cout
} myADCN;
                                                                                                 
                                                                                          

typedef struct {
  unsigned int index;              //The number of the cout in TDC (from 0 to MAXTDC)
  unsigned int data[MAXTDC];       //Value of the cout in the TDC
                                   // (data[i] is the  value of the i-th cout ) 
  unsigned int channel[MAXTDC];    // Channel of the count in the TDC 
                                   // (channel[i] is the channel of the i-th count )
  unsigned int valid[MAXTDC];      // 1 = Good, (in 2009 it was: 0 Good hit, 1 Corrupted)
  unsigned int edge[MAXTDC];       // Edge in TDC (0=falling 1=rising)
} myTDC;

typedef struct {
  int index;
  int data[MAXKTDC];
  int channel[MAXKTDC];
  int over[MAXKTDC];
  int edge[MAXKTDC];
} myKTDC;


typedef struct {
  unsigned int index;              // The number of the count in ADC (from 0 to MAXADC)
  unsigned int charge[MAXADC];     //    Value of the cout in ADC
                                   // (data[i] is the  value of the i-th cout )
  unsigned int channel[MAXADC];    // Channel of the count in ADC
                                   // (channel[i] is the channel of the i-th count )
  unsigned int ovfl[MAXADC];       //  1 if overflow cout 
  unsigned int unth[MAXADC];       //  1 if underflow cout
} myADC;

typedef struct {
  unsigned int index;
  int value[MAXFADC];
  unsigned int channel[MAXFADC];
  unsigned int num[MAXFADC]; 
} myFADC;


typedef struct {
  unsigned int index;               // The number of the count in ADC (from 0 to MAXSCA)    
  unsigned int counts[MAXSCA];      //    Value of the cout in SCALER 
  unsigned int channel[MAXSCA];     //  Channel of the count in SCALER 
                                   // (channel[i] is the channel of the i-th count )

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
#define     BADDATATEKOSC         -216
#define     BADDATAADCN           -217  
#define     MANYEVENTS            -22
#define    BADDATATRIGNUM         -23
/* Function declaration */
                                                                                                       
extern "C"{               
  int RawFileReadEventData(unsigned int *buf);
  int RawFileRunNumber();
  int RawFileEventNumber();
  int RawFileOpen(const char *);
  int RawFileClose(void);
  unsigned int GetRunNumber(void);
  unsigned int GetTotEvts(void);
  unsigned int GetBegTime(void);
  unsigned int GetEndTime(void);
  unsigned int GetEventNumber(void);
  unsigned int GetSpillNumber(void);
  unsigned int GetEventTimes(void);
  unsigned int GetEventTimeu(void);
  unsigned int GetEventHeaderSize(void);
  unsigned int GetEventSize(void);
  unsigned int GetEventMarker(void);
  unsigned int GetSubEventMarker(void);
  unsigned int GetSubEventHeaderSize(void);
  unsigned int GetSubEventSize(void);
  unsigned int GetModuleId(void);
  void UpdateEventTime(void);
  int RawFileReadEvent(unsigned int *);
  unsigned int* SubEventSeek(unsigned int, const unsigned int *);
  unsigned int* SubEventSeek2(unsigned int, unsigned int *, unsigned int);
  int DecodeL1182 (const unsigned int *, myADC  *);
  int DecodeV260(const unsigned int *, mySCA  *);
  int DecodeTriggerNumber(const unsigned int *, myTRIGNUM  *);
  int DecodeSIS3320(const unsigned int *, myFADC  *);
  int DecodeL1176(const unsigned int *, myTDC  *);
  int DecodeKLOETDC(const unsigned int *, myKTDC  *);  
  int DecodeTH03(const unsigned int *, myTH03  *); 
  int DecodeTEKOSC(const unsigned int *, myTEKOSC *);
  int DecodeV792AC(const unsigned int *, myADCN *);
  int DecodeV862(const unsigned int *, myADCN *);
  int DecodeV775 (const unsigned int *buf, myTDC *tdc);
  void abortOnReadError(void);
  void abortOnBadMagicWord(void);
  void abortOnBadEventMarker(int);
  void abortOnBadSubEventMarker(int);
}
#endif
