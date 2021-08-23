//#include <CAENDigitizer.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "eventX742.h"
#include "strEvX742.h"

CAEN_DGTZ_V1742_EVENT_t* newEventV1742()
{
	CAEN_DGTZ_V1742_EVENT_t * ep;
	//CAEN_DGTZ_V1742_GROUP_t * g0, * g1;

	size_t sz = sizeof(CAEN_DGTZ_V1742_EVENT_t);
	printf("newEvent1742 size is %d\n", sz);
	fflush(stdout);
	//printf("VAFFA 1: %d \n", sz);
	ep = (CAEN_DGTZ_V1742_EVENT_t *)malloc(sz);
	//printf("VAFFA 2\n");
	if (ep == NULL) {
		printf("Cazzi amari\n");
		return NULL;
        }
        memset(ep, 0, sz);
	//g0 = &(ep->Group[0]);
	//g1 = &(ep->Group[1]);
	//printf("newEventV1742 sizeof(CAEN_DGTZ_V1742_GROUP_t) %d\n", sizeof(CAEN_DGTZ_V1742_GROUP_t));
	//printf("newEventV1742 sizeof(CAEN_DGTZ_V1742_EVENT_t) %d\n", sizeof(CAEN_DGTZ_V1742_EVENT_t));
	//printf("newEventV1742 4 %p %p\n", g0, g1);
	return ep;
}

CAEN_DGTZ_X742_EVENT_t* newEventX742()
{
  CAEN_DGTZ_X742_EVENT_t* ep;
  //CAEN_DGTZ_X742_GROUP_t* g0, * g1;

  //	printf("In newEvent1742\n");
  //fflush(stdout);
  size_t sz = sizeof(CAEN_DGTZ_X742_EVENT_t);
  printf("newEventX742 size is %d\n", sz);
  fflush(stdout);
  ep = (CAEN_DGTZ_X742_EVENT_t*)malloc(sz);
  if (ep == NULL) return NULL;
  memset(ep, 0, sz);
  int32_t i_gr=0;
  for(i_gr=0; i_gr<MAX_X742_GROUP_SIZE; i_gr++){
    ep->GrPresent[i_gr]=1;
    int32_t i_ch=0;
    for(i_ch=0; i_ch<MAX_X742_CHANNEL_SIZE; i_ch++){
      ep->DataGroup[i_gr].ChSize[i_ch]=1024;
      ep->DataGroup[i_gr].DataChannel[i_ch] = (float*) malloc(1024*sizeof(float));
    }
  }
  //g0 = &(ep->Group[0]);
  //g1 = &(ep->Group[1]);
  //printf("newEventX742 sizeof(CAEN_DGTZ_X742_GROUP_t) %d\n", sizeof(CAEN_DGTZ_X742_GROUP_t));
  //printf("newEventX742 sizeof(CAEN_DGTZ_X742_EVENT_t) %d\n", sizeof(CAEN_DGTZ_X742_EVENT_t));
  //printf("newEventX742 4 %p %p\n", g0, g1);
  return ep;
}


void destroyEventV1742( CAEN_DGTZ_V1742_EVENT_t* ep )
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

void GetEventInfo( void* eventPtr, CAEN_DGTZ_EventInfo_t* eventInfo )
{
//	uint32_t* u32p = (uint32_t*)eventPtr;
// printf ("GetEventInfo %p %x %x %x %x\n", u32p, u32p[0], u32p[1], u32p[2], u32p[3]);
	v1742_raw* vp32 = (v1742_raw*)eventPtr;
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
	printf ("\n\n\nsizeof(v1742_raw) %zu v %x \n", sizeof(v1742_raw), qqq.v);
	printf ("h1 %x %x\n", qqq.h1.mk, qqq.h1.sz);
	printf ("h2 %x %x %x\n", qqq.h2.id, qqq.h2.ptt, qqq.h2.mk);
	printf ("h3 %x\n", qqq.h3.ec);
	printf ("h4 %x\n", qqq.h4.ett);
	printf ("gdw %x %x %x\n", qqq.gdw.sid, qqq.gdw.trig, qqq.gdw.csz);
	printf ("ttt %x\n", qqq.ttt.ttt);
	printf ("dt1 %x %x %x\n", qqq.dt1.dtL, qqq.dt1.dtM, qqq.dt1.dtH);
	printf ("dt2 %x %x %x %x\n", qqq.dt2.dtL, qqq.dt2.dtK, qqq.dt2.dtI, qqq.dt2.dtH);
	printf ("dt3 %x %x %x\n\n\n\n", qqq.dt3.dtL, qqq.dt3.dtM, qqq.dt3.dtH);
}

int32_t V1742UnpackEventGroup(uint32_t* datain, CAEN_DGTZ_V1742_GROUP_t* dataout)
{
	int32_t i, rpnt = 0, wpnt = 0, size1, size2;
	uint16_t st_ind=0; 

	st_ind =(uint16_t)((datain[0]>>20)& 0x3FF);
	size1  = datain[0]& 0xFFF;
	size2  = size1/8 * ((datain[0]>>12) & 0x1);
	  //size2  = (datain[0]>>12)& 0x1FF;
/* 	printf(" size2 = size1/8 * ((datain[0]>>12) & 0x1) = %x/8 * ((%x>>12[=%x]) & 0x1) = %d (%x) \n",size1, datain[0], (datain[0]>>12), size2,size2); */
/* 	printf(" size1 %d (%x)\n size2 %d (%x) \n size1+size2+1 %d (%x)\n datain[size1+size2+1] %x\n",size1,size1,size2,size2,size1+size2+1,size1+size2+1,datain[size1+size2+1]); */

	dataout->TriggerTimeTag = datain[size1+size2+1] & 0x3FFFFFFF; 
	dataout->StartIndexCell = (uint16_t)st_ind;

	// long samples = ((long) (size1 /3)); 
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
  return 0;
}

