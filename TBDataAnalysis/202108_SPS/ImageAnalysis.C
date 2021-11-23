//**************************************************
// \file ImageAnalysis.C
// \brief: analysis on imaging capabilities
//         of Event objects
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 30 August 2021
//**************************************************

// Usage: root -l .x ImageAnalysis.C

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

double* ScinSiPMmap(const int& index) {
    
    static double SSiPMpos[2];
    int row = index / 16;
    int column = (index - 16*row);
    SSiPMpos[0] = 1.0+2*column+96./3.;
    SSiPMpos[1] = 1.0+2.*1.72*row+96./3.;

    return SSiPMpos;
};

double* CherSiPMmap(const int& index){
    
    static double CSiPMpos[2];
    int row = index / 16;
    int column = (index - 16*row);
    CSiPMpos[0] = 2.+2.0*column+96./3.;
    CSiPMpos[1] = 1.72+2.*1.72*row+96./3.;
    
    return CSiPMpos;
};

double* GetScinbar(float svec[160]) {

    static double Sbar[2];
    double x = 0;
    for (unsigned int index=0; index<160; index++){
        x+=svec[index]*ScinSiPMmap(index)[0];
    }
    x = x/std::accumulate(svec, svec+160, 0.);

    double y = 0;
    for (unsigned int index=0; index<160; index++){
        y+=svec[index]*ScinSiPMmap(index)[1];
    }
    y = y/std::accumulate(svec, svec+160, 0.);
    Sbar[0] = x;
    Sbar[1] = y;
    
    return Sbar;
}


double* GetCherbar(const vector<double>& cvec){
    
    static double Cbar[2];
    double x = 0;
    for (unsigned int index=0; index<160; index++){
        x+=cvec.at(index)*CherSiPMmap(index)[0];
    }
    x = x/std::accumulate(cvec.begin(), cvec.end(), 0.);

    double y = 0;
    for (unsigned int index=0; index<160; index++){
        y+=cvec.at(index)*CherSiPMmap(index)[1];
    }
    y = y/std::accumulate(cvec.begin(), cvec.end(), 0.);
    Cbar[0] = x;
    Cbar[1] = y;
    
    return Cbar;
}

