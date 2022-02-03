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

double* SSiPMmap(const int& index){
    
    static double SSiPMpos[2];
    int row = index / 10;
    int column = (index - 10*row);
    SSiPMpos[0] = 1.0+2.0*row+96./3.;
    SSiPMpos[1] = 1.0+2.*1.72*column+96./3.;
    //cout<<SSiPMpos[0]<<" "<<SSiPMpos[1]<<endl;
    return SSiPMpos;
};

double* CSiPMmap(const int& index){
    
    static double CSiPMpos[2];
    int row = index / 10;
    int column = (index - 10*row);
    CSiPMpos[0] = 2.+2.0*row+96./3.;
    CSiPMpos[1] = 1.72+2.*1.72*column+96./3.;
    
    return CSiPMpos;
};

double* GetSbar(const vector<double>& svec){
    
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
    cout<<"---> Analysis e+ at energy(GeV) "<<energy<<endl;  
        
    string filename = "run3/"+file;
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

    double radialprof[15]={};
    double fibers[15]={};
    double lateralprof[15] = {};
    double cumulativeprof[15] = {};
    double radii[15] = {};
    
    double cradialprof[15]={};
    double cfibers[15]={};
    double clateralprof[15] = {};
    double ccumulativeprof[15] = {};

    int entries = tree->GetEntries();
    
    for (unsigned int entry=0; entry<entries; entry++){
        tree->GetEntry(entry);
        auto sbar = GetSbar(*SSiPM);
        auto cbar = GetCbar(*CSiPM);

        for (unsigned int index=0; index<160; index++){
            auto r = Getdist( SSiPMmap(index), sbar );
            auto cr= Getdist( CSiPMmap(index), cbar );
            int newindex = std::round(r/2.);
            int cnewindex = std::round(cr/2.);
            if (newindex <= 15){
                radialprof[newindex] += SSiPM->at(index);
                fibers[newindex] += 1;
            }
            if (cnewindex <= 15){
                cradialprof[cnewindex] += CSiPM->at(index);
                cfibers[cnewindex] += 1;
            }
        }
    }

    for (unsigned int i=0; i<15; i++){
        radialprof[i] = radialprof[i]/entries;
        fibers[i] = fibers[i]/entries;
        cradialprof[i] = cradialprof[i]/entries;
        cfibers[i] = cfibers[i]/entries;
        radii[i] = 1.+2.*i;
    }

    //cout<<"Fibers:"<<endl;
    //for (auto& n : fibers){cout<<n<<endl;}

    for (unsigned int i=0; i<15; i++){lateralprof[i]=radialprof[i]/fibers[i];}
    double counter = 0;
    int index = 0;
    for (auto& n : radialprof){
        counter += n;
        cumulativeprof[index] = counter;
        index += 1;
    } 

    for (unsigned int i=0; i<15; i++){clateralprof[i]=cradialprof[i]/cfibers[i];}
    double ccounter = 0;
    int cindex = 0;
    for (auto& n : cradialprof){
        ccounter += n;
        ccumulativeprof[cindex] = ccounter;
        cindex += 1;
    } 

    /*cout<<"Lateral profile:"<<endl;
    for (auto& n : lateralprof){cout<<n<<endl;}
    cout<<"Radial profile:"<<endl;
    for (auto& n : radialprof){cout<<n<<endl;}
    cout<<"Cumulative profile:"<<endl;
    for (auto& n : cumulativeprof){cout<<n<<endl;}*/
    
    cout<<"Cherenkov Lateral profile:"<<endl;
    for (auto& n : clateralprof){cout<<n<<endl;}
    cout<<"Cherenkov Radial profile:"<<endl;
    for (auto& n : cradialprof){cout<<n<<endl;}
    cout<<"Cherenkov Cumulative profile:"<<endl;
    for (auto& n : ccumulativeprof){cout<<n<<endl;}

    auto Gr1 = new TGraph(15, radii, lateralprof);
    Gr1->SetTitle("lateralprof");
    Gr1->SetName("lateralprof");
    Gr1->SetMarkerStyle(20);
    auto Gr2 = new TGraph(15, radii, radialprof);
    Gr2->SetTitle("radialprof");
    Gr2->SetName("radialprof");
    Gr2->SetMarkerStyle(20);
    auto Gr3 = new TGraph(15, radii, cumulativeprof);
    Gr3->SetTitle("cumulativeprof");
    Gr3->SetName("cumulativeprof");
    Gr3->SetMarkerStyle(20);
    outfile->cd();
    Gr1->Write();
    Gr2->Write();
    Gr3->Write();

};




















void edisplay( const double& energy, const string& file ){

    cout<<"DREMTubes analysis:"<<endl;
    cout<<"---> Analysis at energy(GeV) "<<energy<<endl;  
        
    string filename = "run3/"+file;
    TFile* infile = TFile::Open( filename.c_str(), "READ" );
    TTree* tree = (TTree*)infile->Get( "DREMTubesout" );

    auto outfile = new TFile("edisplay.root", "RECREATE");

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

    for (unsigned int evt=0; evt<5; evt++){
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
            CPMTplot->Fill(PMTmap(index)[0], PMTmap(index)[1], CPMT->at(index));
        }

        outfile->cd(); 

        auto C1 = new TCanvas(("SPMT"+std::to_string(evt)).c_str(), "", 700, 700);
        gStyle->SetPalette(1);
        SPMTplot->Draw("COLZ");
        C1->Write();

        auto C2 = new TCanvas(("SSiPM"+std::to_string(evt)).c_str(), "", 700, 700);
        gPad->SetTheta(90.);
        gPad->SetPhi(0.);
        SSiPMplot->Draw("PCOLZ FB");
        SSiPMplot->GetXaxis()->SetTitleOffset(1.5);
        SSiPMplot->GetYaxis()->SetTitleOffset(-1.2);
        C2->Write();

        auto C3 = new TCanvas(("CPMT"+std::to_string(evt)).c_str(), "", 700, 700);
        gStyle->SetPalette(1);
        CPMTplot->Draw("COLZ");
        C3->Write();

        auto C4 = new TCanvas(("CSiPM"+std::to_string(evt)).c_str(), "", 700, 700);
        gPad->SetTheta(90.);
        gPad->SetPhi(0.);
        CSiPMplot->Draw("PCOLZ FB");
        CSiPMplot->GetXaxis()->SetTitleOffset(1.5);
        CSiPMplot->GetYaxis()->SetTitleOffset(-1.2);
        C4->Write();

        //SPMTplot->Write();
        //CPMTplot->Write();
        //SSiPMplot->Write();
        //CSiPMplot->Write();
    }

};

#endif

//**************************************************
