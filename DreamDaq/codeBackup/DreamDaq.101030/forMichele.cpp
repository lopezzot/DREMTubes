#include "eventX742.h"

  CAEN_DGTZ_EventInfo_t eventInfo;
  CAEN_DGTZ_V1742_GROUP_t * g0, * g1;
  uint32_t * ig0, * ig1;

  CAEN_DGTZ_V1742_EVENT_t * EventV1742 = newEventV1742();

  g0 = &(EventV1742->Group[0]);
  g1 = &(EventV1742->Group[1]);
buf[0] --> 4 event info 
  GetEventInfo ( buf, &eventInfo);
  cout << " EventSize " << eventInfo.EventSize << endl;
  cout << " BoardId " << eventInfo.BoardId << endl;
  cout << " Pattern " << eventInfo.Pattern << endl;
  cout << " ChannelMask " << eventInfo.ChannelMask << endl;
  cout << " EventCounter " << eventInfo.EventCounter << endl;
  cout << " TriggerTimeTag " << eventInfo.TriggerTimeTag << endl;

  ig0 = &buf[4];
  ig1 = &buf[3462];
  V1742UnpackEventGroup( ig0, g0);
  V1742UnpackEventGroup( ig1, g1);

 
