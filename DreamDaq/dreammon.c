
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>

#include <TH1.h>
#include <TF1.h>
#include <TFile.h>
#include <TH2.h>

#include "eventX742.h"
#include "eventX742_corrections.h"
#include "X742CorrectionRoutines.h"

#include "myRawFile.h"

#include "myhbook.h"
#include "mapping.h"
#include "dreammon.h"
#include "dreammon_adc.h"

using namespace std;

TFile* ped_file=0;
int32_t drs4_handle=0;
bool use_CAEN_routines = false;
CAEN_DGTZ_X742_EVENT_t *x742evt=NULL; 
CAEN_DGTZ_V1742_EVENT_t *EventV1742 = NULL; 
static DataCorrection_t	CTable[MAX_X742_GROUP_SIZE][MAX_X742_FREQ_SETTINGS_N];

// float get_pedestal(TH1F* h){
//   if(ped_file==0 || ped_file->IsZombie() || h==0)
//     return 0.;
//   //printf("ped file exists\n");
 
//   static map<string, float> cache;

//   string name = Form("%s",h->GetName());
//   //printf("h_ped: %s\n",name.c_str());
//   float ped = cache[name];

//   if(ped == 0.){ //not in cache
//     //printf("h_ped: %s\n",name.c_str());
//     //printf("not in cache\n");
//     //printf("ped_file: %s\n",ped_file->GetName());
//     TH1F* h_ped = 0;
//     ped_file->GetObject(name.c_str(),h_ped);
//     if (h_ped==0 ){
//       name = Form("Debug/%s",h->GetName());
//       ped_file->GetObject(name.c_str(),h_ped);
//     }
//     if (h_ped==0 ) {
// 	//printf("not found\n");
// 	ped = 0;
// 	cache[name] = -10000.;
// //     } else if(h_ped->GetEntries()<10){
// //       //printf("not enough stat\n");
// //       ped = 0;
// //       cache[name] = -10000.;
//     } else{
//       //printf("found!\n");
//       ped = h_ped->GetMean();
//       cache[name] = ped;
//     } 
//   } else if(ped == - 10000.){ //we already know it's not there
//     //printf("we already know it's missing!\n");
//     ped = 0;
//   }
//   //printf("ped: %f\n",ped);
//   return ped;
// }

