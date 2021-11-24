//**************************************************
// \file edisplay.h
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
    SSiPMpos[0] = 1.0+2.0*row+96./3.;
    SSiPMpos[1] = 1.0+2.*1.73*column+96./3.;
    //cout<<SSiPMpos[0]<<" "<<SSiPMpos[1]<<endl;
    return SSiPMpos;
};

double* CSiPMmap(const int& index){
    
    static double CSiPMpos[2];
    int row = index / 10;
    int column = (index - 10*row);
    CSiPMpos[0] = 2.+2.0*row+96./3.;
    CSiPMpos[1] = 1.73+2.*1.73*column+96./3.;
    
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

void eradius( const double& energy, const string& file ){

    cout<<"DREMTubes analysis:"<<endl;
    cout<<"---> Analysis e+ at energy (GeV) "<<energy<<endl;  
        
    string filename = "run5/"+file;
    TFile* infile = TFile::Open( filename.c_str(), "READ" );
    TTree* tree = (TTree*)infile->Get( "DREMTubesout" );

    auto outfile = new TFile("eradius.root", "RECREATE");

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

    const int points = 13;
    double radialprof[points] = {};
    double radialprofer[points] = {};
    TH1F radprofh1[points]; for (auto& n : radprofh1){n.SetBins(1000,0.,1.0);}
    double fibers[points] = {};
    double lateralprof[points] = {};
    double lateralprofer[points] = {};
    double cumulativeprof[points] = {};
    double radii[points] = {};
    double radiier[points]; std::fill(radiier, radiier+points, 1.); 

    double cradialprof[points] = {};
    double cradialprofer[points] = {};
    TH1F cradprofh1[points]; for (auto& n : cradprofh1){n.SetBins(1000,0.,1.0);}
    double cfibers[points] = {};
    double clateralprof[points] = {};
    double clateralprofer[points] = {};
    double ccumulativeprof[points] = {};

    int entries = tree->GetEntries();
    int cutentries = 0;
    int pitch = 2;
    double totS = 0.;
    double totC = 0.;
    double center[2] = {42.9,49.44};
    double maxdist=10.;

    auto barh2 = new TH2F("bar","bar",200,0.,96.,200,0.,96.);

    for (unsigned int entry=0; entry<entries; entry++){
        tree->GetEntry(entry);
        if (PSdep<10.){
            auto sbar = GetSbar(*SSiPM);
            auto cbar = GetCbar(*CSiPM);
            if (Getdist(center, sbar)<maxdist && Getdist(center, cbar)<maxdist) {
                cutentries += 1; 
                barh2->Fill(sbar[0],sbar[1]);
                totS = std::accumulate(SSiPM->begin(), SSiPM->end(), 0.);
                totC = std::accumulate(CSiPM->begin(), CSiPM->end(), 0.);
                for (unsigned int index=0; index<160; index++){
                    auto r = Getdist( SSiPMmap(index), sbar );
                    auto cr= Getdist( CSiPMmap(index), cbar );
                    int newindex = std::round(r/pitch);
                    int cnewindex = std::round(cr/pitch);
                    if (newindex <= points){
                        radialprof[newindex] += SSiPM->at(index)/totS;
                        fibers[newindex] += 1;
                    }
                    if (cnewindex <= points){
                        cradialprof[cnewindex] += CSiPM->at(index)/totC;
                        cfibers[cnewindex] += 1;
                    }
                }
                for(unsigned int i=0; i<points; i++){radprofh1[i].Fill(radialprof[i]);}
                for(unsigned int i=0; i<points; i++){radialprof[i]=0.;}
                for(unsigned int i=0; i<points; i++){cradprofh1[i].Fill(cradialprof[i]);}
                for(unsigned int i=0; i<points; i++){cradialprof[i]=0.;}
            }
        }
    }
    barh2->Write();
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
};
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
