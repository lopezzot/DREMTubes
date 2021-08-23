// std library includes

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

// root includes

#include <TTree.h>
#include <TString.h>
#include <TFile.h>
#include <TMath.h>

#include "run_offset.h"

void merge2017(int runnumber, bool data, int offset = 9999, bool debug = 0){

  if(offset == 9999) offset=run_offset(runnumber);

  // merged tree

  TString datadir = "/home/dreamtest/storage/";
  if (getenv("DATADIR") != NULL)
    datadir = Form("%s", getenv("DATADIR"));

  TString merged_output_dir = datadir + "merged/";
  TString merged_output_filename = "merged_";
  if(!data) merged_output_filename = "merged_ped_";
  TString daq_input_dir = datadir + "ntuple/";
  TString mada_dir = datadir + "MADA_ntuple/";

  merged_output_filename += runnumber;
  merged_output_filename += ".root";

  TFile *fmerge = new TFile(merged_output_dir + merged_output_filename,"RECREATE");
  TTree* tmerge = new TTree("DREAM_merged","DREAM merged data");


  TString filename;
  if(data) filename = daq_input_dir + "datafile_ntup_run";
  else filename = daq_input_dir + "pedestal_ntup_run";
  filename += runnumber;
  filename += ".root";

  // trees to sync
  TFile *fdream = TFile::Open(filename);
  if (!fdream) std::cout << "Cannot find file " << filename << std::endl;

  filename = mada_dir + "RUN";
  filename += runnumber;
  filename += ".root";
  TFile *fmada = TFile::Open(filename);
  if (!fmada) std::cout << "Cannot find file " << filename << std::endl;

  TTree * tdream = (TTree*) fdream->Get("DREAM");
  TTree * tmada = (TTree*) fmada->Get("DREAM_SiPM");

  /* synced tree variables */

//debug stuff
tmerge->Branch("offset",&offset,"offset/i");

//dream daq
  UInt_t run_n;
  tdream->SetBranchAddress("Nrunnumber", &run_n);
  tmerge->Branch("run_n",&run_n,"run_n/i");
  UInt_t dd_evn;
  tdream->SetBranchAddress("Nevtda", &dd_evn);
  tmerge->Branch("dd_evn",&dd_evn,"dd_evn/i");

  //to be filled
  ULong64_t dd_time_corrected_ns;
  tmerge->Branch("dd_time_ns",&dd_time_corrected_ns,"dd_time_ns/l");
  Long64_t dd_delta_t;
  tmerge->Branch("dd_delta_t",&dd_delta_t,"dd_delta_t/l");

//dream detectors
  UInt_t adc_muon;
  tmerge->Branch("adc_muon",&adc_muon,"adc_muon/i");
  UInt_t adc_preshower;
  tmerge->Branch("adc_preshower",&adc_preshower,"adc_preshower/i");
  UInt_t adc_CET3;
  tmerge->Branch("adc_CET3",&adc_CET3,"adc_CET3/i");
  UInt_t adc_CET4;
  tmerge->Branch("adc_CET4",&adc_CET4,"adc_CET4/i");
  Float_t dwc_x;
  tmerge->Branch("dwc_x",&dwc_x,"dwc_x/F");
  Float_t dwc_y;
  tmerge->Branch("dwc_y",&dwc_y,"dwc_y/F");

//mada daq
  ULong64_t mada_evn;
  tmada->SetBranchAddress("event", &mada_evn);
  tmerge->Branch("mada_evn",&mada_evn,"mada_evn/i");
  Double_t sipm_ch[64];
  for(int ch=0;ch<64;ch++){tmada->SetBranchAddress(Form("ch%d",ch), &sipm_ch[ch]);}
  tmerge->Branch("sipm_ch",sipm_ch,"sipm_ch[64]/D");

  // to be filled
  ULong64_t mada_time_ns;
  tmerge->Branch("mada_time_ns",&mada_time_ns,"mada_time_ns/l");
  Long64_t mada_delta_t;
  tmerge->Branch("mada_delta_t",&mada_delta_t,"mada_delta_t/L");
  Int_t n_rollover = 0;
  tmerge->Branch("n_mada_rollover",&n_rollover,"n_mada_rollover/I");
  Float_t mada_tot_energy = 0;
  tmerge->Branch("mada_tot_energy",&mada_tot_energy,"mada_tot_energy/F");
  Float_t mada_rfrac = 0;
  tmerge->Branch("mada_rfrac",&mada_rfrac,"mada_rfrac/F");

  // support variables to read from trees
  UInt_t dd_time_s; // Time of event in Seconds
  tdream->SetBranchAddress("TimeEvs", &dd_time_s);
  UInt_t dd_time_us; // Time of event in NanoSeconds
  tdream->SetBranchAddress("TimeEvu", &dd_time_us);

  UInt_t adc_counts[32];
  tdream->SetBranchAddress("CHARGEADCN4", adc_counts);
  Float_t X[32];
  tdream->SetBranchAddress("X", X);
  Float_t Y[32];
  tdream->SetBranchAddress("Y", Y);
  Int_t preshower_ch = 9;
  Int_t CET3_ch = 6;
  Int_t CET4_ch = 7;
  Int_t muon_ch = 11;

  ULong64_t mada_time;
  tmada->SetBranchAddress("time", &mada_time);

  // other support variables
  tmada->GetEntry(0);
  tdream->GetEntry(0);

  Long64_t dd_nentries = tdream->GetEntries();
  Long64_t mada_nentries = tmada->GetEntries();

  std::cout << "Found " << dd_nentries << " events in the daq input file and " << mada_nentries << " in the mada input file" << std::endl;

  ULong64_t dd_time_prev = (((Long64_t)dd_time_s) * 1000000 + ((Long64_t)dd_time_us))*1000;
  ULong64_t mada_time_prev = mada_time*20;

  Long64_t mentry=0;

  std::vector<float> mada_ch;
  mada_ch.reserve(64);

  for(Long64_t dentry = 0; dentry<dd_nentries; dentry++){
    tdream->GetEntry(dentry);

    dd_evn += offset;

    if (debug) std::cout << "dd_evn = " << dd_evn << std::endl;

    while((dd_evn>mada_evn) & (mada_evn<mada_nentries)){
      if (debug) std::cout << "mada_evn = " << mada_evn << " -> mentry++ " << std::endl;
      mentry++;
      tmada->GetEntry(mentry);
      if(mada_evn<0.5)continue; // skip events with evn = 0
      if (debug) std::cout << "mada_evn = " << mada_evn << std::endl;
    }
    if(mada_evn>=mada_nentries) {break;}

    if(dd_evn<mada_evn){
      if (debug) std::cout << "dd_evn < mada_evn " << " -> dd_evn++ " << std::endl;
      continue;
    }

    if(mada_evn!=dd_evn){
      std::cout <<"Something went wrong mada_evn != dd_evn"<< std::endl;
      abort();
    } else {
      if (debug) std::cout << "mada_evn == dd_evn -> merge"<< std::endl;
    }

// dream detectors
    adc_muon = adc_counts[muon_ch];
    adc_CET3 = adc_counts[CET3_ch];
    adc_CET4 = adc_counts[CET4_ch];
    adc_preshower = adc_counts[preshower_ch];
    dwc_x = X[0];
    dwc_y = Y[0];
    dd_time_corrected_ns = (((Long64_t)dd_time_s) * 1000000 + ((Long64_t)dd_time_us))*1000;
    dd_delta_t = dd_time_corrected_ns - dd_time_prev;
    dd_time_prev = dd_time_corrected_ns;

    mada_time_ns = mada_time*20;
    if(((Double_t)mada_time_ns - mada_time_prev)<0){
      n_rollover++;
      mada_time_ns = mada_time_ns + n_rollover*TMath::Power(2,32);
    }
    mada_delta_t = mada_time_ns - mada_time_prev;
    mada_time_prev = mada_time_ns;

    mada_tot_energy = 0;

    // compute the ratio between the most energetic channel and the sum of the five most energetic channels

    for (unsigned int i = 0; i < 64; ++i){
      mada_ch.push_back(sipm_ch[i]);
    }

    std::sort(mada_ch.begin(), mada_ch.end(),std::greater<float>());

    // Now compute the ration between the most energetic fiber and the sum of the five most energetic channels

    float mada_sum=0;

    for (unsigned int i = 0; i < 5; ++i){
      mada_sum += mada_ch[i];
    }

    mada_ch.clear();
    mada_rfrac = mada_ch[0]/mada_sum;


    for(int ch=0;ch<64;ch++){
      if (ch == 0 || ch == 2 || ch == 4 || ch == 6 ||
	  ch == 9 || ch == 11 || ch == 13 || ch == 15 ||
	  ch == 16 || ch == 18 || ch == 20 || ch == 22 ||
	  ch == 25 || ch == 27 || ch == 29 || ch == 31 ||
	  ch == 32 || ch == 34 || ch == 36 || ch == 38 ||
	  ch == 41 || ch == 43 || ch == 45 || ch == 47 ||
	  ch == 48 || ch == 50 || ch == 52 || ch == 54 ||
	  ch == 57 || ch == 59 || ch == 61 || ch == 63){
	mada_tot_energy += sipm_ch[ch];

      }
    }
    if (debug) std::cout << "mada_tot_energy" << mada_tot_energy << std::endl;

    tmerge->Fill();

  }
  fmerge->cd();
  tmerge->Write();
  std::cout << fmerge->GetName() << " created." << std::endl;
  fmerge->Close();

}

