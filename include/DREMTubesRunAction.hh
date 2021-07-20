//**************************************************
// \file DREMTubesRunAction.hh 
// \brief: Definition of DREMTubesBRunAction class 
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Prevent including header multiple times
//
#ifndef DREMTubesRunAction_h
#define DREMTubesRunAction_h 1

//Includers from Geant4
//
#include "G4UserRunAction.hh"
#include "globals.hh"

class DREMTubesEventAction;
class G4Run;

class DREMTubesRunAction : public G4UserRunAction {
    
    public:
        //Constructor
        //
        DREMTubesRunAction( DREMTubesEventAction* eventAction );
        //De-constructor
        //
        virtual ~DREMTubesRunAction();

        //Methods
        //
        virtual void BeginOfRunAction(const G4Run*);
        virtual void EndOfRunAction(const G4Run*);

    private:
        DREMTubesEventAction* fEventAction;

};

#endif

//**************************************************
