//**************************************************
// \file edisplay.C
// \brief: extract G4 e+ shower shape info
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 18 November 2021
//**************************************************

#ifndef edisplay_H
#define edisplay_H

double* PMTmap(const int& index){
    
    static double PMTpos[2];
    if (index==0) {PMTpos[0]=52.5; PMTpos[1]=52.5;}
    if (index==1) {PMTpos[0]=87.5; PMTpos[1]=17.5;}
    if (index==2) {PMTpos[0]=52.5; PMTpos[1]=17.5;}
    if (index==3) {PMTpos[0]=17.5; PMTpos[1]=17.5;}
    if (index==4) {PMTpos[0]=87.5; PMTpos[1]=52.5;}
    if (index==5) {PMTpos[0]=17.5; PMTpos[1]=52.5;}
    if (index==6) {PMTpos[0]=87.5; PMTpos[1]=87.5;}
    if (index==7) {PMTpos[0]=52.5; PMTpos[1]=87.5;}
    if (index==8) {PMTpos[0]=17.5; PMTpos[1]=87.5;}
    
    return PMTpos;
};

double* SSiPMmap(const int& index) {
    
    static double SSiPMpos[2];
    int row = index / 10;
    int column = (index - 10*row);
    SSiPMpos[0] = 1.0+2.0*row;
    SSiPMpos[1] = 1.0+2.*1.73*column;
    return SSiPMpos;
};

double* CSiPMmap(const int& index){
    
    static double CSiPMpos[2];
    int row = index / 10;
    int column = (index - 10*row);
    CSiPMpos[0] = 2.+2.0*row;
    CSiPMpos[1] = 1.73+2.*1.73*column;
    
    return CSiPMpos;
};

double* GetSbar(const vector<double>& svec) {
    
    static double Sbar[2];
    double x = 0;
    for (unsigned int index=0; index<160; index++){
        x+=svec.at(index)*SSiPMmap(index)[0];
    }
    x = x/std::accumulate(svec.begin(), svec.end(), 0.);

    double y = 0;
    for (unsigned int index=0; index<160; index++){
        y+=svec.at(index)*SSiPMmap(index)[1];
    }
    y = y/std::accumulate(svec.begin(), svec.end(), 0.);
    Sbar[0] = x;
    Sbar[1] = y;
    
    return Sbar;
}

double* GetCbar(const vector<double>& cvec){
    
    static double Cbar[2];
    double x = 0;
    for (unsigned int index=0; index<160; index++){
        x+=cvec.at(index)*CSiPMmap(index)[0];
    }
    x = x/std::accumulate(cvec.begin(), cvec.end(), 0.);

    double y = 0;
    for (unsigned int index=0; index<160; index++){
        y+=cvec.at(index)*CSiPMmap(index)[1];
    }
    y = y/std::accumulate(cvec.begin(), cvec.end(), 0.);
    Cbar[0] = x;
    Cbar[1] = y;
    
    return Cbar;
}

double Getdist(double pos[2], double bar[2]){
    
    double radius;
    radius = std::sqrt(pow(pos[0]-bar[0],2.)+pow(pos[1]-bar[1],2.));

    return radius;
}

