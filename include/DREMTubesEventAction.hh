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
    
        void Addem(G4double de);  //Add em component
        void Addem2(G4double de); //Add em component (different estimation)
        void AddScin(G4double de);//Add energy in scintillating fibers
        void AddCher(G4double de);//Add energy in Cherenkov fibers
        void AddCherenkov(G4int n);//Add cherenkov photoelectron
        //void AddScintillation();
        void Addenergy(G4double de);//Add all energy deposited
        //void AddEnergyfibre(G4double de, G4int number);//Add energy in fiber cpn
        //void AddSignalfibre(G4int number);
        void SavePrimaryParticle(G4String name);
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
        G4double  Energyem; //Energy of em component
        G4double Energyem2; //Energy of em component (different estimation)
        G4double  EnergyScin; //Energy in scintillating fibers
        G4double  EnergyCher; //Energy in Cherenkov fibers
        G4int     NofCherenkovDetected; //Number of Cherenkov photons detected 
        //G4int     NofScintillationDetected;//Number of Scintillating photons detected 
        G4double  EnergyTot;//Total energy deposited (does not count invisibile energy)
        //G4double  Signalfibre[64];
        ////Signal in 64 single module fibers, to be used with AddEnergyfibre
        G4String PrimaryParticleName; //Name of primary particle
        G4String AbsorberMaterial; //Name of absorber material
        G4double PrimaryParticleEnergy;//Primary particle energy
        G4double EscapedEnergy;

        std::vector<G4double> VectorSignals;
        //Vector filled with scintillating fibers energy deposits
        std::vector<G4double> VectorSignalsCher;
        //Vector filled with Cherenkov fibers Cherenkov photoelectrons

};

//Inline functions definition
//
inline void DREMTubesEventAction::AddEscapedEnergy(G4double escapedenergy){
  EscapedEnergy += escapedenergy;
}

inline void DREMTubesEventAction::SavePrimaryParticle(G4String name){
  PrimaryParticleName = name;
}

inline void DREMTubesEventAction::SaveAbsorberMaterial(G4String AbsorberMaterialName){
  AbsorberMaterial = AbsorberMaterialName;
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

inline void DREMTubesEventAction::Addem(G4double de) {
  Energyem += de; 
}

inline void DREMTubesEventAction::Addem2(G4double de){
  Energyem2 += de;
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

/*inline void B4aEventAction::AddScintillation(){
  NofScintillationDetected = NofScintillationDetected +1;
}*/

inline void DREMTubesEventAction::Addenergy(G4double de){
  EnergyTot += de;
}

/*inline void B4aEventAction::AddEnergyfibre(G4double de, G4int number){
    Signalfibre[number] += de;
}*/

/*inline void B4aEventAction::AddSignalfibre(G4int number){
    Signalfibre[number] = Signalfibre[number] + 1;
}*/
                     
#endif

//**************************************************
