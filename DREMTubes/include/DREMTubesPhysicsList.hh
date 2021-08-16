//**************************************************
// \file DREMTubesPhysicsList.hh
// \brief: Definition of DREMTubesPhysicsList class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Prevent including header multiple times
//
#ifndef DREMTubesPhysicsList_h
#define DREMTubesPhysicsList_h 1

//Includers from Geant4
//
#include "G4VModularPhysicsList.hh"

//Includers from project files
//
#include "DREMTubesOpticalPhysics.hh"

class DREMTubesPhysicsList : public G4VModularPhysicsList{
    
    public:
        //Constructor
        //
        DREMTubesPhysicsList(G4String, const G4bool FullOptic );
        //De-constructor
        //
        virtual ~DREMTubesPhysicsList();
    
        DREMTubesOpticalPhysics* OpPhysics;
    
        G4bool AbsorptionOn;
    
    private:
        
        G4bool fFullOptic;   

};

#endif

//**************************************************


