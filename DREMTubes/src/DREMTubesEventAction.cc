//**************************************************
// \file DREMTubesEventAction.cc
// \brief: Implementation of DREMTubesEventAction 
//         class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
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
    EnergyScin(0.),
    EnergyCher(0.),
    NofCherenkovDetected(0),
    NofScinDet(0),
    EnergyTot(0.),
    PrimaryPDGID(0),
    PrimaryParticleEnergy(0.),
    EscapedEnergy(0.),
    VectorSignals(0.),
    VectorSignalsCher(0.),
		VecSPMT(0.),
		VecCPMT(0.),
		VecTowerE(0.) {
}

//Define de-constructor
//
DREMTubesEventAction::~DREMTubesEventAction() {}

//Define BeginOfEventAction() and EndOfEventAction() methods
//
void DREMTubesEventAction::BeginOfEventAction(const G4Event*) {  
    
    //Initialize data memebers at begin of each event
    //
    EnergyScin = 0.;
    EnergyCher = 0.;
    NofCherenkovDetected = 0;
		NofScinDet = 0;
    EnergyTot = 0;
		PrimaryPDGID = 0;
		PrimaryParticleEnergy = 0;
    EscapedEnergy = 0;

		VectorSignals.assign(2880, 0.);
		VectorSignalsCher.assign(2880, 0.);
		
		VecSPMT.assign(9, 0.);
		VecCPMT.assign(9, 0.);
		VecTowerE.assign(9, 0.);

}

void DREMTubesEventAction::EndOfEventAction(const G4Event* ) {
 
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    //Fill ntuple event by event
    //entries with vectors are automatically filled
    //
    analysisManager->FillNtupleDColumn(0, EnergyScin);
    analysisManager->FillNtupleDColumn(1, EnergyCher);
    analysisManager->FillNtupleDColumn(2, NofCherenkovDetected);
		analysisManager->FillNtupleDColumn(3, NofScinDet);
    analysisManager->FillNtupleDColumn(4, EnergyTot);
    analysisManager->FillNtupleDColumn(5, PrimaryParticleEnergy);
    analysisManager->FillNtupleIColumn(6, PrimaryPDGID);
    analysisManager->FillNtupleDColumn(7, EscapedEnergy);
    analysisManager->AddNtupleRow();
		//Vector entries in ntuple are automatically filled

		/*
    G4int tot_S = 0;
    G4int tot_C = 0;
    for(unsigned int i=0; i<VectorSignals.size(); i++){
        tot_S += VectorSignals.at(i);
    }

    for(unsigned int i=0; i<VectorSignalsCher.size(); i++){
        tot_C += VectorSignalsCher.at(i);
    }

    G4cout<<"Tot S "<<tot_S<<" Tot C "<<tot_C<<G4endl;
		*/
}

//**************************************************
