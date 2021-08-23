//#include <CAENDigitizer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "eventX742.h"
#include "strEvX742.h"

CAEN_DGTZ_V1742_EVENT_t * newEventV1742()
{
	CAEN_DGTZ_V1742_EVENT_t * ep;
	CAEN_DGTZ_V1742_GROUP_t * g0, * g1;

	//	printf("In newEvent1742\n");
	fflush(stdout);
	size_t sz = sizeof(CAEN_DGTZ_V1742_EVENT_t);
	ep = (CAEN_DGTZ_V1742_EVENT_t *)malloc(sz);
	if (ep == NULL) return NULL;
        memset(ep, 0, sz);
	g0 = &(ep->Group[0]);
	g1 = &(ep->Group[1]);
	//printf("newEventV1742 sizeof(CAEN_DGTZ_V1742_GROUP_t) %d\n", sizeof(CAEN_DGTZ_V1742_GROUP_t));
	//printf("newEventV1742 sizeof(CAEN_DGTZ_V1742_EVENT_t) %d\n", sizeof(CAEN_DGTZ_V1742_EVENT_t));
	//printf("newEventV1742 4 %p %p\n", g0, g1);
	return ep;
}

void destroyEventV1742( CAEN_DGTZ_V1742_EVENT_t * ep )
{
	free(ep);
}

size_t sizeEventV1742()
{
	return sizeof(CAEN_DGTZ_V1742_EVENT_t);
}

size_t sizeGroupV1742()
{
	return sizeof(CAEN_DGTZ_V1742_GROUP_t);
}

void GetEventInfo( void * eventPtr, CAEN_DGTZ_EventInfo_t * eventInfo )
{
//	uint32_t * u32p = (uint32_t *)eventPtr;
// printf ("GetEventInfo %p %x %x %x %x\n", u32p, u32p[0], u32p[1], u32p[2], u32p[3]);
	v1742_raw * vp32 = (v1742_raw *)eventPtr;
	eventInfo->EventSize = 4*vp32[0].h1.sz;
	eventInfo->BoardId = vp32[1].h2.id;
	eventInfo->Pattern = vp32[1].h2.ptt;
	eventInfo->ChannelMask = vp32[1].h2.mk;
	eventInfo->EventCounter = vp32[2].v;
	eventInfo->TriggerTimeTag = vp32[3].v;
}

void test_v1742_raw ( uint32_t u)
{
	v1742_raw qqq;
	qqq.v = u;
	printf ("\n\n\nsizeof(v1742_raw) %d v %x \n", sizeof(v1742_raw), qqq.v);
	printf ("h1 %x %x\n", qqq.h1.mk, qqq.h1.sz);
	printf ("h2 %x %x %x\n", qqq.h2.id, qqq.h2.ptt, qqq.h2.mk);
	printf ("h3 %x\n", qqq.h3.ec);
	printf ("h4 %x\n", qqq.h4.ett);
	printf ("gdw %x %x %x\n", qqq.gdw.sid, qqq.gdw.tsz, qqq.gdw.csz);
	printf ("ttt %x\n", qqq.ttt.ttt);
	printf ("dt1 %x %x %x\n", qqq.dt1.dtL, qqq.dt1.dtM, qqq.dt1.dtH);
	printf ("dt2 %x %x %x %x\n", qqq.dt2.dtL, qqq.dt2.dtK, qqq.dt2.dtI, qqq.dt2.dtH);
	printf ("dt3 %x %x %x\n\n\n\n", qqq.dt3.dtL, qqq.dt3.dtM, qqq.dt3.dtH);
}

