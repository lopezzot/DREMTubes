//**************************************************
// \file DREMTubesEventAction.cc
// \brief: Implementation of DREMTubesEventAction class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Includers from project files
//
#include "DREMTubesEventAction.hh"
#include "DREMTubesRunAction.hh"

//Includers from Geant4
//
#include "g4root.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"

//Includers from C++
//
#include <iomanip>
#include <vector>

//Define constructor
//
DREMTubesEventAction::DREMTubesEventAction()
    : G4UserEventAction(),
    Energyem(0.),
    Energyem2(0.),
    EnergyScin(0.),
    EnergyCher(0.),
    NofCherenkovDetected(0),
    //NofScintillationDetected(0),
    EnergyTot(0.),
    PrimaryParticleEnergy(0.),
    EscapedEnergy(0.),
    VectorSignals(0.),
    VectorSignalsCher(0.)
{}

//Define de-constructor
//
DREMTubesEventAction::~DREMTubesEventAction() {}

//Define BeginOfEventAction() and EndOfEventAction() methods
//
void DREMTubesEventAction::BeginOfEventAction(const G4Event*) {  
    
    //Initialize data memebers at each event
    //
    Energyem = 0.;
    Energyem2 = 0.;
    EnergyScin = 0.;
    EnergyCher = 0.;
    NofCherenkovDetected = 0;
    EscapedEnergy = 0;
    //NofScintillationDetected = 0;
    EnergyTot = 0;
    /*for(int i=0;i<64;i++){
        Signalfibre[i]=0;
    }*///only if you want to use SignalFibre[64]
    for (int i=0;i<VectorSignals.size();i++){
        VectorSignals.at(i)=0.;
    }
    for (int i=0;i<VectorSignalsCher.size();i++){
        VectorSignalsCher.at(i)=0.;
    }
    PrimaryParticleEnergy = 0;  
    for(int i=0;i<2880;i++){
        if(VectorSignals.size() < 2880){
            VectorSignals.push_back(0.);
        }
    }
    //VectorSignals.at(i)=0;}
    for(int k=0;k<2880;k++){
        if(VectorSignalsCher.size() < 2880){
            VectorSignalsCher.push_back(0.);
        }
    }
    //VectorSignalsCher[k]=0;}  

}

void DREMTubesEventAction::EndOfEventAction(const G4Event* ) {
 
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    //Fill ntuple event by event
    //entries with vectors are automatically filled
    //
    analysisManager->FillNtupleDColumn(0, Energyem);
    analysisManager->FillNtupleDColumn(1, EnergyScin);
    analysisManager->FillNtupleDColumn(2, EnergyCher);
    analysisManager->FillNtupleDColumn(3, NofCherenkovDetected);
    analysisManager->FillNtupleDColumn(4, EnergyTot);
    analysisManager->FillNtupleDColumn(5, PrimaryParticleEnergy);
    analysisManager->FillNtupleSColumn(6, PrimaryParticleName);
    //analysisManager->FillNtupleSColumn(7, AbsorberMaterial);
    analysisManager->FillNtupleDColumn(7, EscapedEnergy);
    //analysisManager->FillNtupleDColumn(9, Energyem2);
    analysisManager->AddNtupleRow();

}

//**************************************************
