//**************************************************
// \file DREMTubesanalysis_v1p3.C
// \brief: Analysis of DREMTubes simulation
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 15 Novemeber 2021
//**************************************************

// Includers from C++
//
#include <string>
#include <array>

//Includers from project files
//
#include "edisplay.h"

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
    double ESrec[energies.size()];
    double ESerrec[energies.size()];
    double ECrec[energies.size()];
    double ECerrec[energies.size()];

    //For loop over Runs (energies)
    //
    for (unsigned RunNo = 0; RunNo<files.size(); RunNo++ ){
        cout<<"---> Analysis run # "<<RunNo<<", energy(GeV) "<<energies[RunNo]<<endl;  
        
        //Initiate objects through single Run
        //   
        string filename = "run3/"+files[RunNo];
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
        
        int nBins = 30;

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

        auto H1ESrec = new TH1F("Srec", "Srec", 100, 0., energies[RunNo]*2.);
        H1ESrec->GetXaxis()->SetTitle("Scintillation [GeV]");
        auto H1ECrec = new TH1F("Crec", "Crec", 100, 0., energies[RunNo]*2.);
        H1ECrec->GetXaxis()->SetTitle("Cherenkov [GeV]");

        //For loop over events
        //
        for ( unsigned int eventNo = 0; eventNo<tree->GetEntries(); eventNo++ ){
            tree->GetEntry(eventNo);
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
            H1CSiPMresp->Fill(std::accumulate(CSiPM->begin(), CSiPM->end(),0.)/(edep/1000.));

            H1ESrec->Fill(Stot/217.501);
            H1ECrec->Fill(Ctot/54.1621);
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
        H1ESrec->Fit("gaus","Q");
        H1ECrec->Fit("gaus","Q");
        H1ESrec->Write();
        H1ECrec->Write();

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
    G1Cresp->GetYaxis()->SetRangeUser(0.,100.);
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
    G1CSiPMresp->GetYaxis()->SetRangeUser(0.,100.);
    G1CSiPMresp->GetYaxis()->SetTitle("[#Cp.e./GeV]");
    G1CSiPMresp->GetXaxis()->SetTitle("<E_{Beam}> [GeV]");
    G1CSiPMresp->Write();

    double avgSresp = std::accumulate(Sresp, Sresp+energies.size(), 0.)/energies.size();
    double avgCresp = std::accumulate(Cresp, Cresp+energies.size(), 0.)/energies.size();
    std::cout<<"Average response (p.e./GeV), S="<<avgSresp<<" C="<<avgCresp<<std::endl;

};
/*
void analysisPS(const double& energy, const string& file ){

    //Initiate objects through all the analysis
    //
    cout<<"DREMTubes analysis of preshower in e+ runs"<<endl;
    auto outputfile = new TFile( "DREMTubesanalysisPS.root", "RECREATE" );
    int psbins = 11;
    double psarr[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21};

    double EStot[psbins];
    double EerStot[psbins];
    double EStotRMS[psbins];
    double EerStotRMS[psbins];
    double ESSiPM[psbins];
    double EerSSiPM[psbins];
    double ESSiPMRMS[psbins];
    double EerSSiPMRMS[psbins];

    double ECSiPM[psbins];
    double EerCSiPM[psbins];
    double ECSiPMRMS[psbins];
    double EerCSiPMRMS[psbins];
    double ECtot[psbins];
    double EerCtot[psbins];
    double ECtotRMS[psbins];
    double EerCtotRMS[psbins];
    
    double Econt[psbins];
    double Eercont[psbins];
    double EcontRMS[psbins];
    double EercontRMS[psbins];
    
    double zeros[psbins];
    memset( zeros, 0., 11*sizeof(double));

    cout<<"---> Analysis: e+, energy "<<energy<<endl;
        
    string filename = "run3/"+file;
    TFile* infile = TFile::Open( filename.c_str(), "READ" );
    TTree* tree = (TTree*)infile->Get( "DREMTubesout" );
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
        
    TH1F Econtarr[11];
    TH1F EStotarr[11];
    TH1F ESSiPMarr[11];
    TH1F ECtotarr[11];
    TH1F ECSiPMarr[11];

    for ( unsigned int index = 0; index<11; index++ ){
        Econtarr[index].SetTitle("Energy Containment");
        Econtarr[index].GetXaxis()->SetTitle("Energy containment");
        Econtarr[index].SetName("Containment");
        Econtarr[index].SetBins(100, 0., 1.2);

        EStotarr[index].SetTitle("Scintillation signal");
        EStotarr[index].GetXaxis()->SetTitle("Scintillation [p.e.]");
        EStotarr[index].SetName("Scintillation signal");
        EStotarr[index].SetBins(100, 0., 200.*10.*2.);
        
        ECtotarr[index].SetTitle("Cherenkov signal");
        ECtotarr[index].SetTitle("Cherenkov signal [p.e.]");
        ECtotarr[index].SetName("Cherenkov signal");
        ECtotarr[index].SetBins(100, 0., 50.*10.*2.);
        
        ESSiPMarr[index].SetTitle("SiPM scintillation signal");
        ESSiPMarr[index].GetXaxis()->SetTitle("SiPM scintillation [p.e.]");
        ESSiPMarr[index].SetName("SiPM scintillation signal");
        ESSiPMarr[index].SetBins(100, 0., 200.*10.*2.);
        
        ECSiPMarr[index].SetTitle("SiPM Cherenkov signal");
        ECSiPMarr[index].GetXaxis()->SetTitle("SiPM Cherenkov [p.e.]");
        ECSiPMarr[index].SetName("SiPM Cherenkov signal");
        ECSiPMarr[index].SetBins(100, 0., 50.*10.*2.);
    }

    //For loop over events
    //
    int index;
    for ( unsigned int eventNo = 0; eventNo<tree->GetEntries(); eventNo++ ){
        tree->GetEntry(eventNo);
        index = (int)(PSdep-5.)/6.8;
        if (index>10) index=10;
        Econtarr[index].Fill(edep/(energy*1000.));
        EStotarr[index].Fill(Stot);
        ECtotarr[index].Fill(Ctot);
        ESSiPMarr[index].Fill(std::accumulate(SSiPM->begin(), SSiPM->end(),0.));
        ECSiPMarr[index].Fill(std::accumulate(CSiPM->begin(), CSiPM->end(),0.));
    } //end for loop events

    outputfile->cd(); 
    for (unsigned int i=0; i<11; i++){
        Econtarr[i].Write();
        Econt[i]=Econtarr[i].GetMean();
        Eercont[i]=Econtarr[i].GetMeanError();
        EcontRMS[i]=Econtarr[i].GetRMS();
        EercontRMS[i]=Econtarr[i].GetRMSError();

        EStotarr[i].Write();
        EStot[i]=EStotarr[i].GetMean();
        EerStot[i]=EStotarr[i].GetMeanError();
        EStotRMS[i]=EStotarr[i].GetRMS();
        EerStotRMS[i]=EStotarr[i].GetRMSError();
        ESSiPMarr[i].Write();
        ESSiPM[i]=ESSiPMarr[i].GetMean();
        EerSSiPM[i]=ESSiPMarr[i].GetMeanError();
        ESSiPMRMS[i]=ESSiPMarr[i].GetRMS();
        EerSSiPMRMS[i]=ESSiPMarr[i].GetRMSError();

        ECtotarr[i].Write();
        ECtot[i]=ECtotarr[i].GetMean();
        EerCtot[i]=ECtotarr[i].GetMeanError();
        ECtotRMS[i]=ECtotarr[i].GetRMS();
        EerCtotRMS[i]=ECtotarr[i].GetRMSError();
        ECSiPMarr[i].Write();
        ECSiPM[i]=ECSiPMarr[i].GetMean();
        EerCSiPM[i]=ECSiPMarr[i].GetMeanError();
        ECSiPMRMS[i]=ECSiPMarr[i].GetRMS();
        EerCSiPMRMS[i]=ECSiPMarr[i].GetRMSError();
    }   
    
    // Finalize objects over multiple runs
    //
    outputfile->cd();

    auto G1Econt = new TGraphErrors( psbins, psarr, Econt, zeros, Eercont );
    G1Econt->SetMarkerStyle(8); 
    G1Econt->SetName("Energy containment");
    G1Econt->SetTitle("Energy containment");
    G1Econt->GetYaxis()->SetRangeUser(0.6,1.1);
    G1Econt->GetYaxis()->SetTitle("Energy containment");
    G1Econt->GetXaxis()->SetTitle("Preshower signal [a.u.]");
    G1Econt->Write();

    auto G1EcontRMS = new TGraphErrors( psbins, psarr, EcontRMS, zeros, EercontRMS );
    G1EcontRMS->SetMarkerStyle(8); 
    G1EcontRMS->SetName("RMS energy containment");
    G1EcontRMS->SetTitle("RMS energy containment");
    G1EcontRMS->GetYaxis()->SetRangeUser(0.,0.12);
    G1EcontRMS->GetYaxis()->SetTitle("RMS energy containment");
    G1EcontRMS->GetXaxis()->SetTitle("Preshower signal [a.u.]");
    G1EcontRMS->Write();

    auto G1EStot = new TGraphErrors( psbins, psarr, EStot, zeros, EerStot );
    G1EStot->SetMarkerStyle(8); 
    G1EStot->SetName("Scintillation");
    G1EStot->SetTitle("Scintillation signal [p.e.]");
    G1EStot->GetYaxis()->SetRangeUser(1000.,2200.);
    G1EStot->GetYaxis()->SetTitle("Scintillation signal [p.e.]");
    G1EStot->GetXaxis()->SetTitle("Preshower signal [a.u.]");
    G1EStot->Write();

    auto G1EStotRMS = new TGraphErrors( psbins, psarr, EStotRMS, zeros, EerStotRMS );
    G1EStotRMS->SetMarkerStyle(8); 
    G1EStotRMS->SetName("RMS scintillation");
    G1EStotRMS->SetTitle("RMS scintillation signal [p.e.]");
    G1EStotRMS->GetYaxis()->SetRangeUser(150.,400.);
    G1EStotRMS->GetYaxis()->SetTitle("RMS scintillation signal [p.e.]");
    G1EStotRMS->GetXaxis()->SetTitle("Preshower signal [a.u.]");
    G1EStotRMS->Write();
    
    auto G1ESSiPM = new TGraphErrors( psbins, psarr, ESSiPM, zeros, EerSSiPM );
    G1ESSiPM->SetMarkerStyle(29); 
    G1ESSiPM->SetName("SiPM scintillation");
    G1ESSiPM->SetTitle("SiPM scintillation signal [p.e.]");
    G1ESSiPM->GetYaxis()->SetRangeUser(1000.,2200.);
    G1ESSiPM->GetYaxis()->SetTitle("SiPM scintillation signal [p.e.]");
    G1ESSiPM->GetXaxis()->SetTitle("Preshower signal [a.u.]");
    G1ESSiPM->Write();

    auto G1ESSiPMRMS = new TGraphErrors( psbins, psarr, ESSiPMRMS, zeros, EerSSiPMRMS );
    G1ESSiPMRMS->SetMarkerStyle(29); 
    G1ESSiPMRMS->SetName("RMS SiPM scintillation");
    G1ESSiPMRMS->SetTitle("RMS SiPM scintillation signal [p.e.]");
    G1ESSiPMRMS->GetYaxis()->SetRangeUser(150.,400.);
    G1ESSiPMRMS->GetYaxis()->SetTitle("RMS SiPM scintillation signal [p.e.]");
    G1ESSiPMRMS->GetXaxis()->SetTitle("Preshower signal [a.u.]");
    G1ESSiPMRMS->Write();

    auto G1ECtot = new TGraphErrors( psbins, psarr, ECtot, zeros, EerCtot );
    G1ECtot->SetMarkerStyle(8); 
    G1ECtot->SetName("Cherenkov");
    G1ECtot->SetTitle("Cherenkov signal [p.e.]");
    G1ECtot->GetYaxis()->SetRangeUser(200.,600.);
    G1ECtot->GetYaxis()->SetTitle("Cherenkov signal [p.e.]");
    G1ECtot->GetXaxis()->SetTitle("Preshower signal [a.u.]");
    G1ECtot->Write();

    auto G1ECtotRMS = new TGraphErrors( psbins, psarr, ECtotRMS, zeros, EerCtotRMS );
    G1ECtotRMS->SetMarkerStyle(8); 
    G1ECtotRMS->SetName("RMS Cherenkov");
    G1ECtotRMS->SetTitle("RMS Cherenkov signal [p.e.]");
    G1ECtotRMS->GetYaxis()->SetRangeUser(30.,100.);
    G1ECtotRMS->GetYaxis()->SetTitle("RMS Cherenkov signal [p.e.]");
    G1ECtotRMS->GetXaxis()->SetTitle("Preshower signal [a.u.]");
    G1ECtotRMS->Write();

    auto G1ECSiPM = new TGraphErrors( psbins, psarr, ECSiPM, zeros, EerCSiPM );
    G1ECSiPM->SetMarkerStyle(29); 
    G1ECSiPM->SetName("SiPM Cherenkov");
    G1ECSiPM->SetTitle("SiPM Cherenkov signal [p.e.]");
    G1ECSiPM->GetYaxis()->SetRangeUser(200.,600.);
    G1ECSiPM->GetYaxis()->SetTitle("SiPM Cherenkov signal [p.e.]");
    G1ECSiPM->GetXaxis()->SetTitle("Preshower signal [a.u.]");
    G1ECSiPM->Write();

    auto G1ECSiPMRMS = new TGraphErrors( psbins, psarr, ECSiPMRMS, zeros, EerCSiPMRMS );
    G1ECSiPMRMS->SetMarkerStyle(29); 
    G1ECSiPMRMS->SetName("RMS SiPM Cherenkov");
    G1ECSiPMRMS->SetTitle("RMS SiPM Cherenkov signal [p.e.]");
    G1ECSiPMRMS->GetYaxis()->SetRangeUser(30.,100.);
    G1ECSiPMRMS->GetYaxis()->SetTitle("RMS SiPM Cherenkov signal [p.e.]");
    G1ECSiPMRMS->GetXaxis()->SetTitle("Preshower signal [a.u.]");
    G1ECSiPMRMS->Write();

};
*/
void DREMTubesanalysis_v1p3(){
    
    // Analysis of e+ data
    // energies 10, 30, 60, 80, 100 GeV
    //
    vector<double> energies = {10., 30., 60., 80., 100.};
    vector<string> files;
    for ( unsigned int i=0; i<5; i++ ){
        files.push_back( "DREMTubesout_Run"+std::to_string(i)+".root" );
    }
    //analysis( energies, files );
    //analysisPS( 10., "DREMTubesout_Run0.root");
    //edisplay( 10., "DREMTubesout_Run0.root");
    //eradius( 10., "DREMTubesout_Run0.root");
    eradius( 10., "test.root");
}

//**************************************************
