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

//Includers from C++
//
#include <chrono>
#include <random>

//Define constructor
//
DREMTubesSteppingAction::DREMTubesSteppingAction(
    DREMTubesEventAction* eventAction,
    const G4bool FullOptic)
    : G4UserSteppingAction(),
    fEventAction(eventAction),
    fFullOptic(FullOptic)
{}

//Define de-constructor
//
DREMTubesSteppingAction::~DREMTubesSteppingAction() {}

//Define UserSteppingAction() method
//
void DREMTubesSteppingAction::UserSteppingAction( const G4Step* step) {
    
    //Random seed and random number generator
    //
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    
    //Shift from 200 Cp.e./GeV to 50 Cp.e./GeV
    //
    std::poisson_distribution<int> cher_distribution(0.155); 

    // Get step info
    //
    G4VPhysicalVolume* PreStepVolume 
        = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
    //G4VPhysicalVolume* PostStepVolume
    //= step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
    G4double energydeposited = step->GetTotalEnergyDeposit();
    G4String particlename = step->GetTrack()->GetDefinition()->GetParticleName();
    G4double steplength = step->GetStepLength();
    
    double k_B = 0.126; //Birks constant
    
    //--------------------------------------------------
    //Store auxiliary information from event steps
    //--------------------------------------------------

    /*if (PreStepVolume->GetName() == "module"){
        //Function to save absorber material name
        fEventAction->SaveAbsorberMaterial
            (PreStepVolume->GetLogicalVolume()->GetMaterial()->GetName());
    }*/

    if (PreStepVolume->GetName() == "leakageabsorber" ){
        fEventAction->AddEscapedEnergy(step->GetTrack()->GetKineticEnergy());
        step->GetTrack()->SetTrackStatus(fStopAndKill);
    } 

    if ( PreStepVolume->GetName() != "World" 
         && PreStepVolume->GetName() != "leakageabsorber" ) {
            fEventAction->Addenergy(energydeposited); //energy deposited in calo
    }

    if ( PreStepVolume->GetName() != "World"
         && PreStepVolume->GetName() != "leakageabsorber") {

        if (particlename == "e-" || particlename == "e+"){
            fEventAction->Addem(energydeposited); //energy deposited by em-component
        }
    }
 
    /*//em2 fraction estimation (deprecated)
    //
    if (particlename == "pi0" && step->GetTrack()->GetCurrentStepNumber() == 1){
        //if it's a neutral pion at first step
        fEventAction->Addem2(step->GetTrack()->GetTotalEnergy());
    }

    if (particlename == "gamma" &&
        step->GetTrack()->GetCreatorProcess()->GetProcessName() != "eBrem"  &&
        step->GetTrack()->GetCreatorProcess()->GetProcessName() != "annihil"){
        if(step->GetTrack()->GetCurrentStepNumber() == 1){
            fEventAction->Addem2(step->GetTrack()->GetTotalEnergy());
        }
    }*/

    if ( step->GetTrack()->GetTrackID() == 1 &&
         step->GetTrack()->GetCurrentStepNumber() == 1){
        //Save primary particle energy and name
        fEventAction->SavePrimaryParticle(particlename);
        fEventAction->SavePrimaryEnergy(step->GetTrack()->GetKineticEnergy());
    }
    
    //--------------------------------------------------
    //Store information from Scintillation and Cherenkov
    //signals
    //--------------------------------------------------
    
    if (!fFullOptic){
        std::string Fiber;
        std::string S_fiber = "S_fiber";
        std::string C_fiber = "C_fiber";
        Fiber = PreStepVolume->GetName(); //name of current step fiber
    
        G4int copynumber;
      
        if ( strstr( Fiber.c_str(), S_fiber.c_str() ) ) { //scintillating fiber
            G4double saturatedenergydeposited = 0.;
            copynumber = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1);
            if(step->GetTrack()->GetDefinition()->GetPDGCharge() != 0.) {
                if (steplength != 0) {
                    saturatedenergydeposited =
                        (energydeposited/steplength) /
                        ( 1+k_B*(energydeposited/steplength) ) * steplength;
                }
            }
            fEventAction->AddScin(energydeposited); 
            std::poisson_distribution<int> scin_distribution(
                    saturatedenergydeposited*3.78);
            int s_signal = scin_distribution(generator);
            fEventAction->AddVectorScinEnergy(s_signal,copynumber);
            //energy deposited in any scintillating fiber (saturated)
        }
    
        if ( strstr( Fiber.c_str(), C_fiber.c_str() ) ) { //Cherenkov fiber
            fEventAction->AddCher(energydeposited);
        }
        
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
    
            case TotalInternalReflection: //photon reflected inside fiber
                Fiber = PreStepVolume->GetName();
                
                if(strstr(Fiber.c_str(),S_fiber.c_str())){ //scintillating fibre
                    step->GetTrack()->SetTrackStatus(fStopAndKill);
                }
    
                if( strstr( Fiber.c_str(), C_fiber.c_str() ) ) { //Cherenkov fibre
                   copynumber = 
                       step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1);
    	       int c_signal = cher_distribution(generator);
                   fEventAction->AddCherenkov(c_signal); 
                   fEventAction->AddVectorCherPE(copynumber, c_signal);
                   step->GetTrack()->SetTrackStatus(fStopAndKill); 
                }
                break;
    
            case Detection:
                //To be used for SiPM simulation (optional)
                //
                break;
    
            default: 
                step->GetTrack()->SetTrackStatus(fStopAndKill);
                break;
    
            } //end of switch cases
    
        }//end of optical photon loop

    }//end of FullOptic == false option 

    if (fFullOptic){
       if ( particlename == "opticalphoton" ){G4cout<<"fotone"<<G4endl;} 
    }
}

//**************************************************