int32_t dreammon_init(char** , uint32_t run_nr, bool , int32_t , bool phys_h)
{
  // char * myWorkDir;
  string myWorkDir;
  if (getenv("WORKDIR") == NULL)
    myWorkDir = "/home/dreamtest/storage/";
  else
    myWorkDir = getenv("WORKDIR");
  
  // Create Input/Output filenames
  if (getenv("HISTODIR") == NULL)
    sprintf(ntdir, "%s/%s", myWorkDir.c_str(), "hbook");
  else
    sprintf(ntdir, "%s", getenv("HISTODIR"));
  
  Nrunnumber= run_nr;
  if (phys_h){
    sprintf(ntfilename, "%s/datafile_histo_run%d.root", ntdir, Nrunnumber);
  }else if (!phys_h){
    sprintf(ntfilename, "%s/datafile_histo_pedestal_run%d.root", ntdir, Nrunnumber);
  }

//   if (getenv("PEDDIR") == NULL)
//     sprintf(peddir, "%s/%s", myWorkDir, "ped");
//   else
//     sprintf(peddir, "%s", getenv("PEDDIR"));
   
  // ped_run_name -- file name for the pedestal data
//   if (getenv("PED_RUN") == NULL){ 
//     sprintf(ped_run_name, "%s/pedestal_run0.cal",peddir);
//     ped_run_number=0;
//     if(phys_h){
//       ped_file = TFile::Open(Form("%s/datafile_histo_pedestal_run%d.root",
// 				  ntdir, Nrunnumber),"READ");
//       if(ped_file==0)
// 	ped_file = TFile::Open(Form("%s/datafile_histo_pedestal_run0.root",
// 				    ntdir),"READ");
      
//     }
//   }else{ 
//     sprintf(ped_run_name, "%s/pedestal_run%s.cal", peddir, getenv("PED_RUN"));
//     ped_run_number=atol(getenv("PED_RUN"));
//     if(phys_h)ped_file = TFile::Open(Form("%s/datafile_histo_pedestal_run%d.root", ntdir, ped_run_number));
//   }

//   if(!phys_h) ped_file = 0;
//   if(ped_file!=0)printf ("using ped_file %s\n", ped_file->GetName());

  if (getenv("MAXEVT") == NULL)
    evt_max= 100000;
  else
    evt_max = atol(getenv("MAXEVT"));                                                                                         
 
  init_mapping();

  adc_init();

  char histo[256];
  char name[256];

  // TDC (DWC) histograms
  for ( int32_t i_tdc_ch=0;i_tdc_ch<16;i_tdc_ch++) {
    sprintf(histo,"TDC_ch%02d",i_tdc_ch);  
    sprintf(name,"TDC ch %d",i_tdc_ch);  
    htdc_debug[i_tdc_ch]=new TH1F (histo,name,BIN_ADC,-0.5,4095.5);
  }

  h_dwc1_x = new TH1F ("DWC1_x","DWC1 x",   60,-50.5,49.5);
  h_dwc1_y = new TH1F ("DWC1_y","DWC1 y",   60,-50.5,49.5);
  h_dwc1_xy = new TH2F ("DWC1_xy","DWC1 xy",60,-50.5,49.5,60,-50.5,49.5);
  h_dwc1_ud = new TH1F ("DWC1_ud","DWC1 ud",   200,-1000,1000);
  h_dwc1_rl = new TH1F ("DWC1_rl","DWC1 rl",   200,-1000,1000);

  h_dwc2_x = new TH1F ("DWC2_x","DWC2 x",   60,-50.5,49.5);
  h_dwc2_y = new TH1F ("DWC2_y","DWC2 y",   60,-50.5,49.5);
  h_dwc2_xy = new TH2F ("DWC2_xy","DWC2 xy",60,-50.5,49.5,60,-50.5,49.5);
  h_dwc2_ud = new TH1F ("DWC2_ud","DWC2 ud",   300,-300,300);
  h_dwc2_rl = new TH1F ("DWC2_rl","DWC2 rl",   300,-300,300);

  h_X_dwc1_vs_dwc2 = new TH2F("h_X_dwc1_vs_dwc2","h_X_dwc1_vs_dwc2",60,-50.5,49.5, 60,-50.5,49.5 );
  h_Y_dwc1_vs_dwc2 = new TH2F("h_Y_dwc1_vs_dwc2","h_Y_dwc1_vs_dwc2",60,-50.5,49.5, 60,-50.5,49.5 );

  // DRS
  int32_t ret=0;

  if(use_CAEN_routines){
    ret |= CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_PCI_OpticalLink, 0, 0, 0x0, &drs4_handle);
    if(ret!=0) cout << "ERROR " << ret << " could not initialize V1792" << endl;
    ret |= CAEN_DGTZ_AllocateEvent(drs4_handle, (void**)&x742evt);
    if(ret!=0) cout << "ERROR " << ret << " could allocate mem for V1792 event" << endl;
  }else{
    x742evt = newEventX742();
    EventV1742 = newEventV1742();
  }

  char fname[256];
  sprintf(fname,"/home/dreamtest/working/board_corrections.dat");
  FILE *fp=fopen(fname,"r");
  fread(&CTable, sizeof(CTable), 1, fp);
  fclose(fp);

  char thename[256]; 
  for(int32_t i_drs_ch = 0; i_drs_ch <N_CH_DRS; i_drs_ch++ )
  {
    sprintf(thename,"DRS_ch%02d", i_drs_ch);

    sprintf(histo,"%s_map", thename);
    h_drs[i_drs_ch] = new TH2F(histo, histo, N_CELLS_DRS,0, N_CELLS_DRS, RANGE_DRS,
			       MIN_RANGE_DRS,MAX_RANGE_DRS);
	
    for(int32_t i_ev = 0; i_ev <N_1EV_H; i_ev++ ){
      h_drs_1ev[i_drs_ch][i_ev] = 0;
    }

    sprintf(histo,"%s_integral_cher", thename);
    h_drs_cher[i_drs_ch] = new TH1F(histo, histo,  RANGE_DRS/10,
				   -MAX_RANGE_DRS*3,MAX_RANGE_DRS*20); 
    sprintf(histo,"%s_integral_sci", thename);
    h_drs_sci[i_drs_ch] = new TH1F(histo, histo,  RANGE_DRS/10,
				   -MAX_RANGE_DRS*3,MAX_RANGE_DRS*10); 
    sprintf(histo,"%s_integral_ped", thename);
    h_drs_ped[i_drs_ch] = new TH1F(histo, histo,  RANGE_DRS/10,
				   -MAX_RANGE_DRS*2,MAX_RANGE_DRS*2); 

    sprintf(histo,"%s_integral_ped_subtracted", thename);
    h_drs_int_s[i_drs_ch] = new TH1F(histo, histo,  RANGE_DRS/10,
				   -MAX_RANGE_DRS*2,MAX_RANGE_DRS*2); 
    sprintf(histo,"%s_integral", thename);
    //h_drs_int[i_drs_ch] = new TH1F(histo, histo,  RANGE_DRS*2, -MAX_RANGE_DRS*1000,MAX_RANGE_DRS*1000); 
    h_drs_int[i_drs_ch] = new TH1F(histo, histo,  RANGE_DRS/10, 0,MAX_RANGE_DRS*1000); 

    sprintf(histo,"%s_underflow", thename);
    h_drs_under[i_drs_ch] = new TH1F(histo, histo, 3,-1.5,1.5);
    h_drs_under[i_drs_ch]->GetXaxis()->SetBinLabel(1,"# Underflow");
    h_drs_under[i_drs_ch]->GetXaxis()->SetBinLabel(2,"# Ok");
    h_drs_under[i_drs_ch]->GetXaxis()->SetBinLabel(3,"# Overflow");
  } 

  string n;
  n.assign("DRS_PWO_C_integral_sum");
  h_drs_int_c_sum   = new TH1F(n.c_str(), n.c_str(),  RANGE_DRS/10, -MAX_RANGE_DRS*1000,MAX_RANGE_DRS*1000);
  n.assign("DRS_PWO_S_integral_sum");
  h_drs_int_s_sum   = new TH1F(n.c_str(), n.c_str(),  RANGE_DRS/10, -MAX_RANGE_DRS*1000,MAX_RANGE_DRS*1000);
  n.assign("DRS_PWO_C_integral_sum-pedsubtr");
  h_drs_int_c_sum_p = new TH1F(n.c_str(), n.c_str(),  RANGE_DRS/10, -MAX_RANGE_DRS*1000,MAX_RANGE_DRS*1000);
  n.assign("DRS_PWO_S_integral_sum_pedsubtr");
  h_drs_int_s_sum_p = new TH1F(n.c_str(), n.c_str(),  RANGE_DRS/10, -MAX_RANGE_DRS*1000,MAX_RANGE_DRS*1000);

  //cout << "init done" << endl;
  return 0;
}

