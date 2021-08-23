//
// Raw Data Format for Domino Readout
//
// Author: Florian Goebel 
// Modified: Diego Tescaro
// Date:  May 2008// modified: 7.11.2006
// modified: 17.06.2007 (FORMAT_VERSION 9)
// modified: 19.05.2008 (FORMAT_VERSION 10)
// modified: 19.05.2008 (FORMAT_VERSION 11)

#ifndef MAGIC_RAW_DATA_H_SEEN
#define MAGIC_RAW_DATA_H_SEEN

#include "MagicTypes.h"

// diego New magic numbers have to be defined for the Dominos
#define MAGIC_NUMBER_UNFINISHED 0x000BC0C1 // new MAGIC number for MuxFadc data
#define MAGIC_NUMBER_FINISHED   0x000BC0C0 // new MAGIC number for MuxFadc data
//#define MAGIC_NUMBER_CRATE_UNFINISHED 0xC0E1 // new MAGIC number for MuxFadc crate data
//#define MAGIC_NUMBER_CRATE_FINISHED   0xC0E0 // new MAGIC number for MuxFadc crate data

//diego Also this number have to be changed at a certain point
#define FORMAT_VERSION 11 // new number
#define SOFT_VERSION 1  // increase SOFT_VERSION by 1 for each (relevant) change in the DAQ program 
#define SIEGEN_TYPE 1
#define MUX_TYPE 2
#define DOMINO_TYPE 3      // number for domino FADC
#define CAMERA_VERSION 1   // 1: original MAGIC I camera design
//#define TELESCOPE_NUMBER 1 // 1: MAGIC I telescope
#define TELESCOPE_NUMBER 2 // 2: MAGIC II telescope

#define NUM_BYTES_PER_SAMPLE 2    // for Mux FADC: 2 Bytes per sample // the same for the DOMINO
#define SAMPLING_FREQUENCY   2000 // for Mux FADC: 2000 MSamples/s // the same for the DOMINO
//#define FADC_RESOLUTION      10   // for Mux FADC: 10 bit resolution
#define FADC_RESOLUTION      12   // for DOMINO FADC: 12 bit resolution

#define RUN_TYPE_NORMAL      0
#define RUN_TYPE_PEDESTAL    1
#define RUN_TYPE_CALIBRATION 2
#define RUN_TYPE_POINT       3
#define RUN_TYPE_DOMINOCALIB 4 //diego added

#define SIZE_PROJECT_NAME     100 // larger in latest version (Format>=6) of MAGIC1 
#define SIZE_SOURCE_NAME      80  // larger in latest version (Format>=6) of MAGIC1 
#define SIZE_OBSERVATION_MODE 60
//#define SIZE_SOURCE_EPOCH      2
#define SIZE_SOURCE_EPOCH      4 // everything in multiples of 4 since format 11

// diego This was my format 
/*
//#define RUN_HEADER_BASESIZE 309 // excluding PixAssignment
#define RUN_HEADER_BASESIZE 313 // excluding PixAssignment // diego
#define EVENT_HEADER_SIZE    30 // real size of struct: as sum of sizeof(single datatypes)
// diego CHANGE in the DATA FORMAT
//#define CHANNEL_HEADER_SIZE  17 // real size of struct: as sum of sizeof(single datatypes)
#define CHANNEL_HEADER_SIZE  19 // real size of struct: as sum of sizeof(single datatypes)
// diego: what is this?
#define NUMEVENTS_OFFSET    286
#define ENDYEAR_OFFSET      302
*/

// diego START taken from the new MuxReadout
#if (FORMAT_VERSION >= 11)

#define RUN_HEADER_BASESIZE 400  // excluding PixAssignment
#define EVENT_HEADER_SIZE    32
// diego #define CHANNEL_HEADER_SIZE  28
#define CHANNEL_HEADER_SIZE  32 // diego
#define NUMEVENTS_OFFSET    332
#define ENDYEAR_OFFSET      376

#elif (FORMAT_VERSION >= 10)

#define RUN_HEADER_BASESIZE 321 // excluding PixAssignment
#define EVENT_HEADER_SIZE    30 // real size of struct: as sum of sizeof(single datatypes)
#define CHANNEL_HEADER_SIZE  17 // real size of struct: as sum of sizeof(single datatypes)
#define NUMEVENTS_OFFSET    294
#define ENDYEAR_OFFSET      314

#elif (FORMAT_VERSION >= 9)

#define RUN_HEADER_BASESIZE 317 // excluding PixAssignment
#define EVENT_HEADER_SIZE    30 // real size of struct: as sum of sizeof(single datatypes)
#define CHANNEL_HEADER_SIZE  17 // real size of struct: as sum of sizeof(single datatypes)
#define NUMEVENTS_OFFSET    290
#define ENDYEAR_OFFSET      310

