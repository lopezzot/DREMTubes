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

class DREMTubesSignalHelper {

		private:

				static DREMTubesSignalHelper* instance;

				//Private constructor (singleton)
				//
				DREMTubesSignalHelper();

		public:

				static DREMTubesSignalHelper* Instance();

};

#endif

//**************************************************