void eradius( const double& energy, const string& file, const string& file2 ){

    cout<<"DREMTubes analysis:"<<endl;
    cout<<"---> Analysis e+ at energy (GeV) "<<energy<<endl;  
        
    //string filename = "run7_highstat/"+file;
    string filename = file+file2;
    TFile* infile = TFile::Open( filename.c_str(), "READ" );
    TTree* tree = (TTree*)infile->Get( "DREMTubesout" );

    TRandom3* rndm = new TRandom3();
    rndm->SetSeed(1.0);
   
    string outfilename = file+"_"+std::to_string((int)energy)+".root";
    auto outfile = new TFile(outfilename.c_str(), "RECREATE");

    int pdg; tree->SetBranchAddress( "PrimaryPDGID", &pdg );
    double venergy; tree->SetBranchAddress( "PrimaryParticleEnergy", &venergy );
    double lenergy; tree->SetBranchAddress( "EscapedEnergy", &lenergy );
    double edep; tree->SetBranchAddress( "EnergyTot", &edep );
    double Stot; tree->SetBranchAddress( "NofScinDet", &Stot );
    double Ctot; tree->SetBranchAddress( "NofCherDet", &Ctot );
    double PSdep; tree->SetBranchAddress( "PSEnergy", &PSdep );
    vector<double>* TowerE = NULL; 
    tree->SetBranchAddress( "VecTowerE", &TowerE );
    vector<double>* SPMT = NULL; 
    tree->SetBranchAddress( "VecSPMT", &SPMT );
    vector<double>* CPMT = NULL; 
    tree->SetBranchAddress( "VecCPMT", &CPMT );
    vector<double>* SSiPM = NULL; 
    tree->SetBranchAddress( "VectorSignals", &SSiPM );
    vector<double>* CSiPM = NULL; 
    tree->SetBranchAddress( "VectorSignalsCher", &CSiPM );

    const int points = 30;
    double radialprof[points] = {};
    double radialprofer[points] = {};
    TH1F radprofh1[points]; for (auto& n : radprofh1){
        n.SetTitle("scinprof");
        n.SetName("scinprof");
        n.SetBins(1000,0.,1.0);
    }
    double radii[points] = {};
    double radiier[points]; std::fill(radiier, radiier+points, 1.); 

    double cradialprof[points] = {};
    double cradialprofer[points] = {};
    TH1F cradprofh1[points]; for (auto& n : cradprofh1){
        n.SetTitle("cherprof");
        n.SetName("cherprof");
        n.SetBins(1000,0.,1.0);
    }

    int entries = tree->GetEntries();
    int cutentries = 0;
    int pitch = 1;
    double totS = 0.;
    double totC = 0.;
    double center[2] = {11.19,17.35};
    double maxdist=5.;

    auto Sbarh2 = new TH2F("Sbar","Sbar",2000,-100.,100.,2000,-100.,100.);
    auto Cbarh2 = new TH2F("Cbar","Cbar",2000,-100.,100.,2000,-100.,100.);
    auto Slateralh2 = new TH2F("Slateral","Slateral",30,0.,30.,1000,0.,0.5);
    auto Clateralh2 = new TH2F("Clateral","Clateral",30,0.,30.,1000,0.,0.5);

    for (unsigned int entry=0; entry<entries; entry++){
        tree->GetEntry(entry);
        if (PSdep<20.){
            auto sbar = GetSbar(*SSiPM);
            auto cbar = GetCbar(*CSiPM);
            Sbarh2->Fill(sbar[0],sbar[1]);
            Cbarh2->Fill(cbar[0],cbar[1]);
            if (Getdist(center, sbar)<maxdist) {
                cutentries += 1; 
                
                totS = std::accumulate(SSiPM->begin(), SSiPM->end(), 0.);
                totC = std::accumulate(CSiPM->begin(), CSiPM->end(), 0.);
                cout<<totS<<" "<<totC<<endl;
                for (unsigned int index=0; index<160; index++){
                    auto r = Getdist( SSiPMmap(index), sbar );
                    auto cr= Getdist( CSiPMmap(index), cbar );
                    
                    Slateralh2->Fill(r, SSiPM->at(index)/totS);
                    Clateralh2->Fill(cr, (CSiPM->at(index)+rndm->Gaus(0.,1.))/totC);

                    /*
                    int newindex = std::round(r/pitch);
                    int cnewindex = std::round(cr/pitch);
                    if (newindex <= points){
                        radialprof[newindex] += SSiPM->at(index)/totS;
                        fibers[newindex] += 1;
                    }
                    if (cnewindex <= points){
                        cradialprof[cnewindex] += CSiPM->at(index)/totC;
                        cfibers[cnewindex] += 1;
                    }*/
                }
                //for(unsigned int i=0; i<points; i++){radprofh1[i].Fill(radialprof[i]);}
                //for(unsigned int i=0; i<points; i++){radialprof[i]=0.;}
                //for(unsigned int i=0; i<points; i++){cradprofh1[i].Fill(cradialprof[i]);}
                //for(unsigned int i=0; i<points; i++){cradialprof[i]=0.;}
            }
        }
    }
    cout<<"Entries: "<<entries<<" used: "<<cutentries<<endl;
    Sbarh2->Write();
    Cbarh2->Write();
    Slateralh2->GetXaxis()->SetTitle("Distance from shower axis [mm]");
    Slateralh2->GetYaxis()->SetTitle("Percentage of total S SiPM signal in fiber");
    Clateralh2->GetXaxis()->SetTitle("Distance from shower axis [mm]");
    Clateralh2->GetYaxis()->SetTitle("Percentage of total C SiPM signal in fiber");
    Slateralh2->Write();
    Clateralh2->Write();
    auto sprof = Slateralh2->ProfileX();
    auto cprof = Clateralh2->ProfileX();
    sprof->SetTitle("lateralprof");
    sprof->GetXaxis()->SetTitle("Distance from shower axis [mm]");
    sprof->GetYaxis()->SetTitle("Percentage of total SiPM signal in fiber");
    sprof->SetName("lateralprof");
    sprof->SetMarkerStyle(21);
    sprof->SetMarkerColor(2);
    sprof->SetLineColor(2);
    sprof->Write();
    cprof->SetTitle("cherlateralprof");
    cprof->SetName("cherlateralprof");
    cprof->GetXaxis()->SetTitle("Distance from shower axis [mm]");
    cprof->GetYaxis()->SetTitle("Percentage of total SiPM signal in fiber");
    cprof->SetMarkerStyle(29);
    cprof->SetMarkerColor(4);
    cprof->SetLineColor(4);
    cprof->Write();

    auto tbfile = new TFile("../../../TBDataAnalysis/202108_SPS/Out_694_20GeV.root","READ");
    TProfile* slat; tbfile->GetObject("lateralprof",slat);
    TProfile* clat; tbfile->GetObject("cherlateralprof",clat);
    auto C1laterals = new TCanvas("", "", 600, 600);
    sprof->SetTitle("");
    sprof->SetStats(0.);
    sprof->Draw("P");
    cprof->Draw("same P");
    slat->Draw("same P");
    clat->Draw("same P");
    auto C1lateralslegend = new TLegend(1.-0.18,0.7,1.-0.61,0.89);
    C1lateralslegend->AddEntry(slat,"Scintillation, 20 GeV e+ (CERN-SPS TB Run 694)","ep");
    C1lateralslegend->AddEntry(clat,"Cherenkov, 20 GeV e+ (CERN-SPS TB Run 694)","ep");
    C1lateralslegend->AddEntry(sprof,"Scintillation, GEANT4.10.7.p01 - FTFP_BERT","ep");
    C1lateralslegend->AddEntry(cprof,"Cherenkov, GEANT4.10.7.p01 - FTFP_BERT","ep");
    C1lateralslegend->Draw("same");
    C1laterals->SetLeftMargin(0.15);
    outfile->cd();
    C1laterals->Write();
    outfile->Close();
    /*
    for (unsigned int i=0; i<points; i++){
        radprofh1[i].Write();
        radialprof[i] = radprofh1[i].GetMean();
        radialprofer[i] = radprofh1[i].GetMeanError();
        fibers[i] = fibers[i]/cutentries;
        cradialprof[i] = cradprofh1[i].GetMean();
        cradialprofer[i] = cradprofh1[i].GetMeanError();
        cfibers[i] = cfibers[i]/cutentries;
        radii[i] = pitch/2.+pitch*i;
    }

    cout<<"Fibers:"<<endl;
    for (auto& n : fibers){cout<<n<<endl;}

    for (unsigned int i=0; i<points; i++){
        lateralprof[i]=radialprof[i]/fibers[i];
        lateralprofer[i]=radialprofer[i]/fibers[i];
    }
    double counter = 0;
    int index = 0;
    for (auto& n : radialprof){
        counter += n;
        cumulativeprof[index] = counter;
        index += 1;
    } 

    for (unsigned int i=0; i<points; i++){
        clateralprof[i]=cradialprof[i]/cfibers[i];
        clateralprofer[i]=cradialprofer[i]/cfibers[i];
    }
    double ccounter = 0;
    int cindex = 0;
    for (auto& n : cradialprof){
        ccounter += n;
        ccumulativeprof[cindex] = ccounter;
        cindex += 1;
    } 

    cout<<"->Scintillation Lateral profile:"<<endl;
    for (auto& n : lateralprof){cout<<n<<endl;}
    cout<<"Radial profile:"<<endl;
    for (auto& n : radialprof){cout<<n<<endl;}
    cout<<"Cumulative profile:"<<endl;
    for (auto& n : cumulativeprof){cout<<n<<endl;}
    
    cout<<"->Cherenkov Lateral profile:"<<endl;
    for (auto& n : clateralprof){cout<<n<<endl;}
    cout<<"Cherenkov Radial profile:"<<endl;
    for (auto& n : cradialprof){cout<<n<<endl;}
    cout<<"Cherenkov Cumulative profile:"<<endl;
    for (auto& n : ccumulativeprof){cout<<n<<endl;}

    auto Gr1 = new TGraphErrors(points, radii, lateralprof, radiier, lateralprofer);
    Gr1->SetTitle("lateralprof");
    Gr1->SetName("lateralprof");
    Gr1->SetMarkerStyle(20);
    auto Gr2 = new TGraphErrors(points, radii, radialprof, radiier, radialprofer);
    Gr2->SetTitle("radialprof");
    Gr2->SetName("radialprof");
    Gr2->SetMarkerStyle(20);
    auto Gr3 = new TGraph(points, radii, cumulativeprof);
    Gr3->SetTitle("cumulativeprof");
    Gr3->SetName("cumulativeprof");
    Gr3->SetMarkerStyle(20);
    outfile->cd();
    Gr1->Write();
    Gr2->Write();
    Gr3->Write();

    auto CGr1 = new TGraphErrors(points, radii, clateralprof, radiier, clateralprofer);
    CGr1->SetTitle("cherlateralprof");
    CGr1->SetName("cherlateralprof");
    CGr1->SetMarkerStyle(29);
    auto CGr2 = new TGraphErrors(points, radii, cradialprof, radiier, cradialprofer);
    CGr2->SetTitle("cherradialprof");
    CGr2->SetName("cherradialprof");
    CGr2->SetMarkerStyle(29);
    auto CGr3 = new TGraph(points, radii, ccumulativeprof);
    CGr3->SetTitle("chercumulativeprof");
    CGr3->SetName("chercumulativeprof");
    CGr3->SetMarkerStyle(29);
    outfile->cd();
    CGr1->Write();
    CGr2->Write();
    CGr3->Write();
    */
};

