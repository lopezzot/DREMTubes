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
#include "G4Types.hh"

//Forward declarations from Geant4
//
class G4OpBoundaryProcess;

//Forward declarations from project files
//
class DREMTubesDetectorConstruction;
class DREMTubesEventAction;

//Includers from project files
//
#include "DREMTubesSignalHelper.hh"

class DREMTubesSteppingAction : public G4UserSteppingAction {
    
    public:
        //Constructor
        //
        DREMTubesSteppingAction(DREMTubesEventAction* eventAction,
						                    const DREMTubesDetectorConstruction* detConstruction,
                                const G4bool FullOptic );
        //De-constructor
        //
        virtual ~DREMTubesSteppingAction();
        
        //User impementation of SteppingAction
        //
        virtual void UserSteppingAction( const G4Step* step );

        //Retrieve auxialiry info from Step
        //
        void AuxSteppingAction( const G4Step* step );

        //Fast signal simulation (no optical photon propagation)
        //fFullOptic == false
        //
        void FastSteppingAction( const G4Step* step ); 

        //Slow signal simulation (optical photon propagation)
        //fFullOptic == true
        //
        void SlowSteppingAction( const G4Step* step );
    
    private:

        DREMTubesEventAction*  fEventAction;  

        G4OpBoundaryProcess* fOpProcess;
                
				//Pointer to DREMTubesDetectorConstruction
				//
			  const	DREMTubesDetectorConstruction* fDetConstruction;
				
				G4bool fFullOptic;

				//Pointer to only existing implementation (singleton)
				//of DREMTubesTowerHelper
				//
				DREMTubesSignalHelper* fSignalHelper;
};

#endif

//**************************************************
