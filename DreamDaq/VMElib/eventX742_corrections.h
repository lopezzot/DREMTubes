#ifndef _EVENT_X742_CORRECTIONS_H_
#define _EVENT_X742_CORRECTIONS_H_

#include <stdint.h>

#include "eventX742.h"

#define DEFAULT_CONFIG_PATH  "/etc/wavedump/"

typedef struct
 {
  uint16_t TotSize;
  uint16_t StartIndexCell;
  uint32_t TriggerTimeTag;
  float Data[MAX_V1742_CHANNEL_SIZE][NUM_SAMPLES];  // the array of ChSize samples
 } CAEN_DGTZ_V1742_GROUP_CORRECTED_t;

int32_t loadCorrectionFiles();
void DataCorrection(CAEN_DGTZ_V1742_GROUP_t* group0, 
		    CAEN_DGTZ_V1742_GROUP_CORRECTED_t* group, 
		    int32_t gr);

#endif //_EVENT_X742_CORRECTIONS_H_
