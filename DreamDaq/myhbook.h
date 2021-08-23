//NTUPLE VAR
#ifndef _MYHBOOK_
#define _MYHBOOK_
/* Global Variables for HBOOK */
#include <TH1.h>
#include <TF1.h>
#include <TFile.h>
#include <string>
#include <TProfile.h>
#include "mapping.h"

TFile* hfile;
char datadir[256];
char datafilename[256];
char ntdir[256];
char peddir[256]; 
char ped_run_name[256];
char ntfilename[256];
int32_t evt_max; 
int32_t ped_run_number;
float pmean[96];
int32_t Nrunnumber;
int32_t BegTimeEvs;             // Time of first event in Seconds
int32_t BegTimeEvu;             // Time of first event in MicroSeconds
int32_t TimeEvs;                // Time of event in Seconds
int32_t TimeEvu;                // Time of event in MicroSeconds
int32_t Nevtda;                // Number of data event

                                                                           
#define N_1EV_H 50

//DBG histograms
  TH1F* hadc_dbg[N_ADC][N_CH_ADC];
  TH2F* hadc_dbg_map[N_ADC];

  //Dream ADC histograms (+debug)

#define N_TOWERS 36
#define N_RINGS 4
#define N_FIBERS 2
#define N_GAINS 1

  TH1F* hadc_newdream_tot[N_FIBERS][N_GAINS];
  TH2F* hadc_newdream_emap[N_FIBERS][N_GAINS];
  TH1F* hadc_newdream_ring_tot[N_RINGS][N_FIBERS][N_GAINS];
  TH1F* hadc_Cu_dream_tot[N_FIBERS][N_GAINS];

  //Leakage ADC histograms (+debug)
  TH1F* hadc_leakage_debug[N_CH_ADC];
  TH1F* hadc_leakage[N_CH_ADC];
  TH1F* hadc_leakage_tot;
  TH2F *hadc_leakage_map;

  //Muon ADC histograms (+debug)
  TH1F* hadc_ancillary_debug[N_CH_ADC];
  TH1F* hadc_ancillary[N_CH_ADC];
  TH2F *hadc_ancillary_map;
  TH1F *hadc_muon;

  // TDC (debug)
  TH1F* htdc_debug[N_CH_TDC];

  // DWC
  TH1F* h_dwc1_x, *h_dwc1_y;
  TH2F* h_dwc1_xy;
  TH1F* h_dwc2_x, *h_dwc2_y;
  TH2F* h_dwc2_xy;

  TH2F* h_X_dwc1_vs_dwc2;
  TH2F* h_Y_dwc1_vs_dwc2;
  TH1F* h_dwc1_ud;
  TH1F* h_dwc1_rl;
  TH1F* h_dwc2_ud;
  TH1F* h_dwc2_rl;

  //DRS stuff
  TH2F* h_drs[N_CH_DRS];
  TH1F* h_drs_1ev[N_CH_DRS][N_1EV_H];
  TH1F* h_drs_int[N_CH_DRS];
  TH1F* h_drs_cher[N_CH_DRS];
  TH1F* h_drs_sci[N_CH_DRS];
  TH1F* h_drs_ped[N_CH_DRS];
  TH1F* h_drs_int_s[N_CH_DRS];
  TH1F* h_drs_under[N_CH_DRS];
  TProfile* h_drs_average[N_CH_DRS];
  TH1F* h_drs_int_c_sum;
  TH1F* h_drs_int_s_sum;
  TH1F* h_drs_int_c_sum_p;
  TH1F* h_drs_int_s_sum_p;

  // Summary histograms
  TProfile* hsummary_dreams;
  TProfile* hsummary_dreamq;
  TProfile* hsummary_leakage;
  TProfile* hsummary_ancillary;

  //DREAM histograms 
  TH2F* hadc_dream_baricenter[2];
  TH1F* hadc_dream_baricenter_x[2];
  TH1F* hadc_dream_baricenter_y[2];
  
  //NewDREAM histograms 
  TH2F* hadc_newdream_baricenter[2];
  TH1F* hadc_newdream_baricenter_x[2];
  TH1F* hadc_newdream_baricenter_y[2];


#endif
