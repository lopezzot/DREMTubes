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
#include "globals.hh"

class G4VPhysicsConstructor;
class OpticalPhysics;
class G4NeutronTrackingCut;

class DREMTubesPhysicsList: public G4VModularPhysicsList{
    
    public:
        //Constructor
        //
        DREMTubesPhysicsList(G4String);
        //De-constructor
        //
        virtual ~DREMTubesPhysicsList();
    
        OpticalPhysics* OpPhysics;
    
        G4NeutronTrackingCut* nCut;
    
        G4bool AbsorptionOn;
};

#endif

//**************************************************


