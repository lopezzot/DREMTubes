static int V1742UnpackEventGroup(uint32_t *datain, CAEN_DGTZ_X742_GROUP_t *dataout) {

    int i, rpnt = 0, wpnt = 0, size1, size2;
    long samples;
   uint16_t st_ind=0; 

	st_ind =(uint16_t)((datain[0]>>21)& 0x3FF);
	size1  = datain[0]& 0xFFF;
	size2  =(datain[0]>>12)& 0x1FF;

	dataout->TriggerTimeTag =datain[size1+size2+1] & 0x3FFFFFFF; // per leggere il TTT

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

/*

typedef struct 
{
	uint16_t				*Data[MAX_X742_CHANNEL_SIZE];  // the array of ChSize samples
	uint32_t				 TriggerTimeTag;
	uint16_t				 StartIndexCell;
} CAEN_DGTZ_X742_GROUP_t;

typedef struct 
{
	uint32_t			 	GrSize[MAX_X742_GROUP_SIZE]; // the number of samples stored in DataChannel array  
	CAEN_DGTZ_X742_GROUP_t	DataGroup[MAX_X742_GROUP_SIZE]; // the array of ChSize samples
} CAEN_DGTZ_X742_EVENT_t;

*/
