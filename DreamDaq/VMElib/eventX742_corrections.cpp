
#include "eventX742_corrections.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int32_t off_cal[32][1024],off_caltrg[4][1024];
int32_t off_num[32][1024],off_numtrg[4][1024];					   
float TCell[4][1024];

int32_t loadCorrectionFiles() {

  int32_t ch;
  FILE* f_corr;
  //int32_t corr_cell=0;
  //int32_t corr_numsamp=0;
  //int32_t corr_time=0;
  char OutFileName[100];
  int32_t offset;    
  float flapp;
  int32_t i; 


  for (ch=0; ch<32;ch++)
    {
      sprintf(OutFileName, "%sOffsetError_ch%d.txt", DEFAULT_CONFIG_PATH, ch);
      f_corr=fopen (OutFileName,"r");
      if (f_corr !=NULL) {
	i=0;
	while(!feof(f_corr)) {
	  fscanf(f_corr, "%d\n", &offset);
	  off_cal[ch][i]=offset;
	  i++;
	}
	fclose (f_corr);
      }
      else
	return -1;
    }

  for (ch=0; ch<4;ch++)
    {
      sprintf(OutFileName, "%sOffsetError_trg%d.txt", DEFAULT_CONFIG_PATH,ch);
      f_corr=fopen (OutFileName,"r");
      if (f_corr !=NULL) {
	i=0;
	while(!feof(f_corr)) {
	  fscanf(f_corr, "%d\n", &offset);
	  off_caltrg[ch][i]=offset;
	  i++;
	}
	fclose (f_corr);
      }
      else
	return -1;
    }


  for (ch=0; ch<32;ch++)
    {
      sprintf(OutFileName, "%sNumSampleError_ch%d.txt", DEFAULT_CONFIG_PATH, ch);
      f_corr=fopen (OutFileName,"r");
      if (f_corr !=NULL) {
	i=0;
	while(!feof(f_corr)) {
	  fscanf(f_corr, "%d\n", &offset);
	  off_num[ch][i]=offset;
	  i++;
	}
	off_num[ch][0]=0;
	fclose (f_corr);
      }
      else
	return -1;
    }

  for (ch=0; ch<4;ch++)
    {
      sprintf(OutFileName, "%sNumSampleError_trg%d.txt", DEFAULT_CONFIG_PATH, ch);
      f_corr=fopen (OutFileName,"r");
      if (f_corr !=NULL) {
	i=0;
	while(!feof(f_corr)) {
	  fscanf(f_corr, "%d\n", &offset);
	  off_numtrg[ch][i]=offset;
	  i++;
	}
	off_num[ch][0]=0;
	fclose (f_corr);
      }
      else
	return -1;
    }

  for (ch=0; ch<4;ch++)
    {
      sprintf(OutFileName, "%sTime_chip%d.txt", DEFAULT_CONFIG_PATH, ch);
      f_corr=fopen (OutFileName,"r");
      if (f_corr !=NULL) {
	i=0;
	while(!feof(f_corr)) {
	  fscanf(f_corr, "%f\n", &flapp);
	  TCell[ch][i]=flapp;
	  i++;
	}
	fclose (f_corr);
      }
      else
	return -1;
    }
  return 0;
}


void DataCorrection(CAEN_DGTZ_V1742_GROUP_t* group0, 
		    CAEN_DGTZ_V1742_GROUP_CORRECTED_t* group, 
		    int32_t gr) {
	
  group->TotSize = group0->TotSize;
  group->StartIndexCell = group0->StartIndexCell;
  group->TriggerTimeTag = group0->TriggerTimeTag;
  
  float Time[NUM_SAMPLES],t0;
  float Tsamp=(float)((1.0/2500.0)*1000.0);
  int32_t i,j,k;
  float vcorr;
  int32_t offset;
  float wave_fin[NUM_SAMPLES];
	   
  t0=TCell[gr][group->StartIndexCell];                       
  Time[0]=0.0;

  for(j=1; j < NUM_SAMPLES; j++) {
    t0= TCell[gr][((group->StartIndexCell)+j)%NUM_SAMPLES]-t0;
    if  (t0 >0) 
      Time[j] =  Time[j-1]+ t0;
    else
      Time[j] =  Time[j-1]+ t0 + (Tsamp*NUM_SAMPLES);

    t0 = TCell[gr][((group->StartIndexCell)+j)%NUM_SAMPLES];
  }
		
  for(j=0; j < MAX_V1742_CHANNEL_SIZE; j++) {
    for(i=0; i<NUM_SAMPLES; i++)  {
      if (j==8) 
	offset=off_caltrg[gr][(group->StartIndexCell+i)%NUM_SAMPLES]+off_numtrg[gr][i];
      else 
	offset=off_cal[gr*8+j][(group->StartIndexCell+i)%NUM_SAMPLES]+off_num[gr*8+j][i];
      group->Data[j][i] = group0->Data[j][i] - offset;
    }
    group->Data[j][0] = group->Data[j][1];
    wave_fin[0]=group->Data[j][0];
    vcorr = 0.0;
    k=0;
    i=0;
			
    for(i=1; i<NUM_SAMPLES; i++)  {
      while ((k<NUM_SAMPLES-1) && (Time[k]<(i*Tsamp)))  k++;
      vcorr =(((float)(group->Data[j][k] - group->Data[j][k-1])/(Time[k]-Time[k-1]))*((i*Tsamp)-Time[k-1]));
      wave_fin[i]= group->Data[j][k-1] + vcorr;
      k--;								
    }
    memcpy(group->Data[j],wave_fin,NUM_SAMPLES*sizeof(float));
  }
}
