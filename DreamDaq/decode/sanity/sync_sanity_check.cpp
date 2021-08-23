#include <TTree.h>
#include <TString.h>
#include <TFile.h>
#include <TCanvas.h>
#include <iostream>


void sync_sanity_check(int run_n){
  TString datadir = "/home/dreamtest/storage/";
  if (getenv("DATADIR") != NULL)
    datadir = Form("%s", getenv("DATADIR"));

  TString merged_output_dir = datadir + "merged/";
  TString merged_output_filename = "merged_";
  merged_output_filename += run_n;
  merged_output_filename += ".root";

  TFile *fmerge = TFile::Open(merged_output_dir + merged_output_filename);
  if(fmerge==0) {
    std::cout << merged_output_dir + merged_output_filename << " not found!" << std::endl;
    return;
  }
  TTree* tmerge = (TTree*) fmerge->Get("DREAM_merged");

  TCanvas*c=new TCanvas();
  tmerge->Draw("adc_muon:mada_tot_energy","","colz");

  c->SaveAs(Form("sanity_check_%d.root",run_n));
  c->SaveAs(Form("sanity_check_%d.pdf",run_n));

}
