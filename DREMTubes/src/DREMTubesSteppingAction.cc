//**************************************************
// \file DREMTubesSteppingAction.cc
// \brief: Implementation of DREMTubesSteppingAction.cc
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Includers from project files
//
#include "DREMTubesSteppingAction.hh"
#include "DREMTubesEventAction.hh"
#include "DREMTubesDetectorConstruction.hh"

//Includers from Geant4
//
#include "G4Material.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpticalPhoton.hh"

//Includers from C++
//
#include <chrono>
#include <random>

//Define constructor
//
DREMTubesSteppingAction::DREMTubesSteppingAction( DREMTubesEventAction* eventAction,
																									const DREMTubesDetectorConstruction* detConstruction,
																									const G4bool FullOptic)
    : G4UserSteppingAction(),
    fEventAction(eventAction),
		fDetConstruction(detConstruction),
    fFullOptic(FullOptic) {
		
		fSignalHelper = DREMTubesSignalHelper::Instance(); 
		
}

//Define de-constructor
//
DREMTubesSteppingAction::~DREMTubesSteppingAction() {}

//Define UserSteppingAction() method
//
void DREMTubesSteppingAction::UserSteppingAction( const G4Step* step ) {
    
    //Save auxiliary information
    //
    AuxSteppingAction( step );

    //Save fast signal information
    //
    if( fFullOptic == false ){
        FastSteppingAction( step );
    }

    //Save slow signal information
    //
    else {
				G4cout<<"ERROR: FULL OPTICS NOT SUPPORTED IN THIS VERSION"<<G4endl;
				abort();
        //SlowSteppingAction( step );
    } 
}

//Define AuxSteppingAction() method
//
void DREMTubesSteppingAction::AuxSteppingAction( const G4Step* step ) {
	
		// Get step info
    //
    G4VPhysicalVolume* volume 
        = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
    G4double edep = step->GetTotalEnergyDeposit();

    //--------------------------------------------------
    //Store auxiliary information from event steps
    //--------------------------------------------------

    if ( volume == fDetConstruction->GetLeakCntPV() ){
			  //Take care operator== works with pointers only
				//if there is a single placement of the volume
				//use names or cpNo if not the case
				//
        fEventAction->AddEscapedEnergy(step->GetTrack()->GetKineticEnergy());
        step->GetTrack()->SetTrackStatus(fStopAndKill);
    } 

    if ( volume->GetName() == "Clad_S_fiber" ||
				 volume->GetName() == "Core_S_fiber" ||
				 volume->GetName() == "Abs_S_fiber"  ||
				 volume->GetName() == "Clad_C_fiber" ||
		     volume->GetName() == "Core_C_fiber" ||
			   volume->GetName() == "Abs_C_fiber"  ) {
         
		    fEventAction->AddVecTowerE(	
					fDetConstruction->GetTowerID(step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1)),
				  edep );
		}
	
    if ( volume != fDetConstruction->GetWorldPV() ||
         volume != fDetConstruction->GetLeakCntPV() ) {

        fEventAction->Addenergy(edep); 
        
    }
   
    if ( step->GetTrack()->GetTrackID() == 1 &&
         step->GetTrack()->GetCurrentStepNumber() == 1){
        //Save primary particle energy and name
				//
        fEventAction->SavePrimaryPDGID(step->GetTrack()->GetDefinition()->GetPDGEncoding());
        fEventAction->SavePrimaryEnergy(step->GetTrack()->GetVertexKineticEnergy());
    }
}

