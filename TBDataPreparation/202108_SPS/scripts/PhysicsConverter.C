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

ClassImp(EventOut)

void PhysicsConverter(const string run){

  //Open merged ntuples
  //
  string infile = "/eos/user/i/ideadr/TB2021_H8/CERNDATA/v1.3/mergedNtuple/merged_sps2021_run"+run+".root";
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
  auto evout = new EventOut();
  ftree->Branch("Events",evout);
  //Create calibration objects
  //
  SiPMCalibration sipmCalibration("RunXXX_modified_v1.3.5.json");
  PMTCalibration pmtCalibration("RunXXX_modified_v1.3.5.json");
  DWCCalibration dwcCalibration("RunXXX_modified_v1.3.5.json");

  //Check entries in trees
  //
  std::cout<<"Entries in PMT / SiPM tree "<<PMTtree->GetEntries()<<" / "<<SiPMtree->GetEntries()<<std::endl;

  //Allocate branch pointers
  //
  int EventID;
  PMTtree->SetBranchAddress("EventNumber",&EventID);
  int ADCs[96];
  PMTtree->SetBranchAddress("ADCs",&ADCs);
  SiPMtree->SetBranchAddress("HG_Board0",&ev->SiPMHighGain[0]);
  SiPMtree->SetBranchAddress("HG_Board1",&ev->SiPMHighGain[64]);
  SiPMtree->SetBranchAddress("HG_Board2",&ev->SiPMHighGain[128]);
  SiPMtree->SetBranchAddress("HG_Board3",&ev->SiPMHighGain[192]);
  SiPMtree->SetBranchAddress("HG_Board4",&ev->SiPMHighGain[256]);
  SiPMtree->SetBranchAddress("LG_Board0",&ev->SiPMLowGain[0]);
  SiPMtree->SetBranchAddress("LG_Board1",&ev->SiPMLowGain[64]);
  SiPMtree->SetBranchAddress("LG_Board2",&ev->SiPMLowGain[128]);
  SiPMtree->SetBranchAddress("LG_Board3",&ev->SiPMLowGain[192]);
  SiPMtree->SetBranchAddress("LG_Board4",&ev->SiPMLowGain[256]);
  int TDCsval[48];
  PMTtree->SetBranchAddress("TDCsval",&TDCsval);

  //Loop over events 
  //
  for( unsigned int i=0; i<PMTtree->GetEntries(); i++){
    PMTtree->GetEntry(i);
    SiPMtree->GetEntry(i);
    evout->EventID = EventID;

    //Fill ev data members
    //
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
    evout->PShower = ADCs[16];
    evout->MCounter = ADCs[32];
    evout->C1 = ADCs[64];
    evout->C2 = ADCs[65];
    //
    ev->DWC1L=TDCsval[0];
    ev->DWC1R=TDCsval[1];
    ev->DWC1U=TDCsval[2];
    ev->DWC1D=TDCsval[3];
    ev->DWC2L=TDCsval[4];
    ev->DWC2R=TDCsval[5];
    ev->DWC2U=TDCsval[6];
    ev->DWC2D=TDCsval[7];

    //Calibrate SiPMs and PMTs
    //
    ev->calibrate(sipmCalibration, evout);
    ev->calibratePMT(pmtCalibration, evout);
    ev->calibrateDWC(dwcCalibration, evout);
    evout->CompSPMTene();
    evout->CompCPMTene();
    //std::cout<<ev->EventID<<" "<<ev->totSiPMPheS<<std::endl;
    //Write event in ftree
    //
    ftree->Fill();
    //Reset totSiPMPheC and totSiPMPheS to 0
    //
    evout->totSiPMCene = 0;
    evout->totSiPMSene = 0;
  }

  //Write and close Outfile
  //
  Mergfile->Close();
  ftree->Write();
  Outfile->Close();

}

//**************************************************