#else

#define RUN_HEADER_BASESIZE 309 // excluding PixAssignment
#define EVENT_HEADER_SIZE    30 // real size of struct: as sum of sizeof(single datatypes)
#define CHANNEL_HEADER_SIZE  17 // real size of struct: as sum of sizeof(single datatypes)
#define NUMEVENTS_OFFSET    286
#define ENDYEAR_OFFSET      302

#endif
// diego END taken from the new MuxReadout

#define NMAXBOARDS 16
#define NMAXCHANNELSINBOARD 8
#define NMAXCHANNELS (NMAXCHANNELSINBOARD*NMAXBOARDS)


typedef struct runheaderstr {
  U32       MagicNumber;                                                       //
  U32       FormatVersion;                                                     // 2

  U32       RunHeaderSize; // new
  U32       EventHeaderSize; // new
  U32       ChannelHeaderSize; //new

  U32       SoftVersion;                                                       // 4
  U32       FadcType;                                                          // 6

  U32       CameraVersion;                                                     // 8
  U32       TelescopeNumber;                                                   // 10

  U32       RunType;                                                           // 12
  U32       RunNumber;                                                         // 14
  U32       FileNumber;                                                        // new! not counted here

  char      ProjectName[SIZE_PROJECT_NAME];                                    // 20
  char      SourceName[SIZE_SOURCE_NAME];                                      // 118
  char      ObservationMode[SIZE_OBSERVATION_MODE];                            // 198

  F32       SourceRA;                                                          // 258
  F32       SourceDEC;                                                         // 262 
  F32       TelescopeRA;                                                       // 266 
  F32       TelescopeDEC;                                                      // 270

  char      SourceEpochChar[SIZE_SOURCE_EPOCH];                                // 274
  U32       SourceEpochDate;                                                   // 276

// Specific of the readout system
  U32       NumChannels;        // nominal: 13*8 = 104                         // 278
  U32       NumPixInChannel;    // nominal: 10                                 // 280  
  //  U32       Dummy;              // for compatibility with Siegen FADCs (NumLoGainSamples) // 282
  U32       NumSamplesPerPixel; // nominal: to be fix but probably 80          // 284
                                // 1024 if a domino-calibration run

  // if switching noise is removed, NumSamplesPerPixel is the number of stored pixels 
  // after switching noise removal.
  // The total number of samples before removal is:
  // NumSamplesPerPixel + NumSamplesRemovedHead + NumSamplesRemovedTail
  U32       NumSamplesRemovedHead; // keeped for compatibility with MuxReadout
  U32       NumSamplesRemovedTail; // keeped for compatibility with MuxReadout
  // no switching noise is removed in the Dominos for the moment

  U32       NumEvents;  // Number of events stored in this rawdata file

  U32       NumEventsRead;   // added in format_version 9
  // This is the number of events read from the front end electronics 
  //  including corrupt events which have been remove and are not contained 
  //  in this rawdata file
  // Specific of the MuxReadout! keeped for compatibility

// New in general features: (should they be included in new MAGIC1 formats, too?)
  U32       NumBytesPerSample;  // nominal: 2                                  // 290
  U32       SamplingFrequency;  // [MHz], 2000 for the Dominos                 // 292
  U32       FadcResolution;     // nominal resolution [# Bits], 12 for Dominos // 294

  U32       StartYear;                                                         // 295
  U32       StartMonth;                                                        // 297
  U32       StartDay;                                                          // 298
  U32       StartHour;                                                         // 299
  U32       StartMinute;                                                       // 300
  U32       StartSecond;                                                       // 301
  U32       EndYear;                                                           // 302
  U32       EndMonth;                                                          // 304
  U32       EndDay;                                                            // 305
  U32       EndHour;                                                           // 306
  U32       EndMinute;                                                         // 307
  U32       EndSecond;                                                         // 308

  U32       PixAssignment[1];  // need to allocate explicitly the required     // 309
  //   memory space:  [xx] = NumChannels * NumPixInChannel
  // comment: non connected pixels which are nevertheless readout
  //          (e.g. the last channel will not the full 16 pixels)
  //          get the pixel ID=0
  // diego Why [1]?

} run_header_str;


// The EventHeader is almost identical to MAGIC1 format
// The information are read in by a dedicated Digital Module like in MAGIC1
//
typedef struct eventheaderstr {

  U32       EvtNumber;        //4
  U32       TimeSec;          //8
  U32       TimeSubSec;       //12
  U32       FstLvlTrigNumber; //16 // never used in MAGIC1. Should be skipped?
  U32       SecLvlTrigNumber; //20
  U32       TrigPattern;      //24
  U32       CalPattern;       //28
  U32       TriggerType;      //30

} event_header_str;