//Define SlowSteppingAction() method
//
void DREMTubesSteppingAction::SlowSteppingAction( const G4Step* step ){
    
    //Random seed and random number generator
    //
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    
    std::poisson_distribution<int> scin_distribution(0.008); 
    std::poisson_distribution<int> cher_distribution(0.25);
    
    G4double energydeposited = step->GetTotalEnergyDeposit();
    G4VPhysicalVolume* PreStepVolume 
        = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();

    std::string Fiber;
    std::string S_fiber = "S_fiber";
    std::string C_fiber = "C_fiber";
    Fiber = PreStepVolume->GetName(); //name of current step fiber

    if ( strstr( Fiber.c_str(), S_fiber.c_str() ) ) { //scintillating fiber
        fEventAction->AddScin(energydeposited); 
    }

    if ( strstr( Fiber.c_str(), C_fiber.c_str() ) ) { //Cherenkov fiber
        fEventAction->AddCher(energydeposited);
    }

    G4String particlename = step->GetTrack()->GetDefinition()->GetParticleName();

    G4OpBoundaryProcessStatus theStatus = Undefined;

    G4ProcessManager* OpManager =
        G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

    if (OpManager) {
        G4int MAXofPostStepLoops =
              OpManager->GetPostStepProcessVector()->entries();
        G4ProcessVector* fPostStepDoItVector =
              OpManager->GetPostStepProcessVector(typeDoIt);

        for ( G4int i=0; i<MAXofPostStepLoops; i++) {
            G4VProcess* fCurrentProcess = (*fPostStepDoItVector)[i];
            fOpProcess = dynamic_cast<G4OpBoundaryProcess*>(fCurrentProcess);
            if (fOpProcess) { theStatus = fOpProcess->GetStatus(); break;}
        }
    }

    if( particlename == "opticalphoton" ) { //optical photons
        switch ( theStatus ){

            case TotalInternalReflection:
                if(strstr(Fiber.c_str(),S_fiber.c_str())){ //scintillating fibre
                    if (scin_distribution(generator)==0 && step->GetTrack()->GetCurrentStepNumber() == 1){
                        step->GetTrack()->SetTrackStatus(fStopAndKill);
                    }
                }
                else if( strstr( Fiber.c_str(), C_fiber.c_str() ) ) { //Cherenkov fibre
                    if (cher_distribution(generator)==0 && step->GetTrack()->GetCurrentStepNumber() == 1){
                        step->GetTrack()->SetTrackStatus(fStopAndKill);
                    }
                }

            case Detection:

                if (step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName() == "C_SiPM"){
                    fEventAction->AddCherenkov(1); 
                    fEventAction->AddVectorCherPE(step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(), 1);
                }
                if (step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName() == "S_SiPM"){
                    fEventAction->AddVectorScinEnergy(1, step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
                }
                   
                //Print info on Detection position and volumes
                //
                /*G4cout<<"Detection "<<step->GetPreStepPoint()->GetMaterial()->GetName()<<
                    " "<<step->GetPostStepPoint()->GetMaterial()->GetName()<<
                    " "<<step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName()<<
                    " "<<step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber()<<G4endl;*/
            break;
           
            default: 
            break;

        } //end of switch cases

    }//end of optical photon loop

}



//Define FastSteppingAction() method
//
void DREMTubesSteppingAction::FastSteppingAction( const G4Step* step ) { 

    //Random seed and random number generator
    //
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    
    //Shift from 200 Cp.e./GeV to 50 Cp.e./GeV
    //
    std::poisson_distribution<int> cher_distribution(0.155); 
    
    // Get step info
    //
		G4VPhysicalVolume* volume 
        = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
    G4double edep = step->GetTotalEnergyDeposit();
    G4double steplength = step->GetStepLength();
    
    //--------------------------------------------------
    //Store information from Scintillation and Cherenkov
    //signals
    //--------------------------------------------------
   
    std::string Fiber;
    std::string S_fiber = "S_fiber";
    std::string C_fiber = "C_fiber";
    Fiber = volume->GetName(); 
    G4int TowerID;

    if ( strstr( Fiber.c_str(), S_fiber.c_str() ) ) { //scintillating fiber/tube

			if ( step->GetTrack()->GetParticleDefinition() == G4OpticalPhoton::Definition() ) {
				step->GetTrack()->SetTrackStatus( fStopAndKill ); 
			}

			if ( step->GetTrack()->GetDefinition()->GetPDGCharge() == 0 ||
				 step->GetStepLength() == 0 ) { return; }
				 
			TowerID = fDetConstruction->GetTowerID(
					step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1));
			fEventAction->AddScin(edep);
			G4int signalhit = fSignalHelper->SmearSSignal( fSignalHelper->ApplyBirks( edep, steplength ), 
					                                            step->GetTrack()->GetCurrentStepNumber() );
			if ( TowerID != 0 ) { fEventAction->AddVecSPMT( TowerID, signalhit ); }
			else { //fEventAction->AddVectorScinEnergy(s_signal,copynumber);
		 	}
    }

    if ( strstr( Fiber.c_str(), C_fiber.c_str() ) ) { //Cherenkov fiber/tube

				fEventAction->AddCher(edep);

				if ( step->GetTrack()->GetParticleDefinition() == G4OpticalPhoton::Definition() ){
					
						G4OpBoundaryProcessStatus theStatus = Undefined;

						G4ProcessManager* OpManager =
							G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

						if (OpManager) {
								G4int MAXofPostStepLoops =
								OpManager->GetPostStepProcessVector()->entries();
								G4ProcessVector* fPostStepDoItVector =
								OpManager->GetPostStepProcessVector(typeDoIt);

								for ( G4int i=0; i<MAXofPostStepLoops; i++) {
										G4VProcess* fCurrentProcess = (*fPostStepDoItVector)[i];
										fOpProcess = dynamic_cast<G4OpBoundaryProcess*>(fCurrentProcess);
										if (fOpProcess) { theStatus = fOpProcess->GetStatus(); break; }
								}
						}

						switch ( theStatus ){
								
								case TotalInternalReflection: {
										G4int c_signal = fSignalHelper->SmearCSignal( step->GetTrack()->GetCurrentStepNumber() );
										fEventAction->AddCherenkov( c_signal );
										
										TowerID = fDetConstruction->GetTowerID(
												step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1));		
										if ( TowerID != 0 ) { fEventAction->AddVecCPMT( TowerID, c_signal ); }
										else { /*fEventAction->AddVectorCherPE(copynumber, c_signal);*/ }

										step->GetTrack()->SetTrackStatus( fStopAndKill );
								}
								default:
										step->GetTrack()->SetTrackStatus( fStopAndKill );

					  } //end of swich cases

			  }	//end of optical photon

    } //end of Cherenkov fiber
   
}

//**************************************************
