#ifndef _EVENT_X742_H_
#define _EVENT_X742_H_

#include <CAENDigitizer.h>

#define NUM_SAMPLES             1024
#define MAX_V1742_CHANNEL_SIZE   9
#define MAX_V1742_GROUP_SIZE     2

typedef struct
 {
  uint16_t TotSize;
  uint16_t StartIndexCell;
  uint32_t TriggerTimeTag;
  uint16_t Data[MAX_V1742_CHANNEL_SIZE][NUM_SAMPLES];  // the array of ChSize samples
 } CAEN_DGTZ_V1742_GROUP_t;

typedef struct
 {
  CAEN_DGTZ_V1742_GROUP_t Group[MAX_V1742_GROUP_SIZE];
 } CAEN_DGTZ_V1742_EVENT_t;

void test_v1742_raw ( uint32_t u);

void GetEventInfo( void * eventPtr, CAEN_DGTZ_EventInfo_t * eventInfo );

CAEN_DGTZ_V1742_EVENT_t * newEventV1742();
void destroyEventV1742( CAEN_DGTZ_V1742_EVENT_t * ep );
size_t sizeEventV1742();
size_t sizeGroupV1742();
int V1742UnpackEventGroup( uint32_t * in, CAEN_DGTZ_V1742_GROUP_t * out);

#endif // _EVENT_X742_H_
