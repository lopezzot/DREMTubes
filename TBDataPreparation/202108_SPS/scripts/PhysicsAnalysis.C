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

void PhysicsAnalysis(){

  auto file = new TFile("physics_sps2021_run646.root");
  auto *tree = (TTree*) file->Get("Ftree");
  auto evento = new Event();
  tree->SetBranchAddress("Events",&evento);
  for (unsigned int i=0; i<tree->GetEntries(); i++){
      tree->GetEntry(i);
      std::cout<<evento->EventID<<std::endl;
  }

}

//**************************************************
