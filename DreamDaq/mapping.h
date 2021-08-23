#ifndef _MAPPING_H_
#define _MAPPING_H_

//////////////////////////  MAPPING //////////////////////////

#include <TString.h>

#define N_ADC 5 

#define N_CH_DRS 36
#define N_CH_ADC 32
#define N_CH_TDC 16

#define N_CELLS_DRS 1024
#define RANGE_DRS 4096 // MAX_RANGE_DRS - MIN_RANGE_DRS
#define MIN_RANGE_DRS 0
#define MAX_RANGE_DRS 4096

//#define BIN_ADC 1024 
#define BIN_ADC 4196 
#define MIN_ADC -100 
#define MAX_ADC 4096 

/* -- io e Ale abbiam messo nella daq i 4 adc che ci servono per leggere newdream. Gli indirizzi li sono qui di seguito: */

/* ADC792: */
/* adc0 --> 0*0400 */
/* adc1     0*0500 */
/* adc2       0600 */
/* adc3       0700 */

/* ADCv862 */
/* adc4     0*0300 */

int32_t adc_num_to_vme_addr[N_ADC] = {0x04000005,0x05000005,0x06000005,0x07000005,0x03000025};
int32_t tdc_vme_addr = 0x08000024;
int32_t drs_vme_addr = 0x00032026;

TString adc_num_to_name[N_ADC];
void init_adc_name() {
  for(int32_t i_adc=0; i_adc<N_ADC; i_adc++){
    adc_num_to_name[i_adc] = Form("ADC%d",i_adc);
  }
}

TString adc_addr_to_name[N_ADC][N_CH_ADC];
int32_t adc_addr_to_tower[N_ADC][N_CH_ADC];
int32_t adc_addr_to_fiber[N_ADC][N_CH_ADC];
int32_t adc_addr_to_amp[N_ADC][N_CH_ADC];
int32_t adc_addr_to_x[N_ADC][N_CH_ADC];
int32_t adc_addr_to_y[N_ADC][N_CH_ADC];
int32_t adc_addr_to_Cu_dream[N_ADC][N_CH_ADC];
int32_t adc_addr_to_Cu_fiber[N_ADC][N_CH_ADC];
int32_t adc_addr_to_Cu_amp[N_ADC][N_CH_ADC];
int32_t adc_addr_to_leakage[N_ADC][N_CH_ADC];
int32_t adc_addr_to_xtal[N_ADC][N_CH_ADC];
int32_t adc_addr_to_ancillary[N_ADC][N_CH_ADC];

//this map can be automatically generated using "scripts/tower_map_gen.cxx"
int32_t tower_to_ring[] = {-1, 2, 2, 2, 2, 2, 3, 2, 1, 1, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 1, 1, 2, 3, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3}; 


inline int32_t get_tower(int32_t adc, int32_t ch){return adc_addr_to_tower[adc][ch];}
inline int32_t get_fiber(int32_t adc, int32_t ch){return adc_addr_to_fiber[adc][ch];}
inline int32_t get_amp(int32_t adc, int32_t ch){return adc_addr_to_amp[adc][ch];}
inline int32_t get_x(int32_t adc, int32_t ch){return adc_addr_to_x[adc][ch];}
inline int32_t get_y(int32_t adc, int32_t ch){return adc_addr_to_y[adc][ch];}

inline int32_t get_leakage_ch(int32_t adc, int32_t ch){return adc_addr_to_leakage[adc][ch];}
inline int32_t get_Cu_dream_ch(int32_t adc, int32_t ch){return adc_addr_to_Cu_dream[adc][ch];}
inline int32_t get_xtal_ch(int32_t adc, int32_t ch){return adc_addr_to_xtal[adc][ch];}
inline int32_t get_ancillary_ch(int32_t adc, int32_t ch){return adc_addr_to_ancillary[adc][ch];}
inline TString get_name(int32_t adc, int32_t ch){return adc_addr_to_name[adc][ch];}
inline int32_t get_ring(int32_t tower){return tower_to_ring[tower];}

TString getModuleName( char cfiber, int tower)
 {
  int PMT[4] = { 4, 1, 3, 2 };
  tower --;
  int module = (tower/12)*3 + (tower%6)/2 + 1;
  int key1 = tower%12;
  int key4 = (key1%2)<<1 | key1/6;
  char mname[5];
  mname[0] = 'M';
  mname[1] = module+'0';
  mname[2] = cfiber;
  mname[3] =  PMT[key4]+'0';
  mname[4] = '\0';
  return TString(mname);
 }