// Hardware hierarchy:
//    4 FILAR boards containing each:
//    4 S_Link channels (total 16 channels == 16 PULSAR boards), Each with:
//    8 FADC channels i.e. 8 DRS (total 124 channels), each digitizing maximum 
//    10 multiplexed Pixels (total 1240 pixel-channels maximum)
//
// Data Format:
//    Keep only FADC channel level.
//    For each channel store:
//    PULSAR#, DRS# and pixel-channel number
//    ClockTicks & TriggerTimeInterpolation // diego: what is this?
//    FadcDataStream
typedef struct channelheaderstr {

  //  U8  CrateNumber;    // 0-4
  U32  DummyCrateNumber;  // leave this here form compatibility?
  U32  BoardNumber;    // 0-1
  U32  ChannelNumber;  // 0-3

  //diego  U32  CrateEvtNumber;
  U32  ChannelEvtNumber; // there are no Crates, exist just one event number
//  In the MUX readout the CrateEvtNumber will be produced by each crate
//  The central event builder will make sure that the CrateEvtNumber
//  agrees for all crates. This number will then be stored as the
//  EvtNumber in the EventHeader, 
//  NOW:
//  This ChannelEvtNumber is the EvtNumber 

// Clock count. The Clock is synchronized with the 10 MHz external clock,
//  which is feed to for all FADC channels.
// The units are [psec], which is obviously much smaller than the real accuracy.
// The ClockTick should be identical for all channels of the same board.
//  Still keep it for debugging purposes.
// Like in the 300MHz system this number is extremely useful to check the
//  integrity of the data.
//  U32 FadcClockTickHi;  // high significant bits
//  U32 FadcClockTickLo;  // low significant bits
// diego
// With the Dominos each mezzanine (of the 4 in a board) has a different
// clock and trigger number (both U32). The names are changed accordingly
  U32 FadcClockTick;  // the clock counter
  U32 FadcTrgNumber;  // the trigger counter

// Trigger Time Interpolation in [psec] (originally it is a double in Acqiris software)
// Again this number should be identical for all channels in the same board
// It is not clear at the moment if this number will be useful in the end,
//  but I propose to keep it. The data volume is increase by <0.1%
  U32 TrigTimeInterpol;
  // diego not used but keeped for consistency. Should we remove it?

  //diego CHANGE in the DATA FORMAT, this will affect a lot of things!
  // first domino cell actually used for the digitization in the specified channel
  // in principle this is the same number for each board but i'm not sure is the 
  // same for all boards
  U32 FirstCapacitor;

} channel_header_str;

/* diego no more needed right?
// structure passed to the linearization threads to synchronously
// linearize the samples of the same event
typedef struct {
  U16 *samplesptr0;
  U16 *samplesptr1;
  //  U16 *samplesptr2;
  //  U16 *samplesptr3;
  u_int cap_DRS0;
  u_int cap_DRS1;
  //  u_int cap_DRS2;
  //  u_int cap_DRS3;
  float *coef0;
  float *coef1;
  float *coef2;
  float *coef3;
  // the mutex and condition variables to synchronize the linearization
  pthread_mutex_t *mutex_counter; 
  pthread_cond_t *cond_start;
  pthread_cond_t *cond_finish;
  // the flag counter
  int *counter;

} samples2linearize;
*/

#ifdef __cplusplus
extern "C" {
#endif

  extern int RunFilename    (run_header_str *RunHeader, char* filename, char* path);
  extern int fWriteRunHeader(run_header_str *RunHeader, FILE *fptr);
  extern int gzWriteRunHeader(run_header_str *RunHeader, FILE *fptr);
  extern run_header_str* fReadRunHeader(FILE *fptr);
  extern int PrintRunHeader (run_header_str *RunHeader, FILE *fptr);
  extern int fReadEventHeader (event_header_str *EventHeader, FILE *fptr);
  extern int fWriteEventHeader (event_header_str *EventHeader, FILE *fptr);
  extern int PrintEventHeader (event_header_str *EventHeader, FILE *fptr);
  extern int fReadChannelHeader (channel_header_str *ChannelHeader, FILE *fptr);
  extern int PrintChannelHeader (channel_header_str *ChannelHeader, FILE *fptr);

  extern int decode_sec(U32 pattern_sec, int *h, int *m, int *s);
  extern int decode_subsec(U32 pattern_subsec);

#ifdef __cplusplus
}
#endif


#endif





