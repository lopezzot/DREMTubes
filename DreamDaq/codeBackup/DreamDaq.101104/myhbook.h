//NTUPLE VAR
#ifndef _MYHBOOK_
#define _MYHBOOK_
/* Global Variables for HBOOK */
#include <TH1.h>
#include <TF1.h>
#include <TFile.h>
#include <string>
#include <TProfile.h>

TFile *hfile;
char datadir[256];
char datafilename[256];
char ntdir[256];
char peddir[256]; 
char ped_run_name[256];
char ntfilename[256];
int evt_max; 
int ped_run_number;
float pmean[96];
int Nrunnumber;
int BegTimeEvs;             // Time of first event in Seconds
int BegTimeEvu;             // Time of first event in MicroSeconds
int TimeEvs;                // Time of event in Seconds
int TimeEvu;                // Time of event in MicroSeconds
int Nevtda;                // Number of data event

                                                                               
//New histograms

#define N_1EV_H 50
#define N_CH_OSC 4
#define N_CH_DRS 18
#define N_CH_ADC 32
#define N_CH_TDC 16

//#define N_PED_SAMPLES 200 // numberof samples to use to compute pedestal
#define N_PED_SAMPLES 100 // numberof samples to use to compute pedestal

#define N_CELLS_OSC 532
#define RANGE_OSC 512 // MAX_RANGE_OSC - MIN_RANGE_OSC
#define MIN_RANGE_OSC -127
#define MAX_RANGE_OSC 127

#define N_CELLS_DRS 1024
#define RANGE_DRS 4096 // MAX_RANGE_DRS - MIN_RANGE_DRS
#define MIN_RANGE_DRS 0
#define MAX_RANGE_DRS 4096

#define BIN_ADC 1024 
#define MIN_ADC -100 
#define MAX_ADC 4096 

  //Dream ADC histograms (+debug)
  TH1F* hadc_dream_debug[2][N_CH_ADC];
  TH1F* hadc_dream[2][N_CH_ADC];
  TH1F* hadc_dream_tot[2];
  TH2F *hadc_dream_map[2];

  TH1F* hadc_newdream[2][N_CH_ADC];
  TH1F* hadc_newdream_tot[2];

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
  TH1F* h_dwc_x, *h_dwc_y;
  TH2F* h_dwc_xy;

  // hoscilloscope stuff (+debug)
  TH2F* h_osc[N_CH_OSC];
  TH1F* h_osc_1ev[N_CH_OSC][N_1EV_H];
  TH1F* h_osc_int[N_CH_OSC];
  TH1F* h_osc_int_s[N_CH_OSC];
  TH1F* h_osc_under[N_CH_OSC];
  TH2F* h_osc_int_vs_event[N_CH_OSC];
  TProfile* h_osc_average[N_CH_OSC];

  //DRS stuff
  TH2F* h_drs[N_CH_DRS];
  TH1F* h_drs_1ev[N_CH_DRS][N_1EV_H];
  TH1F* h_drs_int[N_CH_DRS];
  TH1F* h_drs_int_s[N_CH_DRS];
  TH1F* h_drs_under[N_CH_DRS];
  TProfile* h_drs_average[N_CH_DRS];
  TH1F* h_drs_int_c_sum;
  TH1F* h_drs_int_s_sum;
  TH1F* h_drs_int_c_sum_p;
  TH1F* h_drs_int_s_sum_p;
  TH1F* h_bungabunga;

  // Scaler
  TH1F* hscaler;

  // Summary histograms
  TProfile* hsummary_dreams;
  TProfile* hsummary_dreamq;
  TProfile* hsummary_leakage;
  TProfile* hsummary_ancillary;

  //DREAM histograms 
  TH2F *hadc_dream_baricenter[2];
  TH1F *hadc_dream_baricenter_x[2];
  TH1F *hadc_dream_baricenter_y[2];
  
  //NewDREAM histograms 
  TH2F *hadc_newdream_baricenter[2];
  TH1F *hadc_newdream_baricenter_x[2];
  TH1F *hadc_newdream_baricenter_y[2];


#endif
  
