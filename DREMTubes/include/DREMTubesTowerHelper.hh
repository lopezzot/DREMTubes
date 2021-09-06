//**************************************************
// \file DREMTubesTowerHelper.hh
// \brief: Definition of DREMTubesTowerHelper class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 1 September 2021
//**************************************************

//Prevent including header multiple times
//
#ifndef DREMTubesTowerHelper_h
#define DREMTubesTowerHelper_h

#include "G4ThreeVector.hh"

class DREMTubesTowerHelper {

		private:

				static DREMTubesTowerHelper* instance;

				//Private constructor (singleton)
				//
				DREMTubesTowerHelper();

		public:

				static DREMTubesTowerHelper* Instance();

};

#endif

//**************************************************
