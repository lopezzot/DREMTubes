#include <iostream>
#include <fstream>
#include <stdlib.h>     /* atoi */
#include <TFile.h>
#include <TTree.h>
#include <TString.h>

void csv_dream(TString filename){

  TFile *fdream = TFile::Open(filename);
  if(fdream==0) abort();
  if(!fdream->IsOpen()) abort();
  TTree * tdream = (TTree*) fdream->Get("DREAM");
  filename.ReplaceAll(".root",".csv");

  ofstream outfile(filename.Data());

  outfile
  << "run_n,"
  << "dd_evn,"
  << "dd_time_us,"
  << "adc_muon,"
  << "adc_preshower,"
  << "adc_CET3,"
  << "adc_CET4,"
  << "dwc_x,"
  << "dwc_y,"
  << std::endl;

  UInt_t run_n;
  UInt_t dd_evn;
  UInt_t dd_time_s; // Time of event in Seconds
  UInt_t dd_time_us; // Time of event in MicroSeconds
  ULong64_t dd_time_corrected_us;
  UInt_t adc_muon;
  UInt_t adc_preshower;
  UInt_t adc_CET3;
  UInt_t adc_CET4;
  Float_t dwc_x, dwc_y;

  UInt_t adc_counts[32];
  Float_t X[32];
  Float_t Y[32];
  Int_t preshower_ch = 9;
  Int_t CET3_ch = 6;
  Int_t CET4_ch = 7;
  Int_t muon_ch = 11;

  tdream->SetBranchAddress("Nevtda", &dd_evn);
  tdream->SetBranchAddress("TimeEvu", &dd_time_us);
  tdream->SetBranchAddress("TimeEvs", &dd_time_s);
  tdream->SetBranchAddress("Nrunnumber", &run_n);
  tdream->SetBranchAddress("CHARGEADCN4", adc_counts);
  tdream->SetBranchAddress("X", X);
  tdream->SetBranchAddress("Y", Y);

  tdream->GetEntry(0);

  int dd_nentries = tdream->GetEntries();
  for(int dentry = 0; dentry<dd_nentries; dentry++){
    tdream->GetEntry(dentry);

    adc_muon = adc_counts[muon_ch];
    adc_preshower = adc_counts[preshower_ch];
    adc_CET3 = adc_counts[CET3_ch];
    adc_CET4 = adc_counts[CET4_ch];

    dwc_x = X[0];
    dwc_y = Y[0];

    dd_time_corrected_us = ((Long64_t)dd_time_s) * 1000000 + ((Long64_t)dd_time_us);

    outfile
    << run_n << ","
    << dd_evn << ","
    << dd_time_corrected_us << ","
    << adc_muon << ","
    << adc_preshower << ","
    << adc_CET3 << ","
    << adc_CET4 << ","
    << dwc_x << ","
    << dwc_y << ","
    << std::endl;
  }
  std::cout << "output in: " << filename << std::endl;
}

void csv_dream(Int_t run=12240){
  TString ntdir;
  if (getenv("NTUPLEDIR") == NULL)
    ntdir = "/home/dreamtest/storage/ntuple";
  else
    ntdir = Form("%s", getenv("NTUPLEDIR"));

  TString filename = Form("%s/datafile_ntup_run%d.root", ntdir.Data(), run);
  csv_dream(filename);
}

int main(int argc, char* argv[])
{

  if(argc<2){
    std::cout << std::endl
	<< "Usage: " << argv[0] << " <run_number>" << std::endl
	      << std::endl;
    std::cout << "Use the following environment variables to change the program behaviour"<< std::endl;
    std::cout << "Input binary files are searched in:"<< std::endl;
    std::cout << "\t $NTUPLEDIR/"<< std::endl;
    exit(1);
  }
    csv_dream(atoi(argv[1]));
    return 0;
}