int main(int argc, char* argv[])
{
//void merge2017(int runnumber, int offset = 0, bool debug = 0){

  if(argc<2){
    std::cout << std::endl
    << "Usage: " << argv[0] << " <run_number> <run_type> [offset] [debug]" << std::endl
    << std::endl;
    std::cout << "run_type = data | pedestal"<< std::endl;
    std::cout << "debug = 0 | 1"<< std::endl;
    std::cout << "if offset = 9999 -> read from file"<< std::endl;
    std::cout << "Use the following environment variable to change the program behaviour"<< std::endl;
    std::cout << "Input files are searched in:"<< std::endl;
    std::cout << "\t $DATADIR/MADA_ntuple"<< std::endl;
    std::cout << "\t $DATADIR/ntuple"<< std::endl;

    exit(1);
  }
  int run_number, offset=0;
  bool data=true, debug=true;
  if(argc>=2) run_number=atoi(argv[1]);
  if(argc>=3){
    TString input = argv[2];
    if(input.Contains("data")){ data = true;}
    else {
      if(input.Contains("ped")) {data = false;}
      else{
        std::cout << "run_type = data | pedestal"<< std::endl;
        exit(1);
      }
    }
  }
  if(argc>=4) offset=atoi(argv[3]);
  if(argc>=5) debug=atoi(argv[4]);

    merge2017(run_number, data, offset, debug);
    return 0;
}
