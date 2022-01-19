//**************************************************
// \file PhysicsAnalysis.C
// \brief: template analysis of Event objects
//         example on how to perform an analysis
//         on physics_sps2021_*.root data
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 24 August 2021
//**************************************************

//Example to be used with run 669 - 40 GeV e+ in T0.
//Usage: root -l .x 'PhysicsAnalysis.C("669")'
//creates scatterplot of S-energy and C-energy, 
//using the EventOut class info.
//It is just a template for future analysis studies.

#include <TTree.h>
#include <TFile.h>
#include <TH2F.h>
#include <iostream>
#include <stdint.h>
#include <string>
#include <fstream>
#include "../../TBDataPreparation/202106_DESY/scripts/PhysicsEvent.h"

ClassImp(EventOut)

void PhysicsAnalysis(const string run){

  string infile = "/eos/user/i/ideadr/TB2021_Desy/recoNtuple/physics_desy2021_run"+run+".root";
  std::cout<<"Using file: "<<infile<<std::endl;
  char cinfile[infile.size() + 1];
  strcpy(cinfile, infile.c_str());

  auto file = new TFile(cinfile);
  auto *tree = (TTree*) file->Get("Ftree");
  auto evt = new EventOut();
  tree->SetBranchAddress("Events",&evt);
  
  float energyS = 0;
  float energyC = 0;
  TH2F * enesplot = new TH2F("splot", "splot", 200, 1., 20., 200, 1., 20.);
  enesplot->GetXaxis()->SetTitle("Scintillation (SiPM+PMT) - Energy (GeV)");
  enesplot->GetYaxis()->SetTitle("Cherenkov (SiPM+PMT) - Energy (GeV)");

  TH1F *E_S = new TH1F("E_S", "E_S", 200, 0., 20.);
  E_S->GetXaxis()->SetTitle("Scintillation (GeV)");

  TH1F *E_C = new TH1F("E_C", "E_C", 200, 0., 20.);
  E_C->GetXaxis()->SetTitle("Cherenkov (GeV)");

  TH1F *E_pmtS = new TH1F("E_pmtS", "E_pmtS", 200, 0., 20.);
  E_pmtS->GetXaxis()->SetTitle("Scintillation PMT (GeV)");

  TH1F *E_pmtC = new TH1F("E_pmtC", "E_pmtC", 200, 0., 20.);
  E_pmtC->GetXaxis()->SetTitle("Cherenkov PMT (GeV)");

  TH1F *E_SiPMS = new TH1F("E_SiPMS", "E_SiPMS", 200, 0., 20.);
  E_SiPMS->GetXaxis()->SetTitle("Scintillation SiPM (GeV)");

  TH1F *E_SiPMC = new TH1F("E_SiPMC", "E_SiPMC", 200, 0., 20.);
  E_SiPMC->GetXaxis()->SetTitle("Cherenkov SiPM (GeV)");


  for (unsigned int i=0; i<tree->GetEntries(); i++){
      tree->GetEntry(i);
      energyS = evt->totSiPMSene + evt->SPMTenergy;    //Energy unit is GeV
      energyC = evt->totSiPMCene + evt->CPMTenergy;
      enesplot->Fill(energyS, energyC);
      E_S->Fill(energyS); 
      E_C->Fill(energyC); 
      E_pmtS->Fill(evt->SPMTenergy);
      E_pmtC->Fill(evt->CPMTenergy);
      E_SiPMS->Fill(evt->totSiPMSene);
      E_SiPMC->Fill(evt->totSiPMCene);

  }

  enesplot->Draw("COLZ");

  TCanvas *energy = new TCanvas("energy","energy", 1000,700); 
  energy->Divide(2,2);
  energy->cd(1); 
  gPad->SetLogy();
  E_pmtS->Draw(); 
  energy->cd(2); 
  gPad->SetLogy();
  E_pmtC->Draw(); 
  energy->cd(3); 
  gPad->SetLogy();
  E_SiPMS->Draw(); 
  energy->cd(4); 
  gPad->SetLogy();
  E_SiPMC->Draw(); 


  TCanvas *Tot_energy = new TCanvas("Tot_energy","Tot_energy", 1000,700); 
  Tot_energy->Divide(2,1);
  Tot_energy->cd(1); 
  gPad->SetLogy();
  E_S->Draw(); 
  Tot_energy->cd(2); 
  gPad->SetLogy();
  E_C->Draw(); 

}

//**************************************************
