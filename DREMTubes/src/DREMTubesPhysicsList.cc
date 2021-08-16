//**************************************************
// \file DREMTubesPhysicsList.cc
// \brief: Implementation of ATLTBHECDetectorConstruction class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Includers from prject files
#include "DREMTubesPhysicsList.hh"
#include "DREMTubesOpticalPhysics.hh"

//Includers from Geant4
//
#include "globals.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4PhysListFactory.hh"
#include "G4SystemOfUnits.hh"
#include "G4ProcessTable.hh"
#include "G4UnitsTable.hh"

//Define constructor
//
DREMTubesPhysicsList::DREMTubesPhysicsList(G4String physName, const G4bool FullOptic )
    :G4VModularPhysicsList(),
    fFullOptic( FullOptic ) {
    
    //Define physics list factor and modular physics list
    //
    G4PhysListFactory factory;
    G4VModularPhysicsList* phys = NULL;
    
    // Check is physName corresponds to a real physics list
    // run an exception if not the case
    //
    if (factory.IsReferencePhysList(physName)) {
        phys = factory.GetReferencePhysList(physName);
    }    
    else {
        G4Exception("PhysicsList::PhysicsList","InvalidSetup",
                    FatalException,"PhysicsList does not exist");
    }
    
    //Register physics from physName
    //
    for (G4int i = 0; ; ++i) {
        G4VPhysicsConstructor* elem =
            const_cast<G4VPhysicsConstructor*> ( phys->GetPhysics(i) );
        
        if (elem == NULL) break;
        
        G4cout << "DREMTubes->RegisterPhysics: " << elem->GetPhysicsName() << G4endl;
        
        RegisterPhysics(elem);
    }

    // Register optical physics 
    // Turn on and off the absorption of optical photons in materials
    // 
    AbsorptionOn = true;
    RegisterPhysics( OpPhysics = new DREMTubesOpticalPhysics(fFullOptic, AbsorptionOn) );
    
}

//Define de-constructor
//
DREMTubesPhysicsList::~DREMTubesPhysicsList(){}

//**************************************************
