/*************************************************************************

        dreammon.cxx
        -----------

        object oriented implementation of the monitoring

        Version 0.1,      M. Cascella 2010 
*********************************************************************/

#ifndef DREAMMON_H
#define DREAMMON_H

#include <TH1.h>
#include <TH2.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TFile.h>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>

#define N_1EV_H 50
#define N_CH_OSC 4
#define N_CH_DRS 18

#define DREAM_ADC_BASE_ADDR 0x04000005

class dreammon {
public:
  dreammon(unsigned int run_nr, bool phys_h);
  ~dreammon(){};

  int event(unsigned int evt, unsigned int * buf);
  int exit();

private: 

  Float_t get_pedestal(TH1F* h);
  std::map<std::string, float> ped_cache;
  TFile* hfile;
  TFile* ped_file;
  string datadir;
  string datafile;
  string ntdir; //FIXME is it used anywhere?
  string ntfile;
  string peddir;
  string pedfile;
  int Nrunnumber;
  int 1ev_counter;  
  
  //Dream ADC histograms (+debug)
  TH1F* hadc_dream_debug[2][32];
  TH1F* hadc_dream[2][32];
  TH1F* hadc_dream_tot[2];

  TH1F* hadc_newdream[2][32];
  TH1F* hadc_newdream_tot[2];

  //DREAM histograms 
  TH2F *hadc_dream_map[2];
  TProfile hadc_dream_prof[2];
  TH2F *hadc_dream_baricenter[2];
  TH1F *hadc_dream_baricenter_x[2];
  TH1F *hadc_dream_baricenter_y[2];
  
  //NewDREAM histograms 
  TH2F *hadc_newdream_map[2];
  TProfile hadc_newdream_prof[2];
  TH2F *hadc_newdream_baricenter[2];
  TH1F *hadc_newdream_baricenter_x[2];
  TH1F *hadc_newdream_baricenter_y[2];

  //Muon ADC histograms (+debug)
  TH1F* hadc_muons_debug[32];
  TH1F* hadc_muons[32];

  // TDC (debug)
  TH1F* htdc_debug[16];

  // DWC
  TH1F* hdwc[4]; //l r u d
  TH1F* h_dwc_x, *h_dwc_y;
  TH2F* h_dwc_xy;

  // hoscilloscope stuff (+debug)
  TH1F* h_osc[N_CH_OSC];
  TH2F* h_osc_mean[N_CH_OSC];
  TProfile* h_osc_mean_profile[N_CH_OSC];
//   TH2F* h_osc_mean_v[N_CH_OSC];
//   TProfile* h_osc_mean_profile_v[N_CH_OSC];
  TH1F* h_osc_1ev[N_CH_OSC][N_1EV_H];
  TH1F* h_osc_int[N_CH_OSC];
  TH1F* h_osc[N_CH_OSC];
  TH1F* h_osc_under[N_CH_OSC];

  //DRS stuff
  TH1F* h_drs[N_CH_DRS];
  TH2F* h_drs_mean[N_CH_DRS];
  TProfile* h_drs_mean_profile[N_CH_DRS];
  TH1F* h_drs_1ev[N_CH_DRS][N_1EV_H];
  TH1F* h_drs_int[N_CH_DRS];
  TH1F* h_drs[N_CH_DRS];
  TH1F* h_drs_under[N_CH_DRS];

};
#endif
