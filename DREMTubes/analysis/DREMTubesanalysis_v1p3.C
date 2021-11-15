//**************************************************
// \file DREMTubesanalysis_v1p3.C
// \brief: Analysis of DREMTubes simulation
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 15 Novemeber 2021
//**************************************************

#include <string>
#include <array>

void analysis( const vector<double>& energies, const vector<string>& files ){

    //Initiate objects through all the analysis
    //
    cout<<"DREMTubes analysis of e+ runs"<<endl;
    auto outputfile = new TFile( "DREMTubesanalysis.root", "RECREATE" );
    double Sresp[energies.size()];
    double Serresp[energies.size()];
    double Cresp[energies.size()];
    double Cerresp[energies.size()];
    double SSiPMresp[energies.size()];
    double SSiPMerresp[energies.size()];
    double CSiPMresp[energies.size()];
    double CSiPMerresp[energies.size()];
    double Econt[energies.size()];
    double Eercont[energies.size()];
    double ETowercont[energies.size()];
    double ETowerercont[energies.size()];
    double zeros[energies.size()];
    memset( zeros, 0., energies.size()*sizeof(double));

    //For loop over Runs (energies)
    //
    for (unsigned RunNo = 0; RunNo<files.size(); RunNo++ ){
        cout<<"---> Analysis run # "<<RunNo<<", energy(GeV) "<<energies[RunNo]<<endl;  
        
        //Initiate objects through single Run
        //   
        string filename = "run2/"+files[RunNo];
        double energy = energies[RunNo];
        TFile* file = TFile::Open( filename.c_str(), "READ" );
        TTree* tree = (TTree*)file->Get( "DREMTubesout" );
    
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
        
        int nBins = 50;

        auto H1Econt = new TH1F("Econt", "Econt", 100, 0., 1.2);
        H1Econt->GetXaxis()->SetTitle("Containment");

        auto H1Stot = new TH1F("ScinTot", "ScinTot",
                              nBins*20, 0., 200*energies[RunNo]*2. );
        H1Stot->GetXaxis()->SetTitle("Scintillation signal [Sp.e.]");

        auto H1Ctot = new TH1F("CherTot", "CherTot",
                              nBins*20, 0., 60*energies[RunNo]*2. );
        H1Ctot->GetXaxis()->SetTitle("Cherenkov signal [Cp.e.]");

        auto H1PSdep = new TH1F("PSedep", "PSedep",
                              nBins*20, 0., 300. );
        H1PSdep->GetXaxis()->SetTitle("Energy [MeV]");

        auto H2PSleak = new TH2F("PSvsLeak", "PSvsLeak", nBins*20, 0., 300.,
                              nBins*20, 0., energies[RunNo]*100. );
        H2PSleak->GetXaxis()->SetTitle("Energy [MeV]");
        H2PSleak->GetYaxis()->SetTitle("Energy [MeV]");

        auto H1SSiPMtot = new TH1F("SSiPMtot", "SSiPMtot",
                                  nBins*20, 0., 200*energies[RunNo]*2. );
        H1SSiPMtot->GetXaxis()->SetTitle("Scintillation signal (SiPM) [Sp.e./GeV]");

        auto H1CSiPMtot = new TH1F("CSiPMtot", "CSiPMtot",
                                  nBins*20, 0., 60*energies[RunNo]*2. );
        H1CSiPMtot->GetXaxis()->SetTitle("Cherenkov signal (SiPM) [Cp.e./GeV]");
        
        auto H1TowerE = new TH1F("TowerE", "TowerE", 120, 0., 1.2 );
        H1TowerE->GetXaxis()->SetTitle("Single tower containment");

        auto H1TowersE = new TH1F("TowersE", "TowersE", 120, 0., 1.2 );
        H1TowersE->GetXaxis()->SetTitle("8 towers containment");

        auto H1Sresp = new TH1F("Sresptot", "Sresptot", 100, 0., 300.);
        H1Sresp->GetXaxis()->SetTitle("Scintillation response [#Sp.e./GeV]");

        auto H1Cresp = new TH1F("Cresptot", "Cresptot", 100, 0., 100.);
        H1Cresp->GetXaxis()->SetTitle("Cherenkov response [#Cp.e./GeV]");

        auto H1SSiPMresp = new TH1F("SSiPMresptot", "SSiPMresptot", 100, 0., 300.);
        H1SSiPMresp->GetXaxis()->SetTitle("Scintillation response (SiPM) [#Sp.e./GeV]");

        auto H1CSiPMresp = new TH1F("CSiPMresptot", "CSiPMresptot", 100, 0., 100.);
        H1CSiPMresp->GetXaxis()->SetTitle("Cherenkov response (SiPM) [#Cp.e./GeV]");
        
        //For loop over events
        //
        for ( unsigned int eventNo = 0; eventNo<tree->GetEntries(); eventNo++ ){
            tree->GetEntry(eventNo);
            if (PSdep<15.){
            H1Econt->Fill(edep/(energy*1000.));
            H1Stot->Fill(Stot);
            H1Ctot->Fill(Ctot);
            H1PSdep->Fill(PSdep);
            H2PSleak->Fill(PSdep, lenergy);
            
            H1SSiPMtot->Fill(std::accumulate(SSiPM->begin(), SSiPM->end(), 0.));
            H1CSiPMtot->Fill(std::accumulate(CSiPM->begin(), CSiPM->end(), 0.));

            H1TowerE->Fill((*max_element(std::begin(*TowerE), std::end(*TowerE))/(energy*1000.)));
            H1TowersE->Fill(std::accumulate(TowerE->begin(), TowerE->end(), 0.)/(energy*1000.));

            H1Sresp->Fill(Stot/(edep/1000.));
            H1Cresp->Fill(Ctot/(edep/1000.));

            H1SSiPMresp->Fill(std::accumulate(SSiPM->begin(), SSiPM->end(),0.)/(edep/1000.));
            H1CSiPMresp->Fill(std::accumulate(CSiPM->begin(), CSiPM->end(),0.)/(edep/1000.));}
        } //end for loop events

        outputfile->cd(); 
        H1Econt->Write();       
        H1Stot->Write();
        H1Ctot->Write();
        H1PSdep->Write();
        H2PSleak->Write();
        H1SSiPMtot->Write();
        H1CSiPMtot->Write();
        H1TowerE->Write();
        H1TowersE->Write();
        H1Sresp->Write();
        H1Cresp->Write();

        Econt[RunNo] = H1Econt->GetMean();
        Eercont[RunNo] = H1Econt->GetMeanError();
        ETowercont[RunNo] = H1TowerE->GetMean();
        ETowerercont[RunNo] = H1TowerE->GetMeanError();
        Sresp[RunNo] = H1Sresp->GetMean();
        Serresp[RunNo] = H1Sresp->GetMeanError();
        Cresp[RunNo] = H1Cresp->GetMean();
        Cerresp[RunNo] = H1Cresp->GetMeanError();
        SSiPMresp[RunNo] = H1SSiPMresp->GetMean();
        SSiPMerresp[RunNo] = H1SSiPMresp->GetMeanError();
        CSiPMresp[RunNo] = H1CSiPMresp->GetMean();
        CSiPMerresp[RunNo] = H1CSiPMresp->GetMeanError();
    }

    // Finalize objects over multiple runs
    //
    outputfile->cd();
    auto G1Econt = new TGraphErrors( energies.size(), &energies[0], Econt, zeros, Eercont );
    G1Econt->SetMarkerStyle(8); 
    G1Econt->SetName("Energy containment");
    G1Econt->SetTitle("Energy containment");
    G1Econt->GetYaxis()->SetRangeUser(0.,1.2);
    G1Econt->GetYaxis()->SetTitle("Energy containment");
    G1Econt->GetXaxis()->SetTitle("<E_{Beam}> [GeV]");
    G1Econt->Write();

    auto G1ETowercont = new TGraphErrors( energies.size(), &energies[0], ETowercont, zeros, ETowerercont );
    G1ETowercont->SetMarkerStyle(29); 
    G1ETowercont->SetName("SiPM-Tower containment");
    G1ETowercont->SetTitle("SiPM-Tower containment");
    G1ETowercont->GetYaxis()->SetRangeUser(0.,1.2);
    G1ETowercont->GetYaxis()->SetTitle("Energy containment");
    G1ETowercont->GetXaxis()->SetTitle("<E_{Beam}> [GeV]");
    G1ETowercont->Write();

    auto G1Sresp = new TGraphErrors( energies.size(), &energies[0], Sresp, zeros, Serresp );
    G1Sresp->SetMarkerStyle(8); 
    G1Sresp->SetName("Sresp");
    G1Sresp->SetTitle("Sres");
    G1Sresp->GetYaxis()->SetRangeUser(0.,300.);
    G1Sresp->GetYaxis()->SetTitle("[#Sp.e./GeV]");
    G1Sresp->GetXaxis()->SetTitle("<E_{Beam}> [GeV]");
    G1Sresp->Write();

    auto G1Cresp = new TGraphErrors( energies.size(), &energies[0], Cresp, zeros, Cerresp );
    G1Cresp->SetMarkerStyle(8); 
    G1Cresp->SetName("Cresp");
    G1Cresp->SetTitle("Cres");
    G1Cresp->GetYaxis()->SetRangeUser(0.,300.);
    G1Cresp->GetYaxis()->SetTitle("[#Cp.e./GeV]");
    G1Cresp->GetXaxis()->SetTitle("<E_{Beam}> [GeV]");
    G1Cresp->Write();

    auto G1SSiPMresp = new TGraphErrors( energies.size(), &energies[0], SSiPMresp, zeros, SSiPMerresp );
    G1SSiPMresp->SetMarkerStyle(29); 
    G1SSiPMresp->SetName("SSiPMresp");
    G1SSiPMresp->SetTitle("SSiPMres");
    G1SSiPMresp->GetYaxis()->SetRangeUser(0.,300.);
    G1SSiPMresp->GetYaxis()->SetTitle("[#Sp.e./GeV]");
    G1SSiPMresp->GetXaxis()->SetTitle("<E_{Beam}> [GeV]");
    G1SSiPMresp->Write();

    auto G1CSiPMresp = new TGraphErrors( energies.size(), &energies[0], CSiPMresp, zeros, CSiPMerresp );
    G1CSiPMresp->SetMarkerStyle(29); 
    G1CSiPMresp->SetName("CSiPMresp");
    G1CSiPMresp->SetTitle("CSiPMres");
    G1CSiPMresp->GetYaxis()->SetRangeUser(0.,300.);
    G1CSiPMresp->GetYaxis()->SetTitle("[#Cp.e./GeV]");
    G1CSiPMresp->GetXaxis()->SetTitle("<E_{Beam}> [GeV]");
    G1CSiPMresp->Write();

    double avgSresp = std::accumulate(Sresp, Sresp+energies.size(), 0.)/energies.size();
    double avgCresp = std::accumulate(Cresp, Cresp+energies.size(), 0.)/energies.size();
    std::cout<<"Average response (p.e./GeV), S="<<avgSresp<<" C="<<avgCresp<<std::endl;

};

void DREMTubesanalysis_v1p3(){
    
    // Analysis of e+ data
    // energies 10, 30, 60, 80, 100 GeV
    //
    vector<double> energies = {10., 30., 60., 80., 100.};
    vector<string> files;
    for ( unsigned int i=0; i<5; i++ ){
        files.push_back( "DREMTubesout_Run"+std::to_string(i)+".root" );
    }
    analysis( energies, files );

}

//**************************************************
