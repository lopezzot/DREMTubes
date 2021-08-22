//**************************************************
// \file PhysicsAnalysis.C
// \brief: analysis of Event objects
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 20 August 2021
//**************************************************

#include <TTree.h>
#include <TFile.h>
#include <iostream>
#include <array>
#include <stdint.h>
#include <string>
#include "json.hpp"
#include <fstream>
#include "PhysicsEvent.h"

ClassImp(Event)

void PhysicsAnalysis(const string run){

  string infile = "physics_sps2021_run"+run+".root";
  std::cout<<"Using file: "<<infile<<std::endl;
  char cinfile[infile.size() + 1];
  strcpy(cinfile, infile.c_str());

  auto file = new TFile(cinfile);
  auto *tree = (TTree*) file->Get("Ftree");
  auto evt = new Event();
  tree->SetBranchAddress("Events",&evt);
  double ene = 0;
  for (unsigned int i=0; i<tree->GetEntries(); i++){
      tree->GetEntry(i);
      evt->CompSPMTene();
      evt->CompCPMTene();
      std::cout<<" SPMTenergy "<<evt->SPMTenergy<<" CPMTenergy "<<evt->CPMTenergy<<std::endl;
  }

}

//**************************************************
