//*************************************************
// \file DesyImageAnalysis.C
// \brief: Checking Desy data quality
//         and imaging capabilities
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 4 February 2021
//**************************************************

// Usage: root -l .x DesyImageAnalysis.C

#include "../../TBDataPreparation/202108_SPS/scripts/PhysicsEvent.h"
#include <TFile.h>
#include <TH2F.h>
#include <TTree.h>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string>
#include <TVector2.h>
#include<algorithm>
#include <string.h>

ClassImp(EventOut);

double Getdist(double pos[2], double bar[2]){
    
    double radius;
    radius = std::sqrt(pow(pos[0]-bar[0],2.)+pow(pos[1]-bar[1],2.));

    return radius;
}

double* ScinSiPMmap(const int& index) {
    
    static double SSiPMpos[2];
    int row = index / 16;
    int column = (index - 16*row);
    SSiPMpos[0] = 1.0+2*column;
    SSiPMpos[1] = 1.0+2.*1.73*row;

    return SSiPMpos;
}

double* CherSiPMmap(const int& index){
    
    static double CSiPMpos[2];
    int row = index / 16;
    int column = (index - 16*row);
    CSiPMpos[0] = 2.+2.0*column;
    CSiPMpos[1] = 1.73+2.*1.73*row;
    
    return CSiPMpos;
}

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

double* GetCherbar(float cvec[160]){
    
    static double Cbar[2];
    double x = 0;
    for (unsigned int index=0; index<160; index++){
        x+=cvec[index]*CherSiPMmap(index)[0];
    }
    x = x/std::accumulate(cvec, cvec+160, 0.);

    double y = 0;
    for (unsigned int index=0; index<160; index++){
        y+=cvec[index]*CherSiPMmap(index)[1];
    }
    y = y/std::accumulate(cvec, cvec+160, 0.);
    Cbar[0] = x;
    Cbar[1] = y;
    
    return Cbar;
};

double invsmear(const double& Nfired){
    double phe = 0.;
    const double Ncells = 4440.;
    phe = -Ncells*std::log(1-(Nfired/Ncells));

    return phe;
};

void DoAnalysis(const string RunNo, const double& mdist, const float& beamene){

    std::string infile = "/Users/lorenzopezzotti/Desktop/tbntuples/Desy/physics_desy2021_run"+RunNo+".root";
    std::cout << "Using file: " << infile << std::endl;
    
    auto file = new TFile(infile.c_str());
    auto *tree = (TTree *)file->Get("Ftree");
    auto evt = new EventOut();
    tree->SetBranchAddress("Events", &evt);
    std::stringstream ene;
    ene << std::fixed << std::setprecision(1) << beamene;
    std::string newbeamene = ene.str();
    auto outfile = new TFile(("Out_"+RunNo+"_"+newbeamene+"GeV.root").c_str(), "RECREATE");

    auto Stoth1 = new TH1F("Stot","Stot",1000,0.,100.);
    auto Ctoth1 = new TH1F("Ctot","Ctot",1000,0.,100.);
    auto SPMTh1 = new TH1F("SPMT","SPMT",1000,0.,100.);
    auto SSiPMh1 = new TH1F("SSiPM","SSiPM",1000,0.,100.);
    auto SCtoth2 = new TH2F("SCtot", "SCtot", 1000, 0., 100., 1000, 0., 100.); 
    auto Sbarh2 = new TH2F("Sbar","Sbar",2000,-100.,100.,2000,-100.,100.);
    auto Cbarh2 = new TH2F("Cbar","Cbar",2000,-100.,100.,2000,-100.,100.);
    auto Slateralh2 = new TH2F("Slateral","Slateral",35,0.,35.,1000,0.,1.0);
    auto Clateralh2 = new TH2F("Clateral","Clateral",35,0.,35.,1000,0.,1.0);

    double center[2] = {4., 27.}; //(16.77, 14.82)(4.,27.)
    double chercenter[2] = {4., 27.};//(16.82, 16.3)(4.,27.)
    double maxdist = 5.;
    double totS = 0.;
    double totC = 0.;
    double cutentries = 0.;

    for (unsigned int i=0; i<tree->GetEntries(); i++){
        tree->GetEntry(i);
        if (evt->totSiPMSene > 0.2*beamene && evt->totSiPMCene > 0.2*beamene){
            auto sbar = GetScinbar(evt->SiPMPheS);
            auto cbar = GetCherbar(evt->SiPMPheC);
            Sbarh2->Fill(GetScinbar(evt->SiPMPheS)[0], GetScinbar(evt->SiPMPheS)[1]);
            Cbarh2->Fill(GetCherbar(evt->SiPMPheC)[0], GetCherbar(evt->SiPMPheC)[1]);
            if( Getdist(center, sbar)<maxdist && Getdist(chercenter, cbar)<maxdist){
                cutentries = cutentries+1.;
                Stoth1->Fill(evt->totSiPMSene + evt->SPMTenergy);
                SPMTh1->Fill(evt->SPMTenergy);
                SSiPMh1->Fill(evt->totSiPMSene);
                Ctoth1->Fill(evt->totSiPMCene + evt->CPMTenergy);
                SCtoth2->Fill(evt->totSiPMSene + evt->SPMTenergy, evt->totSiPMCene + evt->CPMTenergy);
                
                totS = 0.; totC = 0.;
                for (auto& n : evt->SiPMPheS) {totS+=n;}
                for (auto& n : evt->SiPMPheC) {totC+=n;}
                totC = totC-evt->SiPMPheC[8];

                for (unsigned int index=0; index<160; index++){
                    auto r = Getdist( ScinSiPMmap(index), sbar );
                    auto cr = Getdist( CherSiPMmap(index), cbar );
                    Slateralh2->Fill(r,evt->SiPMPheS[index]/totS);
                    if (index != 8){Clateralh2->Fill(cr,evt->SiPMPheC[index]/totC);}
                }

            }
        }
    }
    double ratio = cutentries/tree->GetEntries();
    std::cout<<newbeamene<<" entries "<<tree->GetEntries()<<" used "<<cutentries<<" ratio "<<ratio<<std::endl;
    Stoth1->Write();
    Ctoth1->Write();
    SCtoth2->Write();
    Sbarh2->Write();
    Cbarh2->Write();
    Slateralh2->Write();
    Clateralh2->Write();
    auto sprof = Slateralh2->ProfileX();
    auto cprof = Clateralh2->ProfileX();
    sprof->Write();
    cprof->Write();
    outfile->Close();

}

