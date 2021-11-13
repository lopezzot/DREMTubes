//**************************************************
// \file DREMTubesSignalHelper.cc
// \brief: Implementation of DREMTubesSignalHelper
//         class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 1 September 2021
//**************************************************

//Includers from project files
//
#include "DREMTubesSignalHelper.hh"

//Includers from Geant4
#include "G4Poisson.hh"

DREMTubesSignalHelper* DREMTubesSignalHelper::instance = 0;

//Define (private) constructor (singleton)
//
DREMTubesSignalHelper::DREMTubesSignalHelper(){}

//Define Instance() method
//
DREMTubesSignalHelper* DREMTubesSignalHelper::Instance(){
    if (instance==0){
        instance = new DREMTubesSignalHelper;
    }
    return DREMTubesSignalHelper::instance;
}

//Define ApplyBirks() method
//
G4double DREMTubesSignalHelper::ApplyBirks( const G4double& de, const G4double& steplength ) {
		
    const G4double k_B = 0.126; //Birks constant
    return (de/steplength) / ( 1+k_B*(de/steplength) ) * steplength;

}

//Define SmearSSignal() method
//
G4int DREMTubesSignalHelper::SmearSSignal( const G4double& satde ) {
		
    return G4Poisson(satde*9.5);
		
}

//Define SmearCSignal() method
//
G4int DREMTubesSignalHelper::SmearCSignal( ){
		
    return G4Poisson(0.153);

}

//**************************************************
