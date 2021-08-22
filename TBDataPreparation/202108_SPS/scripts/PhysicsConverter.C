//**************************************************
// \file PhysicsConverter.C
// \brief: converter from merged trees to Event obj
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
//          Edoardo Proserpio (Uni Insubria)
// \start date: 20 August 2021
//**************************************************
//
////usage: root -l .x PhysicsConverter.C++
//
//
#include <TTree.h>
#include <TFile.h>
#include <iostream>
#include <array>
#include <stdint.h>
#include <string>
#include "json.hpp"
#include <fstream>
#include "PhysicsEvent.h"
#include <string>
#include <cstring>

using json = nlohmann::json;

ClassImp(Event)

void PhysicsConverter(const string run){

  //Open merge ntuples
  //
  string infile = "merged_sps2021_run"+run+".root";
  std::cout<<"Using file: "<<infile<<std::endl;
  char cinfile[infile.size() + 1];
  strcpy(cinfile, infile.c_str());
  string outfile = "physics_sps2021_run"+run+".root";
  char coutfile[outfile.size() + 1];
  strcpy(coutfile, outfile.c_str());
  auto Mergfile = new TFile(cinfile, "READ");
  auto *PMTtree = (TTree*) Mergfile->Get("CERNSPS2021");
  auto *SiPMtree = (TTree*) Mergfile->Get("SiPMSPS2021");
  //Create new tree and Event object
  //
  auto Outfile = new TFile(coutfile,"RECREATE");
  auto ftree = new TTree("Ftree","Ftree");
  ftree->SetDirectory(Outfile);
  auto ev = new Event();
  ftree->Branch("Events",ev);
  //Create calibration objects
  //
  SiPMCalibration sipmCalibration("RunXXX.json");
  PMTCalibration pmtCalibration("RunXXX.json");

  //Check entries in trees
  //
  std::cout<<"Entries in PMT / SiPM tree "<<PMTtree->GetEntries()<<" / "<<SiPMtree->GetEntries()<<std::endl;

  //Allocate branch pointers
  //
  int EventID;
  int ADCs[96];
  int HG_b0[64];
  int HG_b1[64];
  int HG_b2[64];
  int HG_b3[64];
  int HG_b4[64];
  int LG_b0[64];
  int LG_b1[64];
  int LG_b2[64];
  int LG_b3[64];
  int LG_b4[64];
  uint16_t HG_all[320];
  uint16_t LG_all[320];
  PMTtree->SetBranchAddress("EventNumber",&EventID);
  PMTtree->SetBranchAddress("ADCs",ADCs);
  SiPMtree->SetBranchAddress("HG_Board0",HG_b0);
  SiPMtree->SetBranchAddress("HG_Board1",HG_b1);
  SiPMtree->SetBranchAddress("HG_Board2",HG_b2);
  SiPMtree->SetBranchAddress("HG_Board3",HG_b3);
  SiPMtree->SetBranchAddress("HG_Board4",HG_b4);
  SiPMtree->SetBranchAddress("LG_Board0",HG_b0);
  SiPMtree->SetBranchAddress("LG_Board1",HG_b1);
  SiPMtree->SetBranchAddress("LG_Board2",HG_b2);
  SiPMtree->SetBranchAddress("LG_Board3",HG_b3);
  SiPMtree->SetBranchAddress("LG_Board4",HG_b4);

  //Loop over events 
  //
  for( unsigned int i=0; i<PMTtree->GetEntries(); i++){
    PMTtree->GetEntry(i);
    SiPMtree->GetEntry(i);

    //Fill HG_all and LG_all
    for(int i=0;i<64;++i){
      HG_all[i] = HG_b0[i];
      HG_all[i+64] = HG_b1[i];
      HG_all[i+64*2] = HG_b2[i];
      HG_all[i+64*3] = HG_b3[i];
      HG_all[i+64*4] = HG_b4[i];
      LG_all[i] = LG_b0[i];
      LG_all[i+64] = LG_b1[i];
      LG_all[i+64*2] = LG_b2[i];
      LG_all[i+64*3] = LG_b3[i];
      LG_all[i+64*4] = LG_b4[i];
    }

    //Fill ev data members
    //
    ev->EventID = EventID;
    ev->SPMT1 = ADCs[8];
    ev->SPMT2 = ADCs[9];
    ev->SPMT3 = ADCs[10];
    ev->SPMT4 = ADCs[11];
    ev->SPMT5 = ADCs[12];
    ev->SPMT6 = ADCs[13];
    ev->SPMT7 = ADCs[14];
    ev->SPMT8 = ADCs[15];
    ev->CPMT1 = ADCs[0];
    ev->CPMT2 = ADCs[1];
    ev->CPMT3 = ADCs[2];
    ev->CPMT4 = ADCs[3];
    ev->CPMT5 = ADCs[4];
    ev->CPMT6 = ADCs[5];
    ev->CPMT7 = ADCs[6];
    ev->CPMT8 = ADCs[7];
    ev->PShower = ADCs[16];
    ev->MCounter = ADCs[32];
    ev->C1 = ADCs[64];
    ev->C2 = ADCs[65];
    for(int j=0;j<320;++j){
      ev->SiPMHighGain[j] = HG_all[j];
      ev->SiPMLowGain[j] = LG_all[j];
    }
    //Calibrate SiPMs and PMTs
    //
    ev->calibrate(sipmCalibration);
    ev->calibratePMT(pmtCalibration);
    //Write event in ftree
    //
    ftree->Fill();
    //Reset totSiPMPheC and totSiPMPheS to 0
    //
    ev->totSiPMPheC = 0;
    ev->totSiPMPheS = 0;
  }

  //Write and close Outfile
  //
  Mergfile->Close();
  ftree->Write();
  Outfile->Close();

}

//**************************************************
