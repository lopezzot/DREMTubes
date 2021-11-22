//**************************************************
// \file ImageAnalysis.C
// \brief: analysis on imaging capabilities
//         of Event objects
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 30 August 2021
//**************************************************

// To be used with run 669 - 40 GeV e+ in T0.
// Usage: root -l .x 'PhysicsAnalysis.C("669")'

#include "../../TBDataPreparation/202108_SPS/scripts/PhysicsEvent.h"
#include "../../DREMTubes/analysis/v1.4/edisplay.h" 
#include <TFile.h>
#include <TH2F.h>
#include <TTree.h>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string>
ClassImp(EventOut);

void ImageAnalysis(const string run){

    // std::string infile =
    // "/eos/user/i/ideadr/TB2021_H8/recoNtuple/physics_sps2021_run"+run+".root";
    std::string infile =
          "/Users/lorenzo/Desktop/tbntuples/recoNtuple/physics_sps2021_run669.root";
    std::cout << "Using file: " << infile << std::endl;
    char cinfile[infile.size() + 1];
    strcpy(cinfile, infile.c_str());

    auto file = new TFile(cinfile);
    auto *tree = (TTree *)file->Get("Ftree");
    auto evt = new EventOut();
    tree->SetBranchAddress("Events", &evt);
    auto outfile = new TFile("out.root", "RECREATE");

    auto SPMTplot = new TH2F("SPMTplot", "SPMTplot", 3, 0., 96., 3, 0., 96.);
    SPMTplot->SetDirectory(outfile);
    auto CPMTplot = new TH2F("CPMTplot", "CPMTplot", 3, 0., 96., 3, 0., 96.);
    CPMTplot->SetDirectory(outfile);

    for (unsigned int i = 0; i < 100; i++) {
        tree->GetEntry(i);
        if (true) {
        
            SPMTplot->Fill(PMTmap(1)[0], PMTmap(1)[1], evt->SPMT1);
            SPMTplot->Fill(PMTmap(2)[0], PMTmap(2)[1], evt->SPMT2);
            SPMTplot->Fill(PMTmap(3)[0], PMTmap(3)[1], evt->SPMT3);
            SPMTplot->Fill(PMTmap(4)[0], PMTmap(4)[1], evt->SPMT4);
            SPMTplot->Fill(PMTmap(5)[0], PMTmap(5)[1], evt->SPMT5);
            SPMTplot->Fill(PMTmap(6)[0], PMTmap(6)[1], evt->SPMT6);
            SPMTplot->Fill(PMTmap(7)[0], PMTmap(7)[1], evt->SPMT7);
            SPMTplot->Fill(PMTmap(8)[0], PMTmap(8)[1], evt->SPMT8);

            CPMTplot->Fill(PMTmap(1)[0], PMTmap(1)[1], evt->CPMT1);
            CPMTplot->Fill(PMTmap(2)[0], PMTmap(2)[1], evt->CPMT2);
            CPMTplot->Fill(PMTmap(3)[0], PMTmap(3)[1], evt->CPMT3);
            CPMTplot->Fill(PMTmap(4)[0], PMTmap(4)[1], evt->CPMT4);
            CPMTplot->Fill(PMTmap(5)[0], PMTmap(5)[1], evt->CPMT5);
            CPMTplot->Fill(PMTmap(6)[0], PMTmap(6)[1], evt->CPMT6);
            CPMTplot->Fill(PMTmap(7)[0], PMTmap(7)[1], evt->CPMT7);
            CPMTplot->Fill(PMTmap(8)[0], PMTmap(8)[1], evt->CPMT8);

            SPMTplot->Write("SPMTplot");
            SPMTplot->Reset();
            CPMTplot->Write("CPMTplot");
            CPMTplot->Reset();

        }
    }

    outfile->Close();

}

//**************************************************
