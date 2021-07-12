//**************************************************
// \file DREMTubesPrimaryGeneratorAction.hh
// \brief: Definition of DREMTubesPrimaryGeneratorAction class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Prevent including headers multiple times
//
#ifndef DREMTubesPrimaryGeneratorAction_h
#define DREMTubesPrimaryGeneratorAction_h 1

//Includers from Geant4
//
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4GeneralParticleSource;
//class G4ParticleGun;         //you can switch to G4ParticleGun
class G4Event;

class DREMTubesPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
    
    public:
        //Constructor()
        //
        DREMTubesPrimaryGeneratorAction();    
        //De-constructor()
        //
        virtual ~DREMTubesPrimaryGeneratorAction();

        virtual void GeneratePrimaries(G4Event* event);
  
    private:
        G4GeneralParticleSource* fGeneralParticleSource;
        //G4ParticleGun*  fParticleGun; // G4ParticleGun

};

#endif

//**************************************************
