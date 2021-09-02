//**************************************************
// \file DREMTubesTowerHelper.cc
// \brief: Implementation of DREMTubesTowerHelper class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 1 September 2021
//**************************************************

#include <iostream>

//Includers from project files
//
#include "DREMTubesTowerHelper.hh"

DREMTubesTowerHelper* DREMTubesTowerHelper::instance = 0;

//Define (private) constructor (singleton)
//
DREMTubesTowerHelper::DREMTubesTowerHelper(){}

//Define Instance() method
//
DREMTubesTowerHelper* DREMTubesTowerHelper::Instance(){
		if (instance==0){
				instance = new DREMTubesTowerHelper;
		}
		return DREMTubesTowerHelper::instance;
}

//Defint TowerID() method
//
G4int DREMTubesTowerHelper::TowerID( const G4ThreeVector& relPos ){
		std::cout<<relPos.getX()<<std::endl;
		G4int i = 0;
		return i;
}

//**************************************************