void DesyImageAnalysis(){

    double mdist = 5.0;
    //Without yellow filters + telescope
    //180, 6 GeV
    //DoAnalysis("180", mdist, 6);
    //132, 6 GeV
    //DoAnalysis("132", mdist, 6);
    //202, 5.8 GeV
    DoAnalysis("202", mdist, 5.8);
    //208, 5.4 GeV
    DoAnalysis("208", mdist, 5.4);
    //203, 5.4 GeV
    //DoAnalysis("203", mdist, 5.4);
    //191, 5 GeV
    DoAnalysis("191", mdist, 5.);
    //192, 4.6 GeV
    DoAnalysis("192", mdist, 4.6);
    //193, 4.2 GeV
    DoAnalysis("193", mdist, 4.2);
    //194, 3.8 GeV
    DoAnalysis("194", mdist, 3.8);
    //195, 3.4 GeV
    DoAnalysis("195", mdist, 3.4);
    //209, 3.2 GeV
    DoAnalysis("209", mdist, 3.2);
    //204, 3.2 GeV
    //DoAnalysis("204", mdist, 3.2);
    //196, 3.0 GeV
    DoAnalysis("196", mdist, 3.0);
    //207, 2.8 GeV
    DoAnalysis("207", mdist, 2.8);
    //197, 2.6 GeV
    DoAnalysis("197", mdist, 2.6);
    //198, 2.2 GeV
    DoAnalysis("198", mdist, 2.2);
    //199, 1.8 GeV
    DoAnalysis("199", mdist, 1.8);
    //200, 1.4 GeV
    DoAnalysis("200", mdist, 1.4);
    //201, 1.0 GeV
    DoAnalysis("201", mdist, 1.0);
    //205, 2.8 GeV
    //DoAnalysis("205", mdist, 2.8);
    //227, 3.0 GeV
    //DoAnalysis("227", mdist, 3.0);
    //228, 2.8 GeV
    //DoAnalysis("228", mdist, 2.8);
    //229, 3.2 GeV
    //DoAnalysis("229", mdist, 3.2);
    //230, 3.0 GeV
    //DoAnalysis("230", mdist, 3.0);
    /*
    //Without yellow filters
    //108, 6.0 GeV
    DoAnalysis("108", mdist, 6.0);
    //113, 5.0 GeV
    DoAnalysis("113", mdist, 5.0);
    //116, 4.0 GeV
    DoAnalysis("116", mdist, 4.0);
    //117, 3.0 GeV
    DoAnalysis("117", mdist, 3.0);
    //118, 2.0 GeV
    DoAnalysis("118", mdist, 2.0);
    //119, 1.0 GeV
    DoAnalysis("119", mdist, 1.0); 
    */
    /*
    //With yellow filters on Scintillation SiPM and PMT
    //73, 6.0 GeV
    DoAnalysis("73", mdist, 6.0); 
    //74, 5.0 GeV
    DoAnalysis("74", mdist, 5.0);
    //75, 4.0 GeV
    DoAnalysis("75", mdist, 4.0);
    //76, 3.0 GeV
    DoAnalysis("76", mdist, 3.0);
    //77, 2.0 GeV
    DoAnalysis("77", mdist, 2.0);
    //78, 1.0 GeV
    DoAnalysis("78", mdist, 1.0);
    */
}

//**************************************************
