//**************************************************
// \file ImageAnalysis.C
// \brief: analysis on imaging capabilities
// 		   of Event objects
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 30 August 2021
//**************************************************

//Example to be used with run 669 - 40 GeV e+ in T0.
//Usage: root -l .x 'PhysicsAnalysis.C("669")'

#include <TTree.h>
#include <TFile.h>
#include <TH2F.h>
#include <iostream>
#include <stdint.h>
#include <string>
#include <fstream>
#include "../../TBDataPreparation/202108_SPS/scripts/PhysicsEvent.h"

double* GetSSiPMPos(int index){
	double SSiPMPos[2];
	const int size = 160;
	const int raw = size / index;
	const int column = index - 20*raw;
	double x = 35.-1.+column*2.;
	double y = 35.+1+raw*1.5;
	SSiPMPos[0]=x;
	SSiPMPos[1]=y;
	return SSiPMPos;
}

ClassImp(EventOut)

void ImageAnalysis(const string run){

  string infile = "/eos/user/i/ideadr/TB2021_H8/recoNtuple/physics_sps2021_run"+run+".root";
  std::cout<<"Using file: "<<infile<<std::endl;
  char cinfile[infile.size() + 1];
  strcpy(cinfile, infile.c_str());

  auto file = new TFile(cinfile);
  auto *tree = (TTree*) file->Get("Ftree");
  auto evt = new EventOut();
  tree->SetBranchAddress("Events",&evt);
  auto outfile = new TFile("out.root","RECREATE");
  
  auto SPMTplot = new TH2F("SPMTplot", "SPMTplot", 3, 0., 105., 3, 0., 105.);
  SPMTplot->SetDirectory(outfile);
  auto CPMTplot = new TH2F("CPMTplot", "CPMTplot", 3, 0., 105., 3, 0., 105.);
  CPMTplot->SetDirectory(outfile);
  auto SSiPMplot = new TH2F("SSiPMplot", "SSiPMplot", 160*3, 0., 105., 160*3, 0., 105);
  SSiPMplot->SetDirectory(outfile);

  for (unsigned int i=0; i<100; i++){
    tree->GetEntry(i);
    if (evt->PShower>500){                               
      SPMTplot->Fill(17.5,17.5,evt->SPMT3);
	  	SPMTplot->Fill(52.5,17.5,evt->SPMT2);
      SPMTplot->Fill(87.5,17.5,evt->SPMT1);
      SPMTplot->Fill(17.5,52.5,evt->SPMT5);
	 		SPMTplot->Fill(87.5,52.5,evt->SPMT4);
      SPMTplot->Fill(17.5,87.5,evt->SPMT8);
			SPMTplot->Fill(52.5,87.5,evt->SPMT7);
      SPMTplot->Fill(87.5,87.5,evt->SPMT6);
     	CPMTplot->Fill(17.5,17.5,evt->CPMT3);
		  CPMTplot->Fill(52.5,17.5,evt->CPMT2);
      CPMTplot->Fill(87.5,17.5,evt->CPMT1);
      CPMTplot->Fill(17.5,52.5,evt->CPMT5);
		  CPMTplot->Fill(87.5,52.5,evt->CPMT4);
      CPMTplot->Fill(17.5,87.5,evt->CPMT8);
      CPMTplot->Fill(52.5,87.5,evt->CPMT7);
      CPMTplot->Fill(87.5,87.5,evt->CPMT6);
		
	  	SPMTplot->Write("SPMTplot");
	  	SPMTplot->Reset();
	  	CPMTplot->Write("CPMTplot");
	  	CPMTplot->Reset();

			for(unsigned int index=0; index<160; index++){
				double* pos = GetSSiPMPos(index);   
		  	
			}

    }
  }
  outfile->Close();
}

//**************************************************
