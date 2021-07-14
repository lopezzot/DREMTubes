//**************************************************
// \file DREMTubesSteppingAction.hh
// \brief: Definition of DREMTubesSteppingAction.hh
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Prevent including header multiple times
//
#ifndef DREMTubesSteppingAction_h
#define DREMTubesSteppingAction_h 1

//Includers from Geant4
//
#include "G4UserSteppingAction.hh"
class G4OpBoundaryProcess;

class DREMTubesDetectorConstruction;
class DREMTubesEventAction;

class DREMTubesSteppingAction : public G4UserSteppingAction {
    
    public:
        //Constructor
        //
        DREMTubesSteppingAction
            (const DREMTubesDetectorConstruction* detectorConstruction,
             DREMTubesEventAction* eventAction);
        //De-constructor
        //
        virtual ~DREMTubesSteppingAction();
        
        //User impementation of SteppingAction
        //
        virtual void UserSteppingAction( const G4Step* step );
    
    private:
        //Data members
        //
        const DREMTubesDetectorConstruction* fDetConstruction;

        DREMTubesEventAction*  fEventAction;  

        G4OpBoundaryProcess* fOpProcess;

};

#endif

//**************************************************
