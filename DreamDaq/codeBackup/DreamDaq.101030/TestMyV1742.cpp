
#include <sys/time.h>
#include <iostream>
#include "myV1742.h"

using namespace std;


double tref(0), t_old(0), t_new(0);
uint64_t n_loop(0);

double get_time()
{
        double time_us;
        struct timeval t1;
        struct timezone tz;
        gettimeofday(&t1, &tz);
        t1.tv_sec -= 1287000000;
        time_us = (t1.tv_sec) * 1000000.0f + t1.tv_usec;
        return time_us;
}

v1742 * DRS4;

int main()
 {
  DRS4 = new v1742 (0x00320000, "/dev/vmedrv32d32", "/home/dreamtest/AleTest/command.list");
  uint32_t * buf = (uint32_t*)malloc(DRS4->evsize()*sizeof(unsigned int));
  unsigned int n;
  CAEN_DGTZ_V1742_EVENT_t * oldEventV1742 = newEventV1742();
  CAEN_DGTZ_V1742_EVENT_t * EventV1742 = newEventV1742();
  CAEN_DGTZ_V1742_GROUP_t * g0, * g1;

  CAEN_DGTZ_EventInfo_t eventInfo;
  void ** oevt = (void **)(&oldEventV1742);
  int j = 0;
  g0 = &(EventV1742->Group[0]);
  g1 = &(EventV1742->Group[1]);
  uint32_t * ig0, * ig1;
  ig0 = &buf[4];
  ig1 = &buf[3462];
  do
    {
     cout << " ONE MORE\n";
     cout << " buf is " << buf << endl;
     n = DRS4->readSingleEvent((unsigned int *)buf);
     cout << dec << n << " GET MORE\n";
     if (n == 0) continue;
     GetEventInfo ( buf, &eventInfo);
     cout << " buf is " << hex << buf << endl;
     cout << " PRINT MORE\n";
     cout << " EventSize " << dec << eventInfo.EventSize << endl;
     cout << " BoardId " << hex << eventInfo.BoardId << endl;
     cout << " Pattern " << eventInfo.Pattern << endl;
     cout << " ChannelMask " << eventInfo.ChannelMask << endl;
     cout << " EventCounter " << dec << eventInfo.EventCounter << endl;
     cout << " TriggerTimeTag " << hex << eventInfo.TriggerTimeTag << dec << endl;
     if (n) cout << " n " << n << endl;
     if (n) {
       tref = get_time();
       CAEN_DGTZ_DecodeEvent(10, (char *)buf, oevt);
       t_old += get_time()-tref;
       tref = get_time();
       mineV1742UnpackEventGroup( ig0, g0);
       mineV1742UnpackEventGroup( ig1, g1);
       t_new += get_time()-tref;
       n_loop ++;
     }
     usleep(10000);
     if (n) j++;
    }
  while (j<10);
  tref = get_time();
  CAEN_DGTZ_DecodeEvent(10, (char *)buf, oevt);
  t_old += get_time()-tref;
  tref = get_time();
  mineV1742UnpackEventGroup( ig0, g0);
  mineV1742UnpackEventGroup( ig1, g1);
  t_new += get_time()-tref;
  n_loop ++;
  GetEventInfo ( buf, &eventInfo);

  cout << dec << " time old " << t_old << " time new " << t_new << " n loop " << n_loop << endl;

  cout << " EventSize " << eventInfo.EventSize << endl;
  cout << " BoardId " << eventInfo.BoardId << endl;
  cout << " Pattern " << eventInfo.Pattern << endl;
  cout << " ChannelMask " << eventInfo.ChannelMask << endl;
  cout << " EventCounter " << eventInfo.EventCounter << endl;
  cout << " TriggerTimeTag " << eventInfo.TriggerTimeTag << endl;

  delete DRS4;

  return 0;
 }
