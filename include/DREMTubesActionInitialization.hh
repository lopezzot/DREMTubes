//**************************************************
// \file DREMTubesActionInitialization.hh
// \brief: Definition of DREMTubesActionInitialization class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Prevent including header multiple times
//
#ifndef DREMTubesActionInitialization_h
#define DREMTubesActionInitialization_h 1

//Includers from Geant4
//
#include "G4VUserActionInitialization.hh"

class DREMTubesActionInitialization : public G4VUserActionInitialization {
    
    public:
        //Constructor
        //
        DREMTubesActionInitialization();
        virtual ~DREMTubesActionInitialization();

        virtual void BuildForMaster() const;
        virtual void Build() const;

};

#endif

//**************************************************
