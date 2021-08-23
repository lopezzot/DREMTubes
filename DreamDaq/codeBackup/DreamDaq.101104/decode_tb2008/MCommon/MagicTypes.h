#ifndef MAGICTYPES_H_SEEN
#define MAGICTYPES_H_SEEN

#include <stdio.h>
#include <stdlib.h>

#define I8      char
#define U8      unsigned char
#define I16     short
#define U16     unsigned short
#define I32     int
#define U32     unsigned int
#define F32     float
#define F64     double
#define Boolean char

#define FILENAME_MAX_SIZE 160
#define MAX_COM_SIZE 1000
#define HOSTNAME_SIZE 80
#define MAX_REPORT_LENGTH 160
#define MAX_PIXEL_STR_SIZE  25000

// diego check if those constants should be here or there is a better place
#define NUM_LINKS_IN_FILAR  4 // FILARs pci boards holds up to 4 S-Link channel each
#define NUM_CAPACITORS_PIX  1024 // number of capacitors in the DRS chip (each domino digitize 10 channels)
#define NUM_CAPACITORS_CH   NUM_CAPACITORS_PIX*10 // number of capacitors in the DRS chip (each domino digitize 10 channels)
// diego FIXME! the 10 is also a read from card numnber
#define MAXNUMANATH 10 // maximum numnber of analysis threads

// number of coefficients needed for the calibration
// this is the number of set of coefficients not the total number
// if we have 10 calibration levels then we have 9 set of calib. coefficients

//for 15 calibration levels
//#define NCOEF 14

#define NCOEF 20

// diego template of the new structure
typedef struct
{
  U32 globalTriggNumber;
  F64 horPos;
  U32 triggerCounter;
  U32 clockCounter;
  U32 pulsarID;
  U32 digitalData0;
  U32 digitalData1;
  U32 controlCounter;
  U32 firstCapacitor;
  // diego More info are given in the SLink packet
  // if they are needed they can be added here
} PacketDescriptor;


// diego: FIXME! old acqiris structures temporary used

typedef struct
{
  U32 globalTriggNumber;
  F64 horPos;
  U32 timeStampLo;           //!< Timestamp 
  U32 timeStampHi;
  // diego CHANGE of the DATA FORMAT
  U16 chStart;
} AqSegmentDescriptor;

//typedef struct
//{
  //    ViInt32     returnedSamplesPerSeg;
  /*
  I32     indexFirstPoint;    //!< "data[desc.indexFirstPoint]" is the first valid point. 
  ViReal64    sampTime;
  ViReal64    vGain;
  ViReal64    vOffset;
  ViInt32     returnedSegments;   //!< When reading multiple segments in one waveform
  ViInt32     nbrAvgWforms;        
  ViUInt32    actualTriggersInAcqLo;
  ViUInt32    actualTriggersInAcqHi;
  ViUInt32    actualDataSize;
  ViInt32     reserved2;    
  ViReal64    reserved3;
  */
//} AqDataDescriptor;


#endif


