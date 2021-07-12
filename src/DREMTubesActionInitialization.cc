//**************************************************
// \file DREMTubesActionInitialization.cc
// \brief: Implementation of DREMTubesActionInitialization class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Includers from project files
//
#include "DREMTubesActionInitialization.hh"
#include "B4PrimaryGeneratorAction.hh"
#include "B4RunAction.hh"
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
  SetUserAction(new B4RunAction);
}

//Build() method
//
void DREMTubesActionInitialization::Build() const {
  SetUserAction(new B4PrimaryGeneratorAction);
  SetUserAction(new B4RunAction);
  B4aEventAction* eventAction = new B4aEventAction;
  SetUserAction(eventAction);
  auto detConstruction = new DREMTubesDetectorConstruction;
  SetUserAction(new B4aSteppingAction(detConstruction,eventAction));
}  

//**************************************************
