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
#include "B4aEventAction.hh"
#include "B4aSteppingAction.hh"
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
    
    auto eventAction = new B4aEventAction;
    SetUserAction( new DREMTubesRunAction( eventAction ) );

}

//Build() method
//
void DREMTubesActionInitialization::Build() const {
  
    SetUserAction(new DREMTubesPrimaryGeneratorAction);
    auto eventAction = new B4aEventAction;
    SetUserAction(new DREMTubesRunAction( eventAction ));
    SetUserAction(eventAction);
    auto detConstruction = new DREMTubesDetectorConstruction;
    SetUserAction(new B4aSteppingAction(detConstruction,eventAction));

}  

//**************************************************
