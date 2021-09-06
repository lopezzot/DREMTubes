//**************************************************
// \file DREMTubesSignalHelper.hh
// \brief: Definition of DREMTubesSignalHelper class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 1 September 2021
//**************************************************

//Prevent including header multiple times
//
#ifndef DREMTubesSignalHelper_h
#define DREMTubesSignalHelper_h

//Includers from Geant4
//
#include "globals.hh"

//Includers from C++
//
#include <chrono>
#include <random>

class DREMTubesSignalHelper {

		private:

				static DREMTubesSignalHelper* instance;

				//Private constructor (singleton)
				//
				DREMTubesSignalHelper();

		public:

				static DREMTubesSignalHelper* Instance();

				G4double ApplyBirks( const G4double& de, const G4double& steplength );

				G4int SmearSSignal( const G4double& de, const G4int& rndseed );

				G4int SmearCSignal( const G4int& rndseed );

};

#endif

//**************************************************