void ImageAnalysis(){

    std::string infile =
          "/Users/lorenzo/Desktop/tbntuples/recoNtuple/physics_sps2021_run670.root";
    std::cout << "Using file: " << infile << std::endl;
    char cinfile[infile.size() + 1];
    strcpy(cinfile, infile.c_str());

    auto file = new TFile(cinfile);
    auto *tree = (TTree *)file->Get("Ftree");
    auto evt = new EventOut();
    tree->SetBranchAddress("Events", &evt);
    auto outfile = new TFile("out.root", "RECREATE");

    //auto SPMTplot = new TH2F("SPMTplot", "SPMTplot", 3, 0., 96., 3, 0., 96.);
    //SPMTplot->SetDirectory(outfile);
    //auto CPMTplot = new TH2F("CPMTplot", "CPMTplot", 3, 0., 96., 3, 0., 96.);
    //CPMTplot->SetDirectory(outfile);
    
    auto PSh1 = new TH1F("PS","PS",300, 0., 3000.);
    auto MUh1 = new TH1F("Muon","Muon",300,100.,500.);
    auto C1h1 = new TH1F("C1","C1",500,0.,5000.);
    auto C2h1 = new TH1F("C2","C2",500,0.,5000.);
    auto barh2 = new TH2F("bar","bar",92,0.,92.,92,0.,92.);

    const int points = 13;
    double radialprof[points] = {};
    double fibers[points] = {};
    double lateralprof[points] = {};
    double cumulativeprof[points] = {};
    double radii[points] = {};
    
    double cradialprof[points] = {};
    double cfibers[points] = {};
    double clateralprof[points] = {};
    double ccumulativeprof[points] = {};

    int cutentries = 0;
    int pitch = 2;
    double totS = 0.;
    double totC = 0.;

    
    for (unsigned int i=0; i<tree->GetEntries(); i++){
        tree->GetEntry(i);
        C1h1->Fill(evt->C1);
        C2h1->Fill(evt->C2);
        totS=0.;
        totC=0.;

        if (evt->C1>90.){
            PSh1->Fill(evt->PShower);
            MUh1->Fill(evt->MCounter);
            if (evt->PShower<350.){ 

                auto sbar = GetScinbar(evt->SiPMPheS);
                auto cbar = GetScinbar(evt->SiPMPheC);
                if (47.<sbar[0] && sbar[0]<49. && 47.<sbar[1] && sbar[1]<49. &&
                    47.<cbar[0] && cbar[0]<49. && 47.<cbar[1] && cbar[1]<49.){
                    cutentries += 1; 
                    barh2->Fill(sbar[0],sbar[1]);
                    for (auto& n : evt->SiPMPheS) {totS+=n;}
                    for (auto& n : evt->SiPMPheC) {totC+=n;}

                    for (unsigned int index=0; index<160; index++){
                        auto r = Getdist( ScinSiPMmap(index), sbar );
                        auto cr = Getdist( CherSiPMmap(index), cbar );
                        int newindex = std::round(r/pitch);
                        int cnewindex = std::round(cr/pitch);
                        if (newindex <= points){
                            radialprof[newindex] += evt->SiPMPheS[index]/totS;
                            fibers[newindex] += 1;
                        }
                        if (cnewindex <= points){
                            cradialprof[cnewindex] += evt->SiPMPheC[index]/totC;
                            cfibers[cnewindex] += 1;
                        }
                    }
                }
            }
        }
    }
    cout<<"entries: "<<tree->GetEntries()<<" used: "<<cutentries<<endl;
    outfile->cd();
    PSh1->Write();
    MUh1->Write();
    C1h1->Write();
    C2h1->Write();
    barh2->Write();

    for (unsigned int i=0; i<points; i++){
        radialprof[i] = radialprof[i]/cutentries;
        fibers[i] = fibers[i]/cutentries;
        cradialprof[i] = cradialprof[i]/cutentries;
        cfibers[i] = cfibers[i]/cutentries;
        radii[i] = pitch/2.+pitch*i;
    }

    //cout<<"Fibers:"<<endl;
    //for (auto& n : fibers){cout<<n<<endl;}

    for (unsigned int i=0; i<points; i++){lateralprof[i]=radialprof[i]/fibers[i];}
    double counter = 0;
    int index = 0;
    for (auto& n : radialprof){
        counter += n;
        cumulativeprof[index] = counter;
        index += 1;
    } 
  
    for (unsigned int i=0; i<points; i++){clateralprof[i]=cradialprof[i]/cfibers[i];}
    double ccounter = 0;
    int cindex = 0;
    for (auto& n : cradialprof){
        ccounter += n;
        ccumulativeprof[cindex] = ccounter;
        cindex += 1;
    } 

    auto Gr1 = new TGraph(points, radii, lateralprof);
    Gr1->SetTitle("lateralprof");
    Gr1->SetName("lateralprof");
    Gr1->SetMarkerStyle(20);
    Gr1->Write();
    auto Gr2 = new TGraph(points, radii, radialprof);
    Gr2->SetTitle("radialprof");
    Gr2->SetName("radialprof");
    Gr2->SetMarkerStyle(20);
    Gr2->Write();
    auto Gr3 = new TGraph(points, radii, cumulativeprof);
    Gr3->SetTitle("cumulativeprof");
    Gr3->SetName("cumulativeprof");
    Gr3->SetMarkerStyle(20);
    Gr3->Write();

    auto CGr1 = new TGraph(points, radii, clateralprof);
    CGr1->SetTitle("cherlateralprof");
    CGr1->SetName("cherlateralprof");
    CGr1->SetMarkerStyle(29);
    CGr1->Write();
    auto CGr2 = new TGraph(points, radii, cradialprof);
    CGr2->SetTitle("cherradialprof");
    CGr2->SetName("cherradialprof");
    CGr2->SetMarkerStyle(29);
    CGr2->Write();
    auto CGr3 = new TGraph(points, radii, ccumulativeprof);
    CGr3->SetTitle("chercumulativeprof");
    CGr3->SetName("chercumulativeprof");
    CGr3->SetMarkerStyle(29);
    CGr3->Write();

    /*for (unsigned int i = 0; i < 100; i++) {
        tree->GetEntry(i);
        if (true) {
             
            auto SSiPMplot = new TGraph2D();
            SSiPMplot->SetName("SSiPM");
            SSiPMplot->SetTitle("SSiPM");
            SSiPMplot->SetMarkerStyle(20);
            SSiPMplot->SetMarkerSize(2.0);
            SSiPMplot->GetXaxis()->SetTitle("X [mm]");
            SSiPMplot->GetYaxis()->SetTitle("Y [mm]");

            auto CSiPMplot = new TGraph2D();
            CSiPMplot->SetName("CSiPM");
            CSiPMplot->SetTitle("CSiPM");
            CSiPMplot->SetMarkerStyle(29);
            CSiPMplot->SetMarkerSize(2.0);
            CSiPMplot->GetXaxis()->SetTitle("X [mm]");
            CSiPMplot->GetYaxis()->SetTitle("Y [mm]");
            
            for (unsigned int index=0; index<160; index++){
                SSiPMplot->SetPoint(index, ScinSiPMmap(index)[0], ScinSiPMmap(index)[1], evt->SiPMPheS[index]);
                CSiPMplot->SetPoint(index, CherSiPMmap(index)[0], CherSiPMmap(index)[1], evt->SiPMPheC[index]);
            } 
            
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
            SSiPMplot->Write();
            CSiPMplot->Write();

            delete CSiPMplot;
            delete SSiPMplot; 

        }
    }*/

    outfile->Close();

}

//**************************************************
