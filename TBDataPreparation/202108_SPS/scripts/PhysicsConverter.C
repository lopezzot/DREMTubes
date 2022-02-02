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
#include <stdlib.h>

using json = nlohmann::json;

ClassImp(PhysicsEvent)

void PhysicsConverter(const int run){

  //Open merge ntuples
  //
  // const string inputFileName = "/eos/user/i/ideadr/TB2021_H8/mergedNtuple/merged_sps2021_run"+std::to_string(run)+".root";
  const string inputFileName = "merged_sps2021_run"+std::to_string(run)+".root";
  std::cout<<"Using file: "<<inputFileName<<std::endl;
  const string outputFileName = "physics_sps2021_run"+std::to_string(run)+".root";
  auto mergedFile = new TFile(inputFileName.c_str(), "READ");
  if(!mergedFile->IsOpen()){
    std::cerr<<"Cannot open file: " + inputFileName;
    exit(EXIT_FAILURE);
  }

  auto *PMTtree = (TTree*) mergedFile->Get("CERNSPS2021");
  auto *SiPMtree = (TTree*) mergedFile->Get("SiPMSPS2021");
  //Create new tree and Event object
  //
  auto outputFile = new TFile(outputFileName.c_str(),"RECREATE");
  if(!outputFile->IsOpen()){
    std::cerr<<"Cannot open new file: " + inputFileName;
    exit(EXIT_FAILURE);
  }

  auto physicsTree = new TTree("physicsTree","physicsTree");
  physicsTree->SetDirectory(outputFile);
  auto daqEvent = new Event();
  auto physicsEvent = new PhysicsEvent();
  physicsTree->Branch("Events",physicsEvent);
  //Create calibration objects
  //
  SiPMCalibration sipmCalibration("RunXXX.json");
  PMTCalibration pmtCalibration("RunXXX.json");
  DWCCalibration dwcCalibration("RunXXX.json");

  //Check entries in trees
  //
  std::cout<<"Entries in PMT / SiPM tree "<<PMTtree->GetEntries()<<" / "<<SiPMtree->GetEntries()<<std::endl;
  if ( PMTtree->GetEntries() != SiPMtree->GetEntries() ){
    std::cout << "PMT and SiPM trees should contain same amount of entries!";
  }

  //Allocate branch pointers
  //
  int EventID;
  int ADCs[96];
  int TDCsval[48];
  PMTtree->SetBranchAddress("EventNumber",&EventID);
  PMTtree->SetBranchAddress("ADCs",&ADCs);
  SiPMtree->SetBranchAddress("HG_Board0",&daqEvent->SiPMHighGain[0]);
  SiPMtree->SetBranchAddress("HG_Board1",&daqEvent->SiPMHighGain[64]);
  SiPMtree->SetBranchAddress("HG_Board2",&daqEvent->SiPMHighGain[128]);
  SiPMtree->SetBranchAddress("HG_Board3",&daqEvent->SiPMHighGain[192]);
  SiPMtree->SetBranchAddress("HG_Board4",&daqEvent->SiPMHighGain[256]);
  SiPMtree->SetBranchAddress("LG_Board0",&daqEvent->SiPMLowGain[0]);
  SiPMtree->SetBranchAddress("LG_Board1",&daqEvent->SiPMLowGain[64]);
  SiPMtree->SetBranchAddress("LG_Board2",&daqEvent->SiPMLowGain[128]);
  SiPMtree->SetBranchAddress("LG_Board3",&daqEvent->SiPMLowGain[192]);
  SiPMtree->SetBranchAddress("LG_Board4",&daqEvent->SiPMLowGain[256]);
  PMTtree->SetBranchAddress("TDCsval",&TDCsval);

  //Loop over events
  //
  const int nEntries = PMTtree->GetEntries();
  for( int i=0; i<nEntries; i++){
    PMTtree->GetEntry(i);
    SiPMtree->GetEntry(i);

    physicsEvent->EventID = EventID;

    //Fill daqEvent data members
    //
    daqEvent->SPMT1 = ADCs[8];
    daqEvent->SPMT2 = ADCs[9];
    daqEvent->SPMT3 = ADCs[10];
    daqEvent->SPMT4 = ADCs[11];
    daqEvent->SPMT5 = ADCs[12];
    daqEvent->SPMT6 = ADCs[13];
    daqEvent->SPMT7 = ADCs[14];
    daqEvent->SPMT8 = ADCs[15];
    daqEvent->CPMT1 = ADCs[0];
    daqEvent->CPMT2 = ADCs[1];
    daqEvent->CPMT3 = ADCs[2];
    daqEvent->CPMT4 = ADCs[3];
    daqEvent->CPMT5 = ADCs[4];
    daqEvent->CPMT6 = ADCs[5];
    daqEvent->CPMT7 = ADCs[6];
    daqEvent->CPMT8 = ADCs[7];
    daqEvent->DWC1L = TDCsval[0];
    daqEvent->DWC1R = TDCsval[1];
    daqEvent->DWC1U = TDCsval[2];
    daqEvent->DWC1D = TDCsval[3];
    daqEvent->DWC2L = TDCsval[4];
    daqEvent->DWC2R = TDCsval[5];
    daqEvent->DWC2U = TDCsval[6];
    daqEvent->DWC2D = TDCsval[7];

    // Fille pysicsEvent data
    physicsEvent->PShower = ADCs[16];
    physicsEvent->MCounter = ADCs[32];
    physicsEvent->C1 = ADCs[64];
    physicsEvent->C2 = ADCs[65];

    //Calibrate SiPMs and PMTs
    //
    daqEvent->calibrateSiPM(sipmCalibration, physicsEvent);
    daqEvent->calibratePMT(pmtCalibration, physicsEvent);
    daqEvent->calibrateDWC(dwcCalibration, physicsEvent);

    //Write event in physicsTree
    //
    physicsTree->Fill();
    //Reset totSiPMPheC and totSiPMPheS to 0
    //
  }

  //Write and close outputFile
  //
  mergedFile->Close();
  if(mergedFile->IsOpen()){
    std::cout<<"Cannot close file!";
    exit(EXIT_FAILURE);
  }
  physicsTree->Write();
  outputFile->Close();
  if(outputFile->IsOpen()){
    std::cout<<"Cannot close file!";
    exit(EXIT_FAILURE);
  }
}