int V1742UnpackEventGroup(uint32_t *datain, CAEN_DGTZ_V1742_GROUP_t *dataout)
{
	int i, rpnt = 0, wpnt = 0, size1, size2;
	long samples;
	uint16_t st_ind=0; 

	st_ind =(uint16_t)((datain[0]>>21)& 0x3FF);
	size1  = datain[0]& 0xFFF;
	size2  =(datain[0]>>12)& 0x1FF;

	dataout->TriggerTimeTag =datain[size1+size2+1] & 0x3FFFFFFF; // per leggere il TTT # Trinidad and Tobago Television ??? Touch-Tone Terrorists ???

	samples = ((long) (size1 /3)); 
	while (rpnt < size1) {

	switch (rpnt % 3) {
          case 0 :
              dataout->Data[0][wpnt]  =  (uint16_t) (datain[rpnt+1] & 0x00000FFF);            /* S0[11:0] - CH0 */
              dataout->Data[1][wpnt]  =  (uint16_t) ((datain[rpnt+1] & 0x00FFF000) >> 12);     /* S0[11:0] - CH1 */
              dataout->Data[2][wpnt]  =  (uint16_t) ((datain[rpnt+1] & 0xFF000000) >> 24);     /* S0[ 7:0] - CH2 */
              break;
          case 1 :
              dataout->Data[2][wpnt] |=  (uint16_t) ((datain[rpnt+1] & 0x0000000F) << 8);      /* S0[11:8] - CH2 */
              dataout->Data[3][wpnt]  =  (uint16_t) ((datain[rpnt+1] & 0x0000FFF0) >> 4);      /* S0[11:0] - CH3 */
              dataout->Data[4][wpnt]  =  (uint16_t) ((datain[rpnt+1] & 0x0FFF0000) >> 16);     /* S0[11:0] - CH4 */
              dataout->Data[5][wpnt]  =  (uint16_t) ((datain[rpnt+1] & 0xF0000000)>> 28);     /* S0[3:0]  - CH5 */
              break;
          case 2 :
              dataout->Data[5][wpnt] |=  (uint16_t)  ((datain[rpnt+1] & 0x000000FF) << 4);      /* S0[11:4] - CH5 */
              dataout->Data[6][wpnt]  =  (uint16_t) ((datain[rpnt+1] & 0x000FFF00) >> 8);      /* S0[11:0] - CH6 */
              dataout->Data[7][wpnt]  =  (uint16_t) ((datain[rpnt+1] & 0xFFF00000) >> 20);     /* S0[11:0] - CH7 */
              wpnt++;
              break;
        }
        rpnt++;
    }

    rpnt++; // scorro per non considerare la prima parola
	wpnt=0;

	for (i=0; i < size2; i++) {
        switch (i % 3) {
          case 0 :
              dataout->Data[8][wpnt++]  =  (uint16_t)  ( datain[rpnt+i] & 0x00000FFF);            /* S0 - CH9 */
              dataout->Data[8][wpnt++]  =  (uint16_t)  ((datain[rpnt+i] & 0x00FFF000) >> 12);     /* S1 - CH9 */
              dataout->Data[8][wpnt]    = (uint16_t)  ( (datain[rpnt+i] & 0xFF000000) >> 24);     /* S2[ 7:0] - CH9 */
              break;
          case 1 :
              dataout->Data[8][wpnt++] |= (uint16_t)  ( (datain[rpnt+i] & 0x0000000F) << 8);      /* S2[11:8] - CH9 */
              dataout->Data[8][wpnt++]  =  (uint16_t)  ((datain[rpnt+i] & 0x0000FFF0) >> 4);      /* S3 - CH9 */
              dataout->Data[8][wpnt++]  =  (uint16_t)  ((datain[rpnt+i] & 0x0FFF0000) >> 16);     /* S4 - CH9 */
              dataout->Data[8][wpnt]    =  (uint16_t)  ((datain[rpnt+i] & 0xF0000000)>> 28);      /* S5[3:0]  - CH9 */
              break;
          case 2 :
              dataout->Data[8][wpnt++] |=  (uint16_t)  ((datain[rpnt+i] & 0x000000FF) << 4);      /* S5[11:4] - CH9 */
              dataout->Data[8][wpnt++]  =  (uint16_t)  ((datain[rpnt+i] & 0x000FFF00) >> 8);      /* S6[11:0] - CH9 */
              dataout->Data[8][wpnt++]  =  (uint16_t)  ((datain[rpnt+i] & 0xFFF00000) >> 20);     /* S7[11:0] - CH9 */
              break;
        }
    }
	dataout->StartIndexCell = (uint16_t)st_ind;
  return 0;
}

