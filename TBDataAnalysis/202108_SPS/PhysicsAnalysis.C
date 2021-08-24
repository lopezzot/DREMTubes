//**************************************************
// \file PhysicsAnalysis.C
// \brief: template analysis of Event objects
//         example on how to perform an analysis
//         on physics_sps2021_*.root data
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 24 August 2021
//**************************************************

#include <TTree.h>
#include <TFile.h>
#include <TH2F.h>
#include <iostream>
#include <array>
#include <stdint.h>
#include <string>
#include <fstream>
#include "../../TBDataPreparation/202108_SPS/scripts/json.hpp"
#include "../../TBDataPreparation/202108_SPS/scripts/PhysicsEvent.h"

ClassImp(EventOut)

void PhysicsAnalysis(const string run){

  string infile = "physics_sps2021_run"+run+".root";
  std::cout<<"Using file: "<<infile<<std::endl;
  char cinfile[infile.size() + 1];
  strcpy(cinfile, infile.c_str());

  auto file = new TFile(cinfile);
  auto *tree = (TTree*) file->Get("Ftree");
  auto evt = new EventOut();
  tree->SetBranchAddress("Events",&evt);
  
  float energyS = 0;
  float energyC = 0;
  auto enesplot = new TH2F("splot", "splot", 100, 0., 100., 100, 0., 100.);

  for (unsigned int i=0; i<tree->GetEntries(); i++){
      tree->GetEntry(i);
      if (evt->PShower>1000){
      	  energyS = evt->totSiPMSene + evt->SPMTenergy;
      	  energyC = evt->totSiPMCene + evt->CPMTenergy;
      	  enesplot->Fill(energyS, energyC);
      }
  }

  enesplot->Draw("COLZ");

}

//**************************************************