void init_adc_map(){
  for(int32_t i_adc=0; i_adc<N_ADC; i_adc++){

    for(int32_t ch=0;ch<N_CH_ADC;ch++){
      adc_addr_to_name[i_adc][ch]="";
      adc_addr_to_fiber[i_adc][ch]=-1;
      adc_addr_to_amp[i_adc][ch]=-1;
      adc_addr_to_x[i_adc][ch]=-1;
      adc_addr_to_y[i_adc][ch]=-1;
      adc_addr_to_tower[i_adc][ch]=-1;
      adc_addr_to_leakage[i_adc][ch]=-1;
      adc_addr_to_Cu_dream[i_adc][ch]=-1;
      adc_addr_to_Cu_fiber[i_adc][ch]=-1;
      adc_addr_to_Cu_amp[i_adc][ch]=-1;
      adc_addr_to_xtal[i_adc][ch]=-1;
      adc_addr_to_ancillary[i_adc][ch]=-1;

      if(i_adc <3){
	int32_t fiber=-1, amp=-1, tower=-1;
	//fiber = 0 -> S
	//        1 -> C

	// this map can be automatically generated with "scripts/tower_map_gen.cxx"
	if(0) if(i_adc == 0){
	  switch(ch){
	  // case 0: tower = 7; fiber = 0; amp = 0; break;
	  case 1: tower = 7; fiber = 1; amp = 0; break;
	  case 2: tower = 8; fiber = 0; amp = 0; break;
	  case 3: tower = 8; fiber = 1; amp = 0; break;
	  case 4: tower = 2; fiber = 0; amp = 0; break;
	  case 5: tower = 2; fiber = 1; amp = 0; break;
	  case 6: tower = 1; fiber = 0; amp = 0; break;
	  case 7: tower = 1; fiber = 1; amp = 0; break;
	  case 8: tower = 9; fiber = 0; amp = 0; break;
	  case 9: tower = 9; fiber = 1; amp = 0; break;
	  case 10: tower = 10; fiber = 0; amp = 0; break;
	  case 11: tower = 10; fiber = 1; amp = 0; break;
	  case 12: tower = 4; fiber = 0; amp = 0; break;
	  case 13: tower = 4; fiber = 1; amp = 0; break;
	  case 14: tower = 3; fiber = 0; amp = 0; break;
	  case 15: tower = 3; fiber = 1; amp = 0; break;
	  case 16: tower = 11; fiber = 0; amp = 0; break;
	  case 17: tower = 11; fiber = 1; amp = 0; break;
	  case 18: tower = 12; fiber = 0; amp = 0; break;
	  case 19: tower = 12; fiber = 1; amp = 0; break;
	  case 20: tower = 6; fiber = 0; amp = 0; break;
	  case 21: tower = 6; fiber = 1; amp = 0; break;
	  case 22: tower = 5; fiber = 0; amp = 0; break;
	  case 23: tower = 5; fiber = 1; amp = 0; break;
	  case 24: tower = 19; fiber = 0; amp = 0; break;
	  case 25: tower = 19; fiber = 1; amp = 0; break;
	  case 26: tower = 20; fiber = 0; amp = 0; break;
	  case 27: tower = 20; fiber = 1; amp = 0; break;
	  case 28: tower = 14; fiber = 0; amp = 0; break;
	  case 29: tower = 14; fiber = 1; amp = 0; break;
	  case 30: tower = 13; fiber = 0; amp = 0; break;
	  case 31: tower = 13; fiber = 1; amp = 0; break;
	  }
	}

	if(0) if(i_adc == 1){
	  switch(ch){
	  case 0: tower = 21; fiber = 0; amp = 0; break;
	  case 1: tower = 21; fiber = 1; amp = 0; break;
	  case 2: tower = 22; fiber = 0; amp = 0; break;
	  case 3: tower = 22; fiber = 1; amp = 0; break;
	  case 4: tower = 16; fiber = 0; amp = 0; break;
	  // case 5: tower = 16; fiber = 1; amp = 0; break;
	  case 6: tower = 15; fiber = 0; amp = 0; break;
	  case 7: tower = 15; fiber = 1; amp = 0; break;
	  case 8: tower = 23; fiber = 0; amp = 0; break;
	  case 9: tower = 23; fiber = 1; amp = 0; break;
	  case 10: tower = 24; fiber = 0; amp = 0; break;
	  case 11: tower = 24; fiber = 1; amp = 0; break;
	  case 12: tower = 18; fiber = 0; amp = 0; break;
	  case 13: tower = 18; fiber = 1; amp = 0; break;
	  case 14: tower = 17; fiber = 0; amp = 0; break;
	  case 15: tower = 17; fiber = 1; amp = 0; break;
	  case 16: tower = 31; fiber = 0; amp = 0; break;
	  case 17: tower = 31; fiber = 1; amp = 0; break;
	  case 18: tower = 32; fiber = 0; amp = 0; break;
	  case 19: tower = 32; fiber = 1; amp = 0; break;
	  case 20: tower = 26; fiber = 0; amp = 0; break;
	  case 21: tower = 26; fiber = 1; amp = 0; break;
	  case 22: tower = 25; fiber = 0; amp = 0; break;
	  case 23: tower = 25; fiber = 1; amp = 0; break;
	  case 24: tower = 33; fiber = 0; amp = 0; break;
	  case 25: tower = 33; fiber = 1; amp = 0; break;
	  case 26: tower = 34; fiber = 0; amp = 0; break;
	  case 27: tower = 34; fiber = 1; amp = 0; break;
	  case 28: tower = 28; fiber = 0; amp = 0; break;
	  case 29: tower = 28; fiber = 1; amp = 0; break;
	  case 30: tower = 27; fiber = 0; amp = 0; break;
	  case 31: tower = 27; fiber = 1; amp = 0; break;
	  }
	}

	if(0) if(i_adc == 2){
	  switch(ch){
	  case 0: tower = 35; fiber = 0; amp = 0; break;
	  case 1: tower = 35; fiber = 1; amp = 0; break;
	  case 2: tower = 36; fiber = 0; amp = 0; break;
	  case 3: tower = 36; fiber = 1; amp = 0; break;
	  case 4: tower = 30; fiber = 0; amp = 0; break;
	  case 5: tower = 30; fiber = 1; amp = 0; break;
	  case 6: tower = 29; fiber = 0; amp = 0; break;
	  case 7: tower = 29; fiber = 1; amp = 0; break;
	  case 8: tower = 7; fiber = 0; amp = 0; break;
	  case 9: tower = 16; fiber = 1; amp = 0; break;
	  }
	}
	if(tower > 0){
	  adc_addr_to_tower[i_adc][ch]=tower;
	  adc_addr_to_fiber[i_adc][ch]=fiber;
	  adc_addr_to_amp[i_adc][ch]=amp;
	  adc_addr_to_x[i_adc][ch]=(tower-1)%6;
	  adc_addr_to_y[i_adc][ch]=(tower-1)/6;
	  char cfiber = (fiber==1) ? 'C' : 'S';
	  /* 	  TString samp = (amp==1) ? "High" : "Low"; */
	  /* 	  adc_addr_to_name[i_adc][ch]=Form("%c%d_%s",cfiber,tower,samp.Data()); */
	  TString mname = getModuleName(cfiber,tower);
          TString buildn = Form("%s_%c%d",mname.Data(),cfiber,tower);
	  adc_addr_to_name[i_adc][ch]=buildn; // Form("%c%d_%s",cfiber,tower,uname.Data()); // + '_' + uname;
	  std::cout << i_adc << "[" << ch << "] => tower " << tower << " " << cfiber << " " << mname << " " << buildn << " " << adc_addr_to_name[i_adc][ch] << std::endl;
	}

      // Al Cu module 
      if(i_adc == 2){
	int32_t fiber=-1;
	//fiber = 0 -> S
	//        1 -> C
	switch(ch){
	case 16: fiber = 0;adc_addr_to_name[i_adc][ch]="Al_Cu S1";break;
	case 17: fiber = 1;adc_addr_to_name[i_adc][ch]="Al_Cu C1";break;
	case 18: fiber = 0;adc_addr_to_name[i_adc][ch]="Al_Cu S2";break;
	case 19: fiber = 1;adc_addr_to_name[i_adc][ch]="Al_Cu C2";break;
	case 20: fiber = 0;adc_addr_to_name[i_adc][ch]="Al_Cu S3";break;
	case 21: fiber = 1;adc_addr_to_name[i_adc][ch]="Al_Cu C3";break;
	case 22: fiber = 0;adc_addr_to_name[i_adc][ch]="Al_Cu S4";break;
	case 23: fiber = 1;adc_addr_to_name[i_adc][ch]="Al_Cu C4";break;
	}
	if(fiber>=0){
	  adc_addr_to_Cu_dream[i_adc][ch]=ch;
	  adc_addr_to_Cu_fiber[i_adc][ch]=fiber;
	  adc_addr_to_Cu_amp[i_adc][ch]=0;
	}

	//analog adders
	if(0) if(i_adc == 2){
	  switch(ch){
	  case 10: tower = 0; adc_addr_to_name[i_adc][ch]="Analog Ring1-S";break;
	  case 11: tower = 0; adc_addr_to_name[i_adc][ch]="Analog Ring1-C";break;
	  case 12: tower = 0; adc_addr_to_name[i_adc][ch]="Analog Ring2-S";break;
	  case 13: tower = 0; adc_addr_to_name[i_adc][ch]="Analog Ring2-C";break;
	  case 14: tower = 0; adc_addr_to_name[i_adc][ch]="Analog Ring3-S";break;
	  case 15: tower = 0; adc_addr_to_name[i_adc][ch]="Analog Ring3-C";break;
	  }
	  if(tower >= 0){
	    adc_addr_to_tower[i_adc][ch]=tower;
	  }  
	}

	// staggered module
	if(i_adc == 1){
	  switch(ch){
	  case 0: tower = 0; adc_addr_to_name[i_adc][ch]="T1SC-HV01";break;
	  case 1: tower = 0; adc_addr_to_name[i_adc][ch]="T1SL-HV02";break;
	  case 2: tower = 0; adc_addr_to_name[i_adc][ch]="T2SC-HV05";break;
	  case 3: tower = 0; adc_addr_to_name[i_adc][ch]="T2SL-HV06";break;
	  case 4: tower = 0; adc_addr_to_name[i_adc][ch]="T3SC-HV09";break;
	  // case 5: tower = 0; adc_addr_to_name[i_adc][ch]="T3SL-HV10";break;
	  case 6: tower = 0; adc_addr_to_name[i_adc][ch]="T4SC-HV13";break;
	  case 7: tower = 0; adc_addr_to_name[i_adc][ch]="T4SL-HV14";break;
	  case 8: tower = 0; adc_addr_to_name[i_adc][ch]="T1CC-HV03";break;
	  case 9: tower = 0; adc_addr_to_name[i_adc][ch]="T1CL-HV04";break;
	  case 10: tower = 0; adc_addr_to_name[i_adc][ch]="T2CC-HV07";break;
	  case 11: tower = 0; adc_addr_to_name[i_adc][ch]="T2CL-HV08";break;
	  case 12: tower = 0; adc_addr_to_name[i_adc][ch]="T3CC-HV11";break;
	  case 13: tower = 0; adc_addr_to_name[i_adc][ch]="T3CL-HV12";break;
	  case 14: tower = 0; adc_addr_to_name[i_adc][ch]="T4CC-HV15";break;
	  case 15: tower = 0; adc_addr_to_name[i_adc][ch]="T4CL-HV16";break;
	  case 16: tower = 0; adc_addr_to_name[i_adc][ch]="T3SL-HV10";break;
	  }
	  if(tower >= 0){
	    adc_addr_to_tower[i_adc][ch]=tower;
	  }  
	}
	
      }

      }
      
      //leakage counters
      if(i_adc == 3 && ch>=16){ 
	adc_addr_to_leakage[i_adc][ch] = ch-16+1; 
	adc_addr_to_name[i_adc][ch]=Form("L%d",ch-16+1);
      }
      if(i_adc==4){
	switch(ch){
	case 0: adc_addr_to_name[i_adc][ch]="L17";break;
	case 1: adc_addr_to_name[i_adc][ch]="L18";break;
	case 3: adc_addr_to_name[i_adc][ch]="L20";break;
	case 4: adc_addr_to_name[i_adc][ch]="L19";break;
	}
	if(adc_addr_to_name[i_adc][ch]!="")adc_addr_to_leakage[i_adc][ch]=ch;
      }

      //ancillary
      if(i_adc==4 && ch > 4){
	switch(ch){
	case 6: adc_addr_to_name[i_adc][ch]="CET-3";break;
	case 7: adc_addr_to_name[i_adc][ch]="CET-4";break;
	case 8: adc_addr_to_name[i_adc][ch]="IT";break;
	case 9: adc_addr_to_name[i_adc][ch]="T3-PS";break;
	case 10: adc_addr_to_name[i_adc][ch]="TC";break;
	case 11: adc_addr_to_name[i_adc][ch]="MU";break;
	}
	if(adc_addr_to_name[i_adc][ch]!="")adc_addr_to_ancillary[i_adc][ch]=ch;
      }


    }
  }
  for(int32_t i_adc=N_ADC; i_adc<N_ADC; i_adc++){

    for(int32_t ch=0;ch<8;ch++){
      adc_addr_to_name[i_adc][ch]=Form("ADC%d_%d",i_adc,ch);
      adc_addr_to_fiber[i_adc][ch]=-1;
    }
  }
}

void init_mapping(){
  init_adc_name();
  init_adc_map();
}

#endif // _MAPPING_H_
