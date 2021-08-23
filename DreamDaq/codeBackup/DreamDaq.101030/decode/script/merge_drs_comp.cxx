#include <iostream>
#include <fstream>
#include <cmath>
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TF2.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"

# include <stdlib.h>
# include <string>
# include <cmath>
using namespace std;


void merge_drs(unsigned int runnumber,TString phys ){

  char ntdatafile_dream[256];
  char ntdatafile_drs[256];
  if (phys=="data") {sprintf(ntdatafile_dream,"/home/dreamtest/working/ntuple/datafile_ntup_run%d.root", runnumber);}
  if (phys=="pedestal")  {sprintf(ntdatafile_dream,"/home/dreamtest/working/ntuple/pedestal_ntup_run%d.root", runnumber);}


 sprintf(ntdatafile_drs,"/home/dreamtest/working/root/run_000%d.root", runnumber);



  TFile *file_drs = new TFile(ntdatafile_drs,"READ");
  if (!(file_drs->IsOpen())){
    printf(" Cannot open drs roople ");
    return;
  }//file open


  
  TFile *file_dream = new TFile(ntdatafile_dream,"UPDATE");
  if (!(file_dream->IsOpen())){
    printf(" Cannot open data/ped roople ");
    return;
  }//file open





  TTree *t_dream= (TTree *)file_dream->Get("DREAM");

  Short_t w_array0[4][1024];
  Short_t w_array1[4][1024];
  Short_t w_array2[4][1024];
  Short_t w_array3[4][1024];
  float temp[4]; 
  Short_t w_array0_drs[4][1024];
  Short_t w_array1_drs[4][1024];
  Short_t w_array2_drs[4][1024];
  Short_t w_array3_drs[4][1024];
  float temp_drs[4];
  Short_t w_array0_temp[4][1024];
  Short_t w_array1_temp[4][1024];
  Short_t w_array2_temp[4][1024];
  Short_t w_array3_temp[4][1024];
  float temp_temp[4];
  int nevt_drs_drs,nevt_daq_drs,spill_n_drs,trg_mask_drs;
  int nevt_drs,nevt_daq,spill_n,trg_mask;
  int nevt_drs_temp,nevt_daq_temp,spill_n_temp,trg_mask_temp;
  unsigned int Nevtda_dream;

  //    t->Branch("nevt",&nevt,"nevt/I");
  TBranch* n1= t_dream->Branch("nevt_drs",&nevt_drs,"nevt_drs/I");
  TBranch* n2= t_dream->Branch("nevt_daq",&nevt_daq,"nevt_daq/I");
  TBranch* n3= t_dream->Branch("spill_n",&spill_n,"spill_n/I");
  TBranch* n4= t_dream->Branch("trg_mask",&trg_mask,"trg_mask/I");
  TBranch* w0 = t_dream->Branch("w_array0",w_array0,"w_array0[4][1024]/S");
  TBranch* w1 = t_dream->Branch("w_array1",w_array1,"w_array1[4][1024]/S");
  TBranch* w2 = t_dream->Branch("w_array2",w_array2,"w_array2[4][1024]/S");
  TBranch* w3 = t_dream->Branch("w_array3",w_array3,"w_array3[4][1024]/S");
  TBranch*  t = t_dream->Branch("temp",temp,"temp[4]/f");
  t_dream->SetBranchAddress("Nevtda", &Nevtda_dream);
  Int_t nentries = (Int_t)t_dream->GetEntries();



  TTree *t_drs= (TTree *)file_drs->Get("DRS4");

  Int_t nentries_drs = (Int_t)t_drs->GetEntries();
  t_drs->SetBranchAddress("nevt_drs", &nevt_drs_drs);
  t_drs->SetBranchAddress("nevt_daq", &nevt_daq_drs); 
  t_drs->SetBranchAddress("spill_n", &spill_n_drs); 
  t_drs->SetBranchAddress("trg_mask", &trg_mask_drs);
  t_drs->SetBranchAddress("w_array0",w_array0_drs);
  t_drs->SetBranchAddress("w_array1",w_array1_drs);
  t_drs->SetBranchAddress("w_array2",w_array2_drs);
  t_drs->SetBranchAddress("w_array3",w_array3_drs);
  t_drs->SetBranchAddress("temp",temp_drs);
  Int_t drs_entries = (Int_t)t_drs->GetEntries();

  int Nevt_dream_temp_e=0;
  int nevt_drs_temp_e=0;
  bool beccato=false;
  bool passato=false;
  int i=0;

  int trg_e;

//    for (int d=0;d<1000;d++) {
  for (int d = 0; d < drs_entries; d++){
    beccato=false;
    passato=false;
    file_drs->cd();
    t_drs->GetEntry(d);
     trg_e=trg_mask_drs; 


    nevt_drs_temp_e=nevt_daq_drs;
    while((!beccato)&&(!passato)){
      file_dream->cd();
      t_dream->GetEntry(i);
      Nevt_dream_temp_e=Nevtda_dream;

      if (Nevt_dream_temp_e==nevt_drs_temp_e) {  
	beccato=true;
//     cout<<" beccato Nevt_drs "<<nevt_drs_temp_e <<" Nevt_dream "<<Nevt_dream_temp_e<<" "<<
//         trg_e<<endl;
//        if (trg_e!=1) cout<<" ERRORE "<<endl;
	file_drs->cd();
	t_drs->GetEntry(d);
	for (int ch=0;ch<4;ch++) {
	  for (int c=0; c<1024;c++) {
	    w_array0_temp[ch][c]=w_array0_drs[ch][c];
	    w_array1_temp[ch][c]=w_array1_drs[ch][c];
	    w_array2_temp[ch][c]=w_array2_drs[ch][c];
	    w_array3_temp[ch][c]=w_array3_drs[ch][c];
	  }//loop c
	  temp_temp[ch]=temp_drs[ch];
	} // loop ch
          nevt_drs_temp=nevt_drs_drs;
          nevt_daq_temp=nevt_daq_drs;
          spill_n_temp=spill_n_drs;
          trg_mask_temp=trg_mask_drs;
//        if (trg_mask_temp!=1) cout<<"ERORE 2"<<endl; 

	file_dream->cd();
	t_dream->GetEntry(i);
	for (int ch=0;ch<4;ch++) {
	  for (int c=0; c<1024;c++) {
	    w_array0[ch][c]=w_array0_temp[ch][c];
	    w_array1[ch][c]=w_array1_temp[ch][c];
	    w_array2[ch][c]=w_array2_temp[ch][c];
	    w_array3[ch][c]=w_array3_temp[ch][c];
	  }//loop c
	  temp[ch]=temp_temp[ch];
	}//loop ch
          nevt_drs=nevt_drs_temp;
          nevt_daq=nevt_daq_temp;
          spill_n=spill_n_temp;
          trg_mask=trg_mask_temp;
//         if (trg_mask!=1) cout<< "ERRORE 3"<<endl;

	w0->Fill();
	w1->Fill();
	w2->Fill();
	w3->Fill();
	t->Fill(); 
        n1->Fill();
        n2->Fill();
        n3->Fill(); 
        n4->Fill();
        i++;  

      }// if same event

      if ((Nevt_dream_temp_e>nevt_drs_temp_e)||(Nevt_dream_temp_e>=drs_entries)||(i>=nentries)) {passato=true;}
     
    }// while
   if (d%1000==0) {cout<<" Processed "<<d<<" events"<<endl;}
  }//loop drs events
  file_dream->cd();
  t_dream->Write("",TObject::kOverwrite);

  file_dream->Close();
  file_drs->cd();
  file_drs->Close(); 
}


int main(int argc, char * argv[]){


  if(argc < 3){
    cout << "Usage: " << argv[0] << " runnr data|pedestal" << endl;
    exit(1);
  }


  int runnr = atoi(argv[1]);

  

  merge_drs(runnr, TString(argv[2]) );

}
