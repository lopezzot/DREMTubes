//**************************************************
// \file DREMTubesEventAction.hh
// \brief: Definition of DREMTubesEventAction class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Prevent including header multiple times
//
#ifndef DREMTubesEventAction_h
#define DREMTubesEventAction_h 1

//Includers from Geant4
//
#include "G4UserEventAction.hh"
#include "globals.hh"

//Includers from C++
//
#include <vector>

class DREMTubesEventAction : public G4UserEventAction {
    
    public:
        //Constructor
        //
        DREMTubesEventAction();
        //De-constructor
        //
        virtual ~DREMTubesEventAction();

        virtual void  BeginOfEventAction(const G4Event* event);
        virtual void    EndOfEventAction(const G4Event* event);
    
        void AddScin(G4double de);//Add energy in scintillating fibers
        void AddCher(G4double de);//Add energy in Cherenkov fibers
        void AddCherenkov(G4int n);//Add cherenkov photoelectron
        //void AddScintillation();
        void Addenergy(G4double de);//Add energy depositedin calo
        void SavePrimaryPDGID(G4int pdgid);
        void SaveAbsorberMaterial(G4String AbsorberMaterialName);
        void SavePrimaryEnergy(G4double primaryparticleenergy);
        void AddEscapedEnergy(G4double escapedenergy);

        //to save vectors in ntuple
        std::vector<G4double>& GetVectorSignals() {return VectorSignals;} 
        std::vector<G4double>& GetVectorSignalsCher() {return VectorSignalsCher;}

        //to fill vectors
        void AddVectorScinEnergy(G4double de, G4int fiber); 
        //fill vector of scintillating fibers with energy deposition
        void AddVectorCherPE(G4int fiber, G4int n);
        //fill vector of cherenkov fibers with chernekov photoelectrons
    
    private:
        G4double  EnergyScin; //Energy in scintillating fibers
        G4double  EnergyCher; //Energy in Cherenkov fibers
        G4int     NofCherenkovDetected; //Number of Cherenkov p.e. detected 
				G4int     NofScinDet; //Number of Scintillating p.e. detected
        G4double  EnergyTot;  //Total energy deposited (does not count invisibile energy)
        G4int     PrimaryPDGID; //PDGID of primary particle
        G4double  PrimaryParticleEnergy; //Primary particle energy
        G4double EscapedEnergy; //Energy deposited in leakage absorber

        //Vector of SiPMs filled with scintillating signals
				//
        std::vector<G4double> VectorSignals;
        //Vector of SiPMs filled with Cherenkov signals
				//
        std::vector<G4double> VectorSignalsCher;
				//Vector of PMTs filled with scintillating signals
				//
				std::vector<G4double> VecSPMT;
				//Vector of PMTs filled with Cherenkov signals
				//
				std::vector<G4double> VecCPMT;

};

//Inline functions definition
//
inline void DREMTubesEventAction::AddEscapedEnergy(G4double escapedenergy){
  EscapedEnergy += escapedenergy;
}

inline void DREMTubesEventAction::SavePrimaryPDGID(G4int pdgid){
  PrimaryPDGID = pdgid;
}

inline void DREMTubesEventAction::SavePrimaryEnergy(G4double primaryparticleenergy){
  PrimaryParticleEnergy = primaryparticleenergy;
}

inline void DREMTubesEventAction::AddVectorScinEnergy(G4double de, G4int fiber) {
    VectorSignals.at(fiber) += de;
}

inline void DREMTubesEventAction::AddVectorCherPE(G4int fiber, G4int n) {
    VectorSignalsCher.at(fiber) = VectorSignalsCher.at(fiber) + n;
}

inline void DREMTubesEventAction::AddScin(G4double de){
  EnergyScin += de;
}

inline void DREMTubesEventAction::AddCher(G4double de){
  EnergyCher += de;
}

inline void DREMTubesEventAction::AddCherenkov(G4int n){
  NofCherenkovDetected = NofCherenkovDetected +n;
}

inline void DREMTubesEventAction::Addenergy(G4double de){
  EnergyTot += de;
}

#endif

//**************************************************
