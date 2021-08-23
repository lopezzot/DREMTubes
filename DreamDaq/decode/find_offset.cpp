// std library includes

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

// root includes

#include <TTree.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TString.h>
#include <TFile.h>
#include <TMath.h>

#include "run_offset.h"


void find_offset(int runnumber, bool debug=false, int min_offset = 0, int max_offset = 3){

  // merged tree

  TString datadir = "/home/dreamtest/storage/";
  if (getenv("DATADIR") != NULL){
    datadir = Form("%s", getenv("DATADIR"));
  }
  TString daq_input_dir = datadir + "ntuple/";
  TString mada_dir = datadir + "MADA_ntuple/";

  TString filename;
  filename = daq_input_dir + "pedestal_ntup_run";
  filename += runnumber;
  filename += ".root";

  // trees to sync
  TFile *fdream = TFile::Open(filename);
  if (!fdream){
    std::cout << "Cannot find file " << filename << std::endl;
    abort();
  }

  filename = mada_dir + "RUN";
  filename += runnumber;
  filename += ".root";
  TFile *fmada = TFile::Open(filename);
  if (!fmada){
    std::cout << "Cannot find file " << filename << std::endl;
    abort();
  }


  TTree * tdream = (TTree*) fdream->Get("DREAM");
  TTree * tmada = (TTree*) fmada->Get("DREAM_SiPM");

  /* synced tree variables */

//dream daq
  UInt_t run_n;
  tdream->SetBranchAddress("Nrunnumber", &run_n);
  UInt_t dd_evn;
  tdream->SetBranchAddress("Nevtda", &dd_evn);


//mada daq
  ULong64_t mada_evn;
  tmada->SetBranchAddress("event", &mada_evn);
  Double_t sipm_ch[64];
  for(int ch=0;ch<64;ch++){tmada->SetBranchAddress(Form("ch%d",ch), &sipm_ch[ch]);}

  // histo
  std::map<int,TH1F*> hEne;
  //hEne.reserve(max_offset-min_offset+1);
  for(int offset=min_offset;offset<=max_offset;offset++){
    hEne[offset] = new TH1F(
      Form("hEne%d",offset),
      Form("Energy in ped events (offset = %d)",offset),
      100,0,2E5);
  }

  // other support variables
  tmada->GetEntry(0);
  tdream->GetEntry(0);

  Long64_t dd_nentries = tdream->GetEntries();
  Long64_t mada_nentries = tmada->GetEntries();

  std::cout << "Found " << dd_nentries << " events in the daq input file and " << mada_nentries << " in the mada input file" << std::endl;

  Long64_t mentry=0;
  Long64_t mada_evn_next = 0;
  Long64_t mentry_next = 0;
  std::vector<float> mada_ch;
  mada_ch.reserve(64);

  for(Long64_t dentry = 0; dentry<dd_nentries; dentry++){
    tdream->GetEntry(dentry);
    if((((Long64_t)dd_evn)+min_offset<0)) continue;

    if (debug) std::cout << "dd_evn = " << dd_evn << std::endl;
    mada_evn = mada_evn_next;
    mentry = mentry_next;
    if(mada_evn>=mada_nentries) {break;}

    for(int offset=min_offset;offset<=max_offset;offset++){
      Long64_t dd_evn_tmp = ((Long64_t)dd_evn)+offset;

      if (debug){ std::cout << "testing offset = " << offset << std::endl
      << "dd_evn_tmp = " << dd_evn_tmp << std::endl;}
      if (debug) std::cout << "mada_evn = " << mada_evn<< std::endl;

      while((((Long64_t)dd_evn_tmp)>((Long64_t)mada_evn)) & (mentry<mada_nentries)){
        if (debug) std::cout << "mada_evn = " << mada_evn << " -> mentry++ " << std::endl;
        mentry++;
        tmada->GetEntry(mentry);
        if(mada_evn<0.5)continue; // skip events with evn = 0
        if (debug) std::cout << "mada_evn = " << mada_evn << std::endl;
      }

      if(offset==min_offset){
        if (debug) std::cout << "mada_evn_next = " << mada_evn << std::endl;
        mada_evn_next = mada_evn;
        mentry_next = mentry;
      }

      if(mentry>=mada_nentries) {break;}

      if(dd_evn_tmp<mada_evn){
        if (debug) std::cout << "dd_evn_tmp < mada_evn " << " -> dd_evn++ " << std::endl;
        continue;
      }

      if(mada_evn!=dd_evn_tmp){
        std::cout <<"Something went wrong mada_evn != dd_evn_tmp"<< std::endl;
        abort();
      } else {
        if (debug) std::cout << "mada_evn == dd_evn_tmp -> merge"<< std::endl;
      }

      Double_t mada_tot_energy = 0;

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
        if (debug) std::cout << "mada_tot_energy = " << mada_tot_energy << std::endl;

        hEne[offset]->Fill(mada_tot_energy);
      }
  }

  Float_t min_rms = FLT_MAX;
  Float_t min_mu = FLT_MAX;
  Float_t max_frac = 0;
  Int_t good_offset_rms=-9999;
  Int_t good_offset_mu=-9999;
  Int_t good_offset_frac=-9999;
  for(int offset=min_offset;offset<=max_offset;offset++){
    //hEne[offset]->SaveAs(Form("test offset %d.root",offset));
    Float_t rms =hEne[offset]->GetRMS();
    Float_t mu =hEne[offset]->GetMean();
    Float_t frac = hEne[offset]->GetMaximum()/hEne[offset]->GetEntries();
    //if (debug)
    { std::cout << "offset = "<< offset
    <<" rms = " << rms
    <<" mu = " << mu
    <<" frac = " << frac
    <<" entries " << hEne[offset]->GetEntries()
    << std::endl;}
    if(min_rms>rms) {
      min_rms= rms;
      good_offset_rms = offset;
    }
    if(min_mu>mu) {
      min_mu= mu;
      good_offset_mu = offset;
    }
    if(max_frac<frac) {
      max_frac = frac;
      good_offset_frac = offset;
    }
  }
  std::cout << "best offset rms = "<< good_offset_rms << std::endl;
  std::cout << "best offset mu  = "<< good_offset_mu << std::endl;
  std::cout << "best offset frac  = "<< good_offset_frac << std::endl;

  if((good_offset_rms == good_offset_mu) &(good_offset_mu == good_offset_frac)){
    run_offset(runnumber,true,good_offset_rms);
  }

}

int main(int argc, char* argv[])
{
//void merge2017(int runnumber, int offset = 0, bool debug = 0){

  if(argc<2){
    std::cout << std::endl
    << "Usage: " << argv[0] << " <run_number> <run_type> [debug] [min_offset] [max_offset]" << std::endl;
    std::cout << "debug = 0 | 1"<< std::endl;
    std::cout << "Use the following environment variables to change the program behaviour"<< std::endl;
    std::cout << "Input files are searched in:"<< std::endl;
    std::cout << "\t $DATADIR/MADA_ntuple"<< std::endl;
    std::cout << "\t $DATADIR/ntuple"<< std::endl;

    exit(1);
  }
  int run_number=atoi(argv[1]);
  bool debug=true;
  int min_offset = 0, max_offset = 3;
  if(argc>=3) debug=atoi(argv[2]);
  if(argc>=4) min_offset=atoi(argv[3]);
  if(argc>=5) max_offset=atoi(argv[4]);

    find_offset(run_number, debug, min_offset,max_offset);
    return 0;
}