static bool debugprint(false);

int32_t dreammon_event(uint32_t , uint32_t evt, uint32_t* buf, bool , int32_t ){

  //cout << "start of event: " << evt << endl;
	  
  int32_t rc;
  uint32_t* addr;
  //mySCA scaData;
  myTDC tdcData;

  EventHeader * head=(EventHeader *) buf;
  if(head->evmark!=0xCAFECAFE){
    cout << "Cannot find the event marker. Something is wrong." << endl;
    return 0;
  }
  uint32_t i = head->evhsiz/sizeof(uint32_t);

  Nevtda   = 0; 
  // Fill histo of run number and pedestal run number 
  Nevtda   = head->evnum;

  uint32_t spill_nr = head->spill;

  rc=0;
  uint32_t sz = (head->evsiz - head->evhsiz )/4;

  if ((sz > 5000) && (sz < 10000)) debugprint = true;
  if (0) if (debugprint)
    {
      printf( "WRONG SIZE Nevtda %d spill %d size %d [0] %x [1] %x [2] %x [3] %x [4] %x [5] %x [6] %x [7] %x\n", Nevtda, spill_nr, sz, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
      printf( "WRONG SIZE [i] %x [i+1] %x [i+2] %x [i+3] %x [i+4] %x [i+5] %x [i+6] %x [i+7] %x\n", buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]);
      fflush(stdout);
    }

  adc_event(buf, i, sz);
  ////////////////////////////// Fill new histograms

  //Decode and  Fill TDC V775 debug histos
  addr = SubEventSeek2(tdc_vme_addr, &buf[i],sz);
  rc += DecodeV775(addr, &tdcData);
  int32_t dwc1_l=9990, dwc1_r=9990, dwc1_u=9990, dwc1_d=9990;
  int32_t dwc2_l=9990, dwc2_r=9990, dwc2_u=9990, dwc2_d=9990;

  bool tdc_is_good_event= true;

  int32_t const empty = -9999;
  vector<int32_t> channelVect(16, empty);

  for (uint32_t j=0; j<tdcData.index; j++) 
    { 
      int32_t ch = tdcData.channel[j];
      int32_t data = tdcData.data[j];
      int32_t valid = tdcData.valid[j];
      //cout << ch << "\t";
      if (!valid) cout << "[Warning] TDC ch = " << ch << "; invalid data " << endl;
      channelVect[ch]=data;
      htdc_debug[ch]->Fill(data);
      switch(ch){
      case 0: {dwc1_l = data; break;}
      case 1: {dwc1_r = data; break;}
      case 2: {dwc1_u = data; break;}
      case 3: {dwc1_d = data; break;}
      case 4: {dwc2_l = data; break;}
      case 5: {dwc2_r = data; break;}
      case 6: {dwc2_u = data; break;}
      case 7: {dwc2_d = data; break;}
      }
    }
  //cout << endl;

  if(tdcData.index!=0 && tdc_is_good_event){
    //X-position = (timeRight - timeLeft) * horizontalSlope + horizontalOffset
    //Y-position = (timeUp - timeDown) * verticalSlope + verticalOffset

    // Dec2014 calibration 
    float hSlope1 = -0.178388; // fit
    float hOffset1 = 0.646796; // fit
    float vSlope1 = 0.174544; // fit
    float vOffset1 = 1.0665; // fit

    float hSlope2 = -0.172184; // fit
    float hOffset2 = -0.00774708; // fit
    float vSlope2 = 0.175517; // fit
    float vOffset2 = 0.61149; // fit

    float couts_to_ns = 0.14; //was 0.2472

    // Do not calculate the difference if one piece of data is missing (9990)
    float x1 = 9990., x2 = 9990. , y1 = 9990., y2 = 9990.;
    if( (dwc1_r + dwc1_l) < 9900 )  x1 = (dwc1_r - dwc1_l) * hSlope1*couts_to_ns + hOffset1;
    if( (dwc2_r + dwc2_l) < 9900 )  x2 = (dwc2_r - dwc2_l) * hSlope2*couts_to_ns + hOffset2;
    if( (dwc1_u + dwc1_d) < 9900 )  y1 = (dwc1_u - dwc1_d) * vSlope1*couts_to_ns + vOffset1;
    if( (dwc2_u + dwc2_d) < 9900 )  y2 = (dwc2_u - dwc2_d) * vSlope2*couts_to_ns + vOffset2;
	    
    h_dwc1_rl->Fill((dwc1_r - dwc1_l)*couts_to_ns);
    h_dwc2_rl->Fill((dwc2_r - dwc2_l)*couts_to_ns);
    h_dwc1_ud->Fill((dwc1_u - dwc1_d)*couts_to_ns);
    h_dwc2_ud->Fill((dwc2_u - dwc2_d)*couts_to_ns);

    h_dwc1_xy->Fill(x1,y1);
    h_dwc1_x->Fill(x1);
    h_dwc1_y->Fill(y1);
    h_dwc2_xy->Fill(x2,y2);
    h_dwc2_x->Fill(x2);
    h_dwc2_y->Fill(y2);
    h_X_dwc1_vs_dwc2->Fill(x1,x2);
    h_Y_dwc1_vs_dwc2->Fill(y1,y2);
    //printf("h_dwc1_xy->Fill(x1[%f],y1[%f]);\n",x1,y1);

  }

  int32_t i_1ev_h = evt/100;
  bool fill_1ev_h = (evt%100==0) && (i_1ev_h < N_1EV_H);

  // *******************************************************
  // DRS stuff - Cascella 2011
  // *******************************************************

  addr =  SubEventSeek2(drs_vme_addr, &buf[i], sz);
  if(addr!=0){
    //DRS with board specific corrections 

    //will have to start checking for active groups, eventually
    CAEN_DGTZ_EventInfo_t eventInfo;
    GetEventInfo((char*)(addr+4), &eventInfo );

      // DRS "DGTZ" buffers

    if(use_CAEN_routines){
      int32_t ret=0;
      ret = CAEN_DGTZ_DecodeEvent(drs4_handle, (char*)(addr+4), (void**)&x742evt);
      if(ret!=0) cout << "ERROR " << ret << " could not decode event" << endl;
    }else{
      for(int32_t gr = 0; gr<MAX_X742_GROUP_SIZE; gr++){
	//if the group is not present in the mask skip it
	if(! ((eventInfo.ChannelMask >> gr) & 1) )continue;
	V1742UnpackEventGroup((uint32_t *)addr+(8+3458*gr), &(EventV1742->Group[gr])); 
      }

      for(int32_t i_gr=0; i_gr<MAX_X742_GROUP_SIZE; i_gr++){
	//if the group is not present in the mask skip it
	if(! ((eventInfo.ChannelMask >> i_gr) & 1) )continue;
	x742evt->DataGroup[i_gr].TriggerTimeTag = EventV1742->Group[i_gr].TriggerTimeTag;
	x742evt->DataGroup[i_gr].StartIndexCell = EventV1742->Group[i_gr].StartIndexCell;
	for(int32_t i_ch=0; i_ch<MAX_X742_CHANNEL_SIZE; i_ch++){
	  for(int32_t sample=0; sample<1024; sample++){
	    x742evt->DataGroup[i_gr].DataChannel[i_ch][sample]= EventV1742->Group[i_gr].Data[i_ch][sample];
	  }
	}
      }
    }


    for(int32_t i_gr=0; i_gr<MAX_X742_GROUP_SIZE; i_gr++){
      //if the group is not present in the mask skip it
      if(! ((eventInfo.ChannelMask >> i_gr) & 1) )continue;
      CAEN_DGTZ_DRS4Frequency_t freq = CAEN_DGTZ_DRS4_5GHz; //can be read from data
      ApplyDataCorrection(&CTable[i_gr][freq], 
			  freq,
			  7, // it's a bitmask 7in dec = 111 in binary
			  &(x742evt->DataGroup[i_gr]));
    }
	   
	 

    // Now prepare data and fill our histos - These are Michele's stuff with minor changes...
    float cher[N_CH_DRS], sci[N_CH_DRS], sum_sci, sum_cher;
    float ped[N_CH_DRS], sum_ped;
    float integral[N_CH_DRS];
    bool underflow[N_CH_DRS];
    bool overflow[N_CH_DRS];
    float data;

#define N_PED_SAMPLES 60 // numberof samples to use to compute pedestal
#define N_SCI_LOW 250 
#define N_SCI_HI 550
#define N_CHER_LOW 80 
#define N_CHER_HI 130
	 
    for(int32_t i_ch=0; i_ch<N_CH_DRS; i_ch++) {
      integral[i_ch] = 0;
      underflow[i_ch] = false;
      overflow[i_ch] = false;
    }

    ////
    if(fill_1ev_h){
      char histo[20];
      char thename[20]; 
      for(int32_t i_drs_ch = 0; i_drs_ch <N_CH_DRS; i_drs_ch++ ){
	sprintf(thename,"DRS_ch%02d", i_drs_ch);
	sprintf(histo,"%s_ev%05d",thename,i_1ev_h*100);
	h_drs_1ev[i_drs_ch][i_1ev_h] = new TH1F(histo, histo, N_CELLS_DRS,0, N_CELLS_DRS);
      }
    }

    for(int32_t gr = 0; gr<MAX_X742_GROUP_SIZE; gr++){
      for(int32_t i_ch=0; i_ch<MAX_X742_CHANNEL_SIZE; i_ch++ ) {
	int32_t ii = i_ch + MAX_X742_CHANNEL_SIZE * gr;
	if(ii>=N_CH_DRS) continue;
	sum_ped=0; 
	sum_cher=0; 
	sum_sci=0; 

	int32_t cell_tot = x742evt->DataGroup[gr].ChSize[i_ch];
	for(int32_t cell=0; cell < cell_tot; cell++) {
	  data = x742evt->DataGroup[gr].DataChannel[i_ch][cell]; 
		  
	  if(data <= MIN_RANGE_DRS) underflow[ii] = true;
	  if(data >= MAX_RANGE_DRS) overflow[ii] = true;
	  //accumulate data for pedestal, cher and sci integrals
	  if(cell<N_PED_SAMPLES) sum_ped+=data; 
	  if( N_CHER_LOW<cell && cell < N_CHER_HI) sum_cher+=data; 
	  if( N_SCI_LOW<cell && cell < N_SCI_HI) sum_sci+=data; 

	  integral[ii] += data;
	  h_drs[ii]->Fill(cell,data);

	  if(fill_1ev_h) h_drs_1ev[ii][i_1ev_h]->Fill(cell,data);
	}
	// compute averages
	ped[ii]=sum_ped/N_PED_SAMPLES; 
	cher[ii]=sum_cher; 
	sci[ii]=sum_sci; 
      }
    }

    if (debugprint) if (sz > 10000) debugprint = false;

    for (int32_t i_ch=0; i_ch<N_CH_DRS; i_ch++ ) {
      if (underflow[i_ch])
	h_drs_under[i_ch]->Fill(-1);
      else if (overflow[i_ch])
	h_drs_under[i_ch]->Fill(1);
      else
	h_drs_under[i_ch]->Fill(0);
	      
      h_drs_ped[i_ch]->Fill(ped[i_ch]);
      h_drs_sci[i_ch]->Fill(ped[i_ch]*(N_SCI_HI-N_SCI_LOW)-sci[i_ch]);
      h_drs_cher[i_ch]->Fill(ped[i_ch]*(N_CHER_HI-N_CHER_LOW)-cher[i_ch]);

      h_drs_int_s[i_ch]->Fill(integral[i_ch]-ped[i_ch]*1024);
      h_drs_int[i_ch]->Fill(integral[i_ch]);
      if    ( i_ch >= 0 && i_ch <=6) {   // C side
	h_drs_int_c_sum  ->Fill(integral[i_ch]);
	h_drs_int_c_sum_p->Fill(-integral[i_ch]+ped[i_ch]*1024);
      }
      else if ( i_ch >= 9 && i_ch <= 15) { // S side
	h_drs_int_s_sum  ->Fill(integral[i_ch]);
	h_drs_int_s_sum_p->Fill(-integral[i_ch]+ped[i_ch]*1024);
      }
    }


    // *******************************************************
    // End of DRS stuff
    // *******************************************************
  }
  return 0;
}
	 
