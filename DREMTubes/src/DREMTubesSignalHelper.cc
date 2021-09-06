//**************************************************
// \file DREMTubesSignalHelper.cc
// \brief: Implementation of DREMTubesSignalHelper class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 1 September 2021
//**************************************************

#include <iostream>

//Includers from project files
//
#include "DREMTubesSignalHelper.hh"

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

//**************************************************
