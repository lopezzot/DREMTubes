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
        void Addenergy(G4double de);//Add energy depositedin calo
        void SavePrimaryPDGID(G4int pdgid);
        void SaveAbsorberMaterial(G4String AbsorberMaterialName);
        void SavePrimaryEnergy(G4double primaryparticleenergy);
        void AddEscapedEnergy(G4double escapedenergy);
        void AddPSEnergy(G4double de);

        //Save vectors in ntuple
				//
        std::vector<G4double>& GetVectorSignals() {return VectorSignals;} 
        std::vector<G4double>& GetVectorSignalsCher() {return VectorSignalsCher;}
				std::vector<G4double>& GetVecTowerE() {return VecTowerE;}
				std::vector<G4double>& GetVecSPMT() {return VecSPMT;}
				std::vector<G4double>& GetVecCPMT() {return VecCPMT;}

        //Fill vector of scintillating fibers with energy deposition
				//
        void AddVectorScin(G4double de, G4int fiber); 
        //Fill vector of cherenkov fibers with chernekov photoelectrons
        //
        void AddVectorCher(G4int fiber, G4int n);
        //Fill vector of energy in each tower
				//
				void AddVecTowerE(G4int TowerID, G4double de);
        //Fill vector of signals in scintillating PMTs
				//
				void AddVecSPMT(G4int PMTID, G4double de);
				//Fill vector of signals in Cherenkov PMTs
				//
				void AddVecCPMT(G4int PMTID, G4double de);

    private:
        G4double  EnergyScin; //Energy in scintillating fibers
        G4double  EnergyCher; //Energy in Cherenkov fibers
        G4int     NofCherDet; //Number of Cherenkov p.e. detected 
				G4int     NofScinDet; //Number of Scintillating p.e. detected
        G4double  EnergyTot;  //Total energy deposited (does not count invisibile energy)
        G4int     PrimaryPDGID; //PDGID of primary particle
        G4double  PrimaryParticleEnergy; //Primary particle energy
        G4double  EscapedEnergy; //Energy deposited in leakage absorber
				G4double  PSEnergy;

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
				//Vector of energy deposited in towers
				//
				std::vector<G4double> VecTowerE;

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

inline void DREMTubesEventAction::AddVectorScin(G4double de, G4int fiber) {
    VectorSignals.at(fiber) += de;
}

inline void DREMTubesEventAction::AddVectorCher(G4int fiber, G4int n) {
    VectorSignalsCher.at(fiber) = VectorSignalsCher.at(fiber) + n;
}

inline void DREMTubesEventAction::AddVecTowerE(G4int TowerID, G4double de) {
		VecTowerE.at(TowerID) += de;
}

inline void DREMTubesEventAction::AddVecSPMT(G4int PMTID, G4double de) {
		VecSPMT.at(PMTID) += de;
}

inline void DREMTubesEventAction::AddVecCPMT(G4int PMTID, G4double de) {
		VecCPMT.at(PMTID) += de;
}
inline void DREMTubesEventAction::AddScin(G4double de){
  EnergyScin += de;
}

inline void DREMTubesEventAction::AddCher(G4double de){
  EnergyCher += de;
}

inline void DREMTubesEventAction::Addenergy(G4double de){
  EnergyTot += de;
}

inline void DREMTubesEventAction::AddPSEnergy(G4double de){
	PSEnergy += de;
}
#endif

//**************************************************