void edisplay(){
    
    //20 GeV GEANT4 data
    //eradius( 20., "run7_1.1", "/DREMTubesout_Run1.root");
    //eradius( 20., "run7_1.3", "/DREMTubesout_Run1.root");
    //eradius( 20., "run7_1.4", "/DREMTubesout_Run1.root");
    //eradius( 20., "run7_1.5", "/DREMTubesout_Run1.root");
    //eradius( 20., "run7_1.6", "/DREMTubesout_Run1.root");
    //eradius( 20., "run7_1.7", "/DREMTubesout_Run1.root");

    //10 GeV GEANT4 data
    //eradius( 10., "run7_1.3", "/DREMTubesout_Run0.root"); 
    //eradius( 10., "run7_1.4", "/DREMTubesout_Run0.root"); 
    //eradius( 10., "run7_1.7", "/DREMTubesout_Run0.root"); 

    //30 GeV GEANT4 data
    //eradius( 30., "run7_1.1", "/DREMTubesout_Run2.root"); 
    eradius( 30., "run7_1.3", "/DREMTubesout_Run2.root"); 
    eradius( 30., "run7_1.7", "/DREMTubesout_Run2.root"); 
    
    //Get 1.4 and 1.7 degree TProfile
    auto tbfile1 = new TFile("run7_1.3_30.root","READ");
    TProfile* slat; tbfile1->GetObject("lateralprof",slat);
    TProfile* clat; tbfile1->GetObject("cherlateralprof",clat);
    auto tbfile2 = new TFile("run7_1.7_30.root","READ");
    TProfile* slat2; tbfile2->GetObject("lateralprof",slat2);
    TProfile* clat2; tbfile2->GetObject("cherlateralprof",clat2);

    Double_t arr1Y[30];
    Double_t arr2Y[30];
    for(int i=1; i<31; i++){
        arr1Y[i-1] = slat->GetBinContent(i);
        arr2Y[i-1] = slat2->GetBinContent(i);
    }
    for(auto &n : arr2Y)cout<<n<<endl;
    Double_t arrX[60];
    for(int i=0; i<30; i++) arrX[i]=0.5+i*1.0;
    for(int i=30; i<60; i++) arrX[i]=-0.5+(60-i)*1.0;
    for(auto &n : arrX )cout<<n<<endl;
    Double_t farrY[60];
    for(int i=0; i<30; i++){
        if (arr1Y[i]>arr2Y[i]){
            farrY[i]=arr1Y[i];
        }
        else {
            farrY[i]=arr2Y[i];
        }
    } 
    for(int i=0; i<30; i++){
        if(arr1Y[29-i]>arr2Y[29-i]){
            farrY[30+i]=arr2Y[29-i];
        }
        else{
            farrY[30+i]=arr1Y[29-i];
        }
    }

    Double_t arrC1Y[30];
    Double_t arrC2Y[30];
    for(int i=1; i<31; i++){
        arrC1Y[i-1] = clat->GetBinContent(i);
        arrC2Y[i-1] = clat2->GetBinContent(i);
    }
    Double_t arrCX[60];
    for(int i=0; i<30; i++) arrCX[i]=0.5+i*1.0;
    for(int i=30; i<60; i++) arrCX[i]=-0.5+(60-i)*1.0;
    Double_t fCarrY[60];
    for(int i=0; i<30; i++){
        if (arrC1Y[i]>arrC2Y[i]){
            fCarrY[i]=arrC1Y[i];
        }
        else {
            fCarrY[i]=arrC2Y[i];
        }
    } 
    for(int i=0; i<30; i++){
        if(arrC1Y[29-i]>arrC2Y[29-i]){
            fCarrY[30+i]=arrC2Y[29-i];
        }
        else{
            fCarrY[30+i]=arrC1Y[29-i];
        }
    }

    auto ban = new TGraph(60, arrX, farrY); 
    const double alpha = 0.5;
    const int red = 2;
    const int blue = 4;
    ban->SetFillColor(red);
    ban->SetLineStyle(1);
    ban->SetLineWidth(2);
    ban->SetLineColor(red);
    auto banfile = new TFile("band.root", "RECREATE");
    auto cban = new TGraph(60, arrCX, fCarrY);
    cban->SetFillColor(blue);
    cban->SetLineStyle(1);
    cban->SetLineWidth(2);
    cban->SetLineColor(blue);

    auto tbfile = new TFile("../../../TBDataAnalysis/202108_SPS/Out_671_30GeV.root","READ");
    TProfile* tbslat; tbfile->GetObject("lateralprof",tbslat);
    TProfile* tbclat; tbfile->GetObject("cherlateralprof",tbclat);

    auto C1 = new TCanvas("", "", 600, 600);
    C1->SetLeftMargin(0.15);
    ban->GetYaxis()->SetTickLength(0.01);
    ban->GetXaxis()->SetTickLength(0.01);
    ban->GetYaxis()->SetRangeUser(0.0,0.1);
    ban->GetXaxis()->SetRangeUser(0.0,25.0);
    ban->GetXaxis()->SetTitle("Distance from shower axis [mm]");
    ban->GetYaxis()->SetTitle("Percentage of total SiPM signal in fiber");
    ban->SetTitle("");
    ban->Draw("fAl");
    cban->Draw("same fl");
    tbslat->SetMarkerColor(1);
    tbslat->SetLineColor(1);
    tbclat->SetMarkerColor(1);
    tbclat->SetLineColor(1);
    tbslat->Draw("same P");
    tbclat->Draw("same P");
    auto legend = new TLegend(1.-0.18,0.7,1.-0.61,0.89);
    legend->AddEntry(tbslat,"Scintillation, 30 GeV e+ (CERN-SPS TB Run 671)","ep");
    legend->AddEntry(tbclat,"Cherenkov, 30 GeV e+ (CERN-SPS TB Run 671)","ep");
    legend->AddEntry(ban,"Scintillation, GEANT4.10.7.p01 - FTFP_BERT - 91.3-91.7 deg","f");
    legend->AddEntry(cban,"Cherenkov, GEANT4.10.7.p01 - FTFP_BERT - 91.3-91.7 deg","f");
    legend->Draw("same");
    banfile->cd();
    C1->Write();
    ban->Write();
    cban->Write();
    banfile->Close();
}

