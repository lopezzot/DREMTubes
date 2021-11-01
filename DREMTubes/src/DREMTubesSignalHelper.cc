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
G4int DREMTubesSignalHelper::SmearSSignal( const G4double& satde, const G4int& rndseed ) {
		
		//Random seed and random number generator
    //
    std::default_random_engine generator(rndseed);
	 
		//Poisson distribution and Poissonian smearing
		//
		std::poisson_distribution<int> scin_distr(satde*3.78);
    return scin_distr(generator);
		
}

//Define SmearCSignal() method
//
G4int DREMTubesSignalHelper::SmearCSignal( const G4int& rndseed ){
		
		//Random seed and random number generator
    //
    std::default_random_engine generator(rndseed);

		//Poisson distribution and Poissonian smearing
		//Shift from 200 Cp.e./GeV to 50 Cp.e./GeV
	  //
    std::poisson_distribution<int> cher_distr(0.155); 
		return cher_distr(generator);

}

//**************************************************
