//*************************************************
// \file ImageAnalysis.C
// \brief: analysis on imaging capabilities
//         of Event objects
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 30 August 2021
//**************************************************

// Usage: root -l .x ImageAnalysis.C

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

void DoAnalysis(const string RunNo, const double& mdist){

    //std::string infile = "/Users/lorenzo/Desktop/tbntuples/v1.3/physics_sps2021_run695.root";
    std::string infile = "/Users/lorenzopezzotti/Desktop/tbntuples/v1.3.1/physics_sps2021_run"+RunNo+".root";
    std::cout << "Using file: " << infile << std::endl;

    auto file = new TFile(infile.c_str());
    auto *tree = (TTree *)file->Get("Ftree");
    auto evt = new EventOut();
    tree->SetBranchAddress("Events", &evt);
    auto outfile = new TFile(("Out_"+RunNo+".root").c_str(), "RECREATE");

    auto PSh1 = new TH1F("PS","PS",300, 0., 3000.);
    auto MUh1 = new TH1F("Muon","Muon",300,100.,500.);
    auto C1h1 = new TH1F("C1","C1",500,0.,5000.);
    auto C2h1 = new TH1F("C2","C2",500,0.,5000.);
    auto DWC1h2 = new TH2F("DWC1","DWC1",2000,-32.,32.,2000,-32.,32.);
    auto DWC2h2 = new TH2F("DWC2","DWC2",2000,-32.,32.,2000,-32.,32.);
    auto Sbarh2 = new TH2F("Sbar","Sbar",6000,-92.,92.,6000,-92.,92.);
    auto Stoth1 = new TH1F("Stot","Stot",1000,0.,100.);
    auto Ctoth1 = new TH1F("Ctot","Ctot",1000,0.,100.);
    auto disth1 = new TH1F("dist","dist",1000,0.,100.);
    auto smaxh1 = new TH1F("Smax","Smax",100,0.,10.);

    const int points = 30;
    double radialprof[points] = {};
    double radialprofer[points] = {};
    TH1F radprofh1[points]; for (auto& n : radprofh1){
        n.SetTitle("scinprof");
        n.SetName("scinprof");
        n.SetBins(1000,0.,1.0);
    }
    double fibers[points] = {};
    TH1F fibersh1[points];
    for (auto& n : fibersh1){
        n.SetTitle("scinfibers");
        n.SetName("scinfibers");
        n.SetBins(100,0.,100.);
    }
    double lateralprof[points] = {};
    double lateralprofer[points] = {};
    double cumulativeprof[points] = {};
    double radii[points] = {};
    double radiier[points]; std::fill(radiier, radiier+points, 1.); 
    
    double cradialprof[points] = {};
    double cradialprofer[points] = {};
    TH1F cradprofh1[points]; for (auto& n : cradprofh1){
        n.SetTitle("cherprof");
        n.SetName("cherprof");
        n.SetBins(1000,0.,1.0);
    }
    double cfibers[points] = {};
    TH1F cfibersh1[points];
    for (auto& n : cfibersh1){
        n.SetTitle("cherfibers");
        n.SetName("cherfibers");
        n.SetBins(100,0.,100.);
    }
    double clateralprof[points] = {};
    double clateralprofer[points] = {};
    double ccumulativeprof[points] = {};

    int cutentries = 0;
    int pitch = 1.;
    double totS = 0.;
    double totC = 0.;
    double center[2] = {-2.897,3.235};
    double center2[2] = {-5.475,12.68};
    double scenter[2] = {22.36,18.34};
    double DWC1pos[2];
    double DWC2pos[2];
    double maxdist=mdist;

    auto Slateralh2 = new TH2F("Slateral","Slateral",35,0.,35.,1000,0.,0.5);
    auto Clateralh2 = new TH2F("Clateral","Clateral",35,0.,35.,1000,0.,0.5);

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
                
                DWC1pos[0]=evt->XDWC1;
                DWC1pos[1]=evt->YDWC1;
                DWC2pos[0]=evt->XDWC2;
                DWC2pos[1]=evt->YDWC2;
                auto sbar = GetScinbar(evt->SiPMPheS);
                auto cbar = GetCherbar(evt->SiPMPheC);
                if (Getdist(center, DWC1pos)<maxdist && Getdist(center2,DWC2pos)<maxdist
                    && Getdist(scenter, sbar)<maxdist ) {
                
                    DWC2h2->Fill(evt->XDWC2,evt->YDWC2);
                    DWC1h2->Fill(evt->XDWC1,evt->YDWC1);

                    Sbarh2->Fill(sbar[0],sbar[1]);
                    cutentries += 1; 
                    for (auto& n : evt->SiPMPheS) {totS+=n;}
                    for (auto& n : evt->SiPMPheC) {totC+=n;}
                    
                    smaxh1->Fill(*std::max_element(evt->SiPMPheS, evt->SiPMPheS+160)); 
                    Stoth1->Fill(totS);
                    Ctoth1->Fill(totC);

                    for (unsigned int index=0; index<160; index++){
                        auto r = Getdist( ScinSiPMmap(index), sbar );
                        disth1->Fill(r);
                        //cout<<"entry "<<i<<" Sipm "<<index<<" distance "<<r<<" round "<<(int)r/pitch<<endl;
                        auto cr = Getdist( CherSiPMmap(index), cbar );
                        int newindex = (int)r/pitch;
                        int cnewindex = (int)cr/pitch;
                        Slateralh2->Fill(r,evt->SiPMPheS[index]/totS);
                        Clateralh2->Fill(r,evt->SiPMPheC[index]/totC);
                        if (newindex < points){
                            radialprof[newindex] += evt->SiPMPheS[index]/totS;
                            fibers[newindex] += 1;
                        }
                        if (cnewindex < points){
                            cradialprof[cnewindex] += evt->SiPMPheC[index]/totC;
                            cfibers[cnewindex] += 1;
                        }
                    }
                    //cout<<"------------------------>entrys "<<i<<" scin: "<<radialprof[0]<<" "<<fibers[0]<<endl;
                    for(unsigned int i=0; i<points; i++){radprofh1[i].Fill(radialprof[i]);}
                    for(unsigned int i=0; i<points; i++){radialprof[i]=0.;}
                    for(unsigned int i=0; i<points; i++){fibersh1[i].Fill(fibers[i]);}
                    for(unsigned int i=0; i<points; i++){fibers[i]=0.;}
                    for(unsigned int i=0; i<points; i++){cfibersh1[i].Fill(cfibers[i]);}
                    for(unsigned int i=0; i<points; i++){cfibers[i]=0.;}
                    for(unsigned int i=0; i<points; i++){cradprofh1[i].Fill(cradialprof[i]);}
                    for(unsigned int i=0; i<points; i++){cradialprof[i]=0.;}
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
    Stoth1->Write();
    Ctoth1->Write();
    disth1->Write();
    DWC1h2->Write();
    DWC2h2->Write();
    Sbarh2->Write();
    smaxh1->Write();
    Slateralh2->Write();
    Clateralh2->Write();
    auto sprof = Slateralh2->ProfileX();
    auto cprof = Clateralh2->ProfileX();
    sprof->SetTitle("lateralprof");
    sprof->GetXaxis()->SetTitle("Distance from shower axis [mm]");
    sprof->GetYaxis()->SetTitle("Percentage of total SiPM signal in fiber");
    sprof->SetName("lateralprof");
    sprof->SetMarkerStyle(20);
    sprof->Write();
    cprof->SetTitle("cherlateralprof");
    cprof->SetName("cherlateralprof");
    cprof->GetXaxis()->SetTitle("Distance from shower axis [mm]");
    cprof->GetYaxis()->SetTitle("Percentage of total SiPM signal in fiber");
    cprof->SetMarkerStyle(29);
    cprof->Write();

    for (unsigned int i=0; i<points; i++){
        radprofh1[i].Write();
        cradprofh1[i].Write();
        fibersh1[i].Write();
        radialprof[i] = radprofh1[i].GetMean();
        radialprofer[i] = radprofh1[i].GetMeanError();
        fibers[i] = fibersh1[i].GetMean();
        cradialprof[i] = cradprofh1[i].GetMean();
        cradialprofer[i] = cradprofh1[i].GetMeanError();
        cfibers[i] = cfibersh1[i].GetMean();
        radii[i] = pitch/2.+pitch*i;
    }

    cout<<"Scin fibers:"<<endl; for (auto& n : fibers){cout<<n<<endl;}
    cout<<"Cher fibers:"<<endl; for (auto& n : cfibers){cout<<n<<endl;}

    double counter = 0;
    int index = 0;
    for (auto& n : radialprof){
        counter += n;
        cumulativeprof[index] = counter;
        index += 1;
    } 
  
    double ccounter = 0;
    int cindex = 0;
    for (auto& n : cradialprof){
        ccounter += n;
        ccumulativeprof[cindex] = ccounter;
        cindex += 1;
    } 

    auto Gr2 = new TGraphErrors(points, radii, radialprof, radiier,radialprofer);
    Gr2->SetTitle("radialprof");
    Gr2->SetName("radialprof");
    Gr2->SetMarkerStyle(20);
    Gr2->Write();
    auto Gr3 = new TGraph(points, radii, cumulativeprof);
    Gr3->SetTitle("cumulativeprof");
    Gr3->SetName("cumulativeprof");
    Gr3->SetMarkerStyle(20);
    Gr3->Write();

    auto CGr2 = new TGraphErrors(points, radii, cradialprof, radiier, cradialprofer);
    CGr2->SetTitle("cherradialprof");
    CGr2->SetName("cherradialprof");
    CGr2->SetMarkerStyle(29);
    CGr2->Write();
    auto CGr3 = new TGraph(points, radii, ccumulativeprof);
    CGr3->SetTitle("chercumulativeprof");
    CGr3->SetName("chercumulativeprof");
    CGr3->SetMarkerStyle(29);
    CGr3->Write();

    outfile->Close();

}

void ImageAnalysis(){

    double mdist = 5.0;

    //10 GeV -> 404 events selected
    DoAnalysis("657", mdist);
    auto file1 = new TFile("Out_657.root","READ"); 
    TGraphErrors* gr10s; file1->GetObject("lateralprof",gr10s);
    TGraphErrors* gr10c; file1->GetObject("cherlateralprof",gr10c);
    gr10s->SetMarkerColor(2); gr10s->SetLineColor(2);
    gr10c->SetMarkerColor(2); gr10c->SetLineColor(2); gr10c->SetMarkerStyle(22);

    //10 GeV -> 175 events selected
    //DoAnalysis("655");
    //20 GeV -> 339 events selected
    //DoAnalysis("670");
    //20 GeV -> 564 events selected
    
    DoAnalysis("694", mdist);
    auto file2 = new TFile("Out_694.root","READ"); 
    TGraphErrors* gr20s; file2->GetObject("lateralprof",gr20s);
    TGraphErrors* gr20c; file2->GetObject("cherlateralprof",gr20c);
    gr20s->SetMarkerColor(4); gr20s->SetLineColor(4);
    gr20c->SetMarkerColor(4); gr20c->SetLineColor(4); gr20c->SetMarkerStyle(22);
    //20 GeV no preshower -> 4818 events selected
    DoAnalysis("695", mdist);
    //30 GeV -> 93 events selected
    //DoAnalysis("693");
    //30 GeV -> 72 events selected
    //DoAnalysis("671");
    //40 GeV -> 106 events selected
    DoAnalysis("687", mdist);
    auto file3 = new TFile("Out_687.root","READ"); 
    TGraphErrors* gr40s; file3->GetObject("lateralprof",gr40s);
    TGraphErrors* gr40c; file3->GetObject("cherlateralprof",gr40c);
    gr40s->SetMarkerColor(8); gr40s->SetLineColor(8);
    gr40c->SetMarkerColor(8); gr40c->SetLineColor(8); gr40c->SetMarkerStyle(22);
    //10 GeV -> 175 events selected
    //40 GeV ->69 events selected
    //DoAnalysis("686");
    //6 GeV ->70 events selected
    //DoAnalysis("696");
    
    //Energy comparison convas
    //
    /*
    auto canvasfile = new TFile("Canvases.root", "RECREATE");
    canvasfile->cd();
    auto C1laterals = new TCanvas("", "", 600, 600);
    gr10s->GetHistogram()->SetMinimum(0.);
    gr10s->GetHistogram()->SetMaximum(0.09);
    gr10s->Draw("AP");
    gr20s->Draw("same P");
    gr40s->Draw("same P");
    gr10c->Draw("same P");
    gr20c->Draw("same P");
    gr40c->Draw("same P");
    auto C1lateralslegend = new TLegend(1.-0.18,0.7,1.-0.61,0.89);
    C1lateralslegend->AddEntry(gr10s,"10 GeV e+ Scintillation (Run 657)","ep");
    C1lateralslegend->AddEntry(gr20s,"20 GeV e+ Scintillation (Run 694)","ep");
    C1lateralslegend->AddEntry(gr40s,"40 GeV e+ Scintillation (Run 687)","ep");
    C1lateralslegend->AddEntry(gr10c,"10 GeV e+ Cherenkov (Run 657)","ep");
    C1lateralslegend->AddEntry(gr20c,"20 GeV e+ Cherenkov (Run 694)","ep");
    C1lateralslegend->AddEntry(gr40c,"40 GeV e+ Cherenkov (Run 687)","ep");
    C1lateralslegend->Draw("same");
    C1laterals->SetLeftMargin(0.15);
    C1laterals->Write();*/
}

//**************************************************