/*
void edisplay( const double& energy, const string& file ){

    cout<<"DREMTubes analysis:"<<endl;
    cout<<"---> Analysis at energy(GeV) "<<energy<<endl;  
        
    string filename = "run3/"+file;
    TFile* infile = TFile::Open( filename.c_str(), "READ" );
    TTree* tree = (TTree*)infile->Get( "DREMTubesout" );

    auto outfile = new TFile("edisplay.root", "RECREATE");

    int pdg; tree->SetBranchAddress( "PrimaryPDGID", &pdg );
    double venergy; tree->SetBranchAddress( "PrimaryParticleEnergy", &venergy );
    double lenergy; tree->SetBranchAddress( "EscapedEnergy", &lenergy );
    double edep; tree->SetBranchAddress( "EnergyTot", &edep );
    double Stot; tree->SetBranchAddress( "NofScinDet", &Stot );
    double Ctot; tree->SetBranchAddress( "NofCherDet", &Ctot );
    double PSdep; tree->SetBranchAddress( "PSEnergy", &PSdep );
    vector<double>* TowerE = NULL; 
    tree->SetBranchAddress( "VecTowerE", &TowerE );
    vector<double>* SPMT = NULL; 
    tree->SetBranchAddress( "VecSPMT", &SPMT );
    vector<double>* CPMT = NULL; 
    tree->SetBranchAddress( "VecCPMT", &CPMT );
    vector<double>* SSiPM = NULL; 
    tree->SetBranchAddress( "VectorSignals", &SSiPM );
    vector<double>* CSiPM = NULL; 
    tree->SetBranchAddress( "VectorSignalsCher", &CSiPM );

    for (unsigned int evt=0; evt<4; evt++){
        auto SPMTplot = new TH2F("SPMTplot", "SPMTplot", 3, 0., 96., 3, 0., 96.);
        auto CPMTplot = new TH2F("CPMTplot", "CPMTplot", 3, 0., 96., 3, 0., 96.);
    
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
        CSiPMplot->SetMarkerStyle(20);
        CSiPMplot->SetMarkerSize(2.0);
        CSiPMplot->GetXaxis()->SetTitle("X [mm]");
        CSiPMplot->GetYaxis()->SetTitle("Y [mm]");
        tree->GetEntry(evt);
        std::string SPMTname = "Sevt_"+std::to_string(evt);
        SPMTplot->SetName(SPMTname.c_str());
        std::string CPMTname = "Cevt_"+std::to_string(evt);
        CPMTplot->SetName(CPMTname.c_str());
        
        for (unsigned int index=0; index<160; index++){
            SSiPMplot->SetPoint(index, SSiPMmap(index)[0], SSiPMmap(index)[1], SSiPM->at(index));
            CSiPMplot->SetPoint(index, CSiPMmap(index)[0], CSiPMmap(index)[1], CSiPM->at(index));
        } 
        for (unsigned int index=0; index<9; index++){
            SPMTplot->Fill(PMTmap(index)[0], PMTmap(index)[1], SPMT->at(index));
            SPMTplot->Fill(PMTmap(0)[1], PMTmap(0)[0], std::accumulate(SSiPM->begin(), SSiPM->end(), 0));
            CPMTplot->Fill(PMTmap(index)[0], PMTmap(index)[1], CPMT->at(index));
            CPMTplot->Fill(PMTmap(0)[1], PMTmap(0)[0], std::accumulate(CSiPM->begin(), CSiPM->end(), 0));
        }

        outfile->cd(); 

        auto C1 = new TCanvas(("Event_"+std::to_string(evt)).c_str(), "", 2000, 2000);
        C1->SetTitle(("Event_"+std::to_string(evt)).c_str());
        gStyle->SetPalette(1);
        C1->Divide(2,2);
        C1->cd(1);
        SPMTplot->SetStats(0);
        SPMTplot->SetTitle(("S-PMT_Event_"+std::to_string(evt)+" [ph.e.]").c_str());
        SPMTplot->Draw("COLZ");
        C1->cd(2);
        gPad->SetTheta(90.);
        gPad->SetPhi(0.);
        SSiPMplot->GetXaxis()->SetTitleOffset(1.5);
        SSiPMplot->GetYaxis()->SetTitleOffset(-1.2);
        SSiPMplot->SetTitle(("S-SiPM_Event_"+std::to_string(evt)+"  [ph.e.]").c_str());
        SSiPMplot->Draw("PCOLZ FB");
        C1->cd(3);
        CPMTplot->SetStats(0);
        CPMTplot->SetTitle(("C-PMT_Event_"+std::to_string(evt)+" [ph.e.]").c_str());
        CPMTplot->Draw("COLZ");
        C1->cd(4);
        gPad->SetTheta(90.);
        gPad->SetPhi(0.);
        CSiPMplot->GetXaxis()->SetTitleOffset(1.5);
        CSiPMplot->GetYaxis()->SetTitleOffset(-1.2);
        CSiPMplot->SetTitle(("C-SiPM_Event_"+std::to_string(evt)+" [ph.e.]").c_str());
        CSiPMplot->Draw("PCOLZ FB");
        C1->Write();
        delete SPMTplot;
        delete SSiPMplot;
        delete CPMTplot;
        delete CSiPMplot;
        //SPMTplot->Write();
        //CPMTplot->Write();
        //SSiPMplot->Write();
        //CSiPMplot->Write();
    }

};
*/

#endif

//**************************************************
