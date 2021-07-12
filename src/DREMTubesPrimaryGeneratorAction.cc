//**************************************************
// \file DREMTubesPrimaryGeneratorAction.cc
// \brief: Implementation of DREMTubesPrimaryGeneratorAction class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Includers from project files
//
#include "DREMTubesPrimaryGeneratorAction.hh"

//Includers from Geant4
//
#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

//Constructor
//
DREMTubesPrimaryGeneratorAction::DREMTubesPrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(),
   fGeneralParticleSource( nullptr )
   /*fParticleGun( nullptr )*/ {

    //G4int nofParticles = 1;                           //for particle gun
    //fParticleGun = new G4ParticleGun(nofParticles);   //for particle gun
    fGeneralParticleSource = new G4GeneralParticleSource();

    //default G4GeneralParticleSource parameters (can be changed via UI)
    //
    G4ParticleDefinition* particleDefinition =
        G4ParticleTable::GetParticleTable()->FindParticle("e-");

    fGeneralParticleSource->SetParticleDefinition(particleDefinition);
    fGeneralParticleSource->SetParticlePosition( G4ThreeVector( 0.,0.,0. ) );

}

//De-constructor
//
DREMTubesPrimaryGeneratorAction::~DREMTubesPrimaryGeneratorAction() {
  
    delete fGeneralParticleSource;
    //delete fParticleGun;

}

//GeneratePrimaries() method
//
void DREMTubesPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    
    fGeneralParticleSource->GeneratePrimaryVertex(anEvent);
    //fParticleGun->GeneratePrimaryVertex(anEvent);

}

//**************************************************
