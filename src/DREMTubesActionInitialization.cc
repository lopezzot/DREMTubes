//**************************************************
// \file DREMTubesActionInitialization.cc
// \brief: Implementation of DREMTubesActionInitialization class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Includers from project files
//
#include "DREMTubesActionInitialization.hh"
#include "DREMTubesPrimaryGeneratorAction.hh"
#include "DREMTubesRunAction.hh"
#include "DREMTubesEventAction.hh"
#include "DREMTubesSteppingAction.hh"
#include "DREMTubesDetectorConstruction.hh"

//Constructor
//
DREMTubesActionInitialization::DREMTubesActionInitialization()
    : G4VUserActionInitialization() {}

//De-constructor
//
DREMTubesActionInitialization::~DREMTubesActionInitialization() {}

//BuildForMaster() method
//
void DREMTubesActionInitialization::BuildForMaster() const {
    
    auto eventAction = new DREMTubesEventAction;
    SetUserAction( new DREMTubesRunAction( eventAction ) );

}

//Build() method
//
void DREMTubesActionInitialization::Build() const {
  
    SetUserAction(new DREMTubesPrimaryGeneratorAction);
    auto eventAction = new DREMTubesEventAction;
    SetUserAction(new DREMTubesRunAction( eventAction ));
    SetUserAction(eventAction);
    auto detConstruction = new DREMTubesDetectorConstruction;
    SetUserAction(new DREMTubesSteppingAction(detConstruction,eventAction));

}  

//**************************************************