int32_t dreammon_sync(uint32_t , bool , int32_t ){
 
  TFile hfile(ntfilename,"RECREATE");
  if (hfile.IsZombie()) {
    fprintf(stderr, "ERROR Cannot open  file %s\n", ntfilename);
    return -1;
  }

  //debug plots in the Debug directory

  TDirectory *dbg_dir = hfile.mkdir("Debug");
 
  adc_sync(&hfile, dbg_dir);
 
  hfile.cd("Debug");

  // debug histos for TDC - DWC
  for ( int32_t i_tdc_ch=0;i_tdc_ch<16;i_tdc_ch++) {
    htdc_debug[i_tdc_ch]->Write();
  }

  h_dwc1_ud->Write();
  h_dwc1_rl->Write();
  h_dwc2_ud->Write();
  h_dwc2_rl->Write();

  h_dwc2_x->Write();
  h_dwc2_y->Write();

  h_dwc1_x->Write();
  h_dwc1_y->Write();

  dbg_dir->mkdir("Random_events");
  dbg_dir->cd("Random_events");
  for(int32_t i_drs_ch = 0; i_drs_ch <N_CH_DRS; i_drs_ch++ ){
    for(int32_t i_ev = 0; i_ev <N_1EV_H; i_ev++ ){
      if(h_drs_1ev[i_drs_ch][i_ev] != 0){
	h_drs_1ev[i_drs_ch][i_ev]->Write();
      }
    }
  }

  // new plots are divided in directories
  hfile.cd("");
 
  hfile.mkdir("DWC");
  hfile.cd("DWC");

  h_dwc1_xy->Write();
  h_dwc2_xy->Write();
  h_X_dwc1_vs_dwc2->Write();
  h_Y_dwc1_vs_dwc2->Write();

  //Save DRS Histos
  hfile.cd("");
  hfile.mkdir("DRS");
  hfile.cd("DRS");

  for(int32_t i_drs_ch = 0; i_drs_ch <N_CH_DRS; i_drs_ch++ ){
    h_drs[i_drs_ch]->Write();
    h_drs_int[i_drs_ch]->Write();
    h_drs_int_s[i_drs_ch]->Write();
    h_drs_under[i_drs_ch]->Write();
    string n = h_drs[i_drs_ch]->GetName();
    n.append("_average_shape");
    h_drs_average[i_drs_ch] = h_drs[i_drs_ch]->ProfileX();
    h_drs_average[i_drs_ch]->SetNameTitle(n.c_str(),n.c_str());
    h_drs_average[i_drs_ch]->Write();
  }

  hfile.Close();
  printf("monitoring histograms written to:\n%s\n",ntfilename);

  return 0;
}

int32_t dreammon_exit(uint32_t i, bool drs, int32_t drs_setup){

  if(use_CAEN_routines){
    // End of analsis - free DRS "DGTZ" buffers
    CAEN_DGTZ_FreeEvent(drs4_handle, (void**)&x742evt);
    CAEN_DGTZ_CloseDigitizer(drs4_handle);
  }

  return dreammon_sync(i, drs, drs_setup);
}
/***************************************************/
// Local Variables:
// mode: c++
// End:
