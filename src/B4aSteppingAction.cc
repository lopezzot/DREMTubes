//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B4aSteppingAction.cc 68058 2013-03-13 14:47:43Z gcosmo $
// 
/// \file B4aSteppingAction.cc
/// \brief Implementation of the B4aSteppingAction class

#include "B4aSteppingAction.hh"
#include "B4aEventAction.hh"
#include "DREMTubesDetectorConstruction.hh"
#include "G4Material.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

#include "G4OpBoundaryProcess.hh"

#include <chrono>
#include <random>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aSteppingAction::B4aSteppingAction(
                      const DREMTubesDetectorConstruction* detectorConstruction,
                      B4aEventAction* eventAction)
  : G4UserSteppingAction(),
    fDetConstruction(detectorConstruction),
    fEventAction(eventAction)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aSteppingAction::~B4aSteppingAction()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4aSteppingAction::UserSteppingAction(const G4Step* step)
{
  
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);

  std::poisson_distribution<int> cher_distribution(0.155); //shift from 200 Cp.e./GeV to 50 Cp.e./GeV

  // get volume of the current pre-step
  G4VPhysicalVolume* PreStepVolume 
    = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  // get volume of the current post-step
 // G4VPhysicalVolume* PostStepVolume  = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
  // get energy deposited by ionization in step
  G4double energydeposited = step->GetTotalEnergyDeposit();
  // get step particle name
  G4String particlename = step->GetTrack()->GetDefinition()->GetParticleName();
  // get step length
  G4double steplength = step->GetStepLength();
  //define Birk's constant
  double k_B = 0.126; 
  /*
  if (PreStepVolume->GetName() == "module"){
    //Function to save absorber material name
    fEventAction->SaveAbsorberMaterial(PreStepVolume->GetLogicalVolume()->GetMaterial()->GetName());
  }*/

  if (PreStepVolume->GetName() == "leakageabsorber" ){
          fEventAction->AddEscapedEnergy(step->GetTrack()->GetKineticEnergy());
          step->GetTrack()->SetTrackStatus(fStopAndKill);
  } 

  if ( PreStepVolume->GetName() != "World" && PreStepVolume->GetName() != "leakageabsorber" ) {
     //Function to add up energy deposited in the whole calorimeter
     fEventAction->Addenergy(energydeposited);
  }


  if ( PreStepVolume->GetName() != "World" && PreStepVolume->GetName() != "leakageabsorber") {
    if (particlename == "e-" || particlename == "e+"){
      //Function to add up energy deposited by em component
      fEventAction->Addem(energydeposited);
    }
  }
 
 
  /* part for em2 fraction estimation
  if (particlename == "pi0" && step->GetTrack()->GetCurrentStepNumber() == 1){
    //if it's a neutral pion at first step
    fEventAction->Addem2(step->GetTrack()->GetTotalEnergy());
  }

  if (particlename == "gamma" &&  step->GetTrack()->GetCreatorProcess()->GetProcessName() != "eBrem"  && step->GetTrack()->GetCreatorProcess()->GetProcessName() != "annihil"){
      if(step->GetTrack()->GetCurrentStepNumber() == 1){
      fEventAction->Addem2(step->GetTrack()->GetTotalEnergy());
      //G4cout << step->GetTrack()->GetTrackID() << step->GetTrack()->GetCreatorProcess()->GetProcessName() << G4endl;
    }
  }
  */

  if ( step->GetTrack()->GetTrackID() == 1 && step->GetTrack()->GetCurrentStepNumber() == 1){
    // Function to save primary particle energy and name
    fEventAction->SavePrimaryParticle(particlename);
    fEventAction->SavePrimaryEnergy(step->GetTrack()->GetKineticEnergy());
  }

  //Here I compute and save all informations about scintillating and Cherenkov fibers
  std::string Fiber;
  std::string S_fiber = "S_fiber";
  std::string C_fiber = "C_fiber";
  Fiber = PreStepVolume->GetName(); //name of current step fiber

  int copynumber;//copy number of fibers: from 0 to 63 by definition in detector construction
  
  //G4double distance; // will be the distance a photon travels before reaching a SiPM 
  //G4double pRandom,pDetection,pSurvive,pTot; // will be used as probabilities for parameterization of light
  //pRandom=G4UniformRand(); // random numeber between 0 and 1
  //pDetection=1.0; // SiPM photon detection efficiency 40%
  G4ThreeVector Prestep;
  G4ThreeVector Postsep;
  G4ThreeVector Momentum; // will be the versor of the momentum of each photon inside fibres
  //G4double costheta; // will be the angle of emission of each photon inside fibres

  if ( strstr(Fiber.c_str(),S_fiber.c_str())){ //it's a scintillating fiber
    //Function to add up energy depoisted in scintillating fibers:
    //- as signal saturated by Birk's law in VectorSignals
    //- as regular energy deposition in all scintillating fibers in EnergyScin
    G4double saturatedenergydeposited = 0.;
    copynumber = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1);
    if(step->GetTrack()->GetDefinition()->GetPDGCharge() != 0.){
        if (steplength != 0)
                {
                    saturatedenergydeposited = (energydeposited/steplength) / ( 1+k_B*(energydeposited/steplength) ) * steplength;
                }
    }
    else if ( particlename == "neutron" || particlename == "anti_neutron" ) {
    	        saturatedenergydeposited = 0.;
            }
      fEventAction->AddScin(energydeposited); //All energy deposited in scin fibers (not saturated)
      //fEventAction->AddEnergyfibre(edep, copynumber); //only if you want to use Signalfibre[64]
      std::poisson_distribution<int> scin_distribution(saturatedenergydeposited*3.78);
      int s_signal = scin_distribution(generator);
      fEventAction->AddVectorScinEnergy(s_signal,copynumber); //energy deposited in any scintillating fiber (saturated)

  }

  if ( strstr(Fiber.c_str(),C_fiber.c_str())){//it's a Cherenkov fiber
    //Function to add up energy deposited in Cherenkov fibres
    fEventAction->AddCher(energydeposited);
  }
/* //part to check things on killing neutrons
 if ( particlename == "neutron" || particlename == "anti_neutron" ) {
 	if (step->GetTrack()->GetTrackStatus() == 2){
    	        G4cout<< energydeposited << " " <<step->GetTrack()->GetGlobalTime()<< " "<<step->GetTrack()->GetTrackID()<< " "<<step->GetTrack()->GetKineticEnergy()<<step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()<<G4endl;
             }
            }
  
  	if (step->GetTrack()->GetGlobalTime() > 400.0){
  		//G4cout<<"morto tardi "<<particlename<<G4endl;
  		G4cout<<step->GetTrack()->GetGlobalTime()<<" "<<PreStepVolume->GetName()<<" "<< particlename<<" "<< energydeposited << " " <<step->GetTrack()->GetGlobalTime()<< " "<<step->GetTrack()->GetTrackID()<< " "<<step->GetTrack()->GetKineticEnergy()<< " "<<step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()<<G4endl;
  	}
*/  

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

  std::string SiPMC = "SiPMC";
  std::string SiPMS = "SiPMS";
  std::string SiPMdetection;

  //If the particle is an optical photon...
  if(particlename == "opticalphoton"){

     switch (theStatus){

        case TotalInternalReflection: //it's a photon reflected inside at the core cladding fibre boundary
        // Here starts the parameterization of light, if you don't want it and want to
        // have the complete full simulation with light transportation comment from here to break line!
        // Warning: if you want full simulation with both Cherenkov and scintillation on make sure to have
        // a small light yield in DetectorConstruction.cc otherwise simulation takes forever

           Fiber = PreStepVolume->GetName();//get fibre name
           /*if(strstr(Fiber.c_str(),S_fiber.c_str())){ //it's a scintillating fibre
               Prestep = step->GetPreStepPoint()->GetPosition(); //get pre step point   
               Postsep = step->GetPostStepPoint()->GetPosition(); //get post step point
               Momentum = step->GetTrack()->GetMomentumDirection(); //get momentum direction of the photon
               if(Momentum.z()>0.){ //if the photon is going towards SiPM 
                 costheta = Momentum.z(); //cosine of the photon respect to fibre axis
                 if(costheta>0.94) { //correspond to 20.4 degree maximum acceptance angle of fibres
                   distance = (560.9-Prestep.z())/costheta; //distance the photon travels before SiPM
                   pSurvive = std::exp(-(distance/4000)); //exponential decay for light attenuation with attenuation lenght of 5 m
                   pTot =pSurvive; //pSurvive*pdetection probability of not being absorbed * probability of being detected by SiPM
                   if(pRandom<pTot){ // if that's the case
                     fEventAction->AddScintillation(); //add one photoelectron from scintillation
                     step->GetTrack()->SetTrackStatus(fStopAndKill); // kill photon in order to not track it, if not simulation becomes too long
                   }
                 }
               }
             }*/

            if(strstr(Fiber.c_str(), C_fiber.c_str())){ //it's a Cherenkov fibre
               copynumber = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1);
               Prestep = step->GetPreStepPoint()->GetPosition();   
               Postsep = step->GetPostStepPoint()->GetPosition();
               Momentum = step->GetTrack()->GetMomentumDirection();
	       int c_signal = cher_distribution(generator);
               fEventAction->AddCherenkov(c_signal); // add one photoelectron from Cherenkov process in Cherenkov fibers                  
               fEventAction->AddVectorCherPE(copynumber, c_signal);
               step->GetTrack()->SetTrackStatus(fStopAndKill); //I kille the photon just after having counted it or excluded
            }
    break;

  case Detection:
  // if you want no parameterization and complete full simulation uncomment this part
    
   /* SiPMdetection = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName();
    if (strstr(SiPMdetection.c_str(),SiPMC.c_str()))
     {
       fEventAction->AddCherenkov();
     } 
   
    if (strstr(SiPMdetection.c_str(),SiPMS.c_str()))
    {
      fEventAction->AddScintillation();
    }*/
  
  break;

  default: 
     //only for parameterization, comment for full simulation
     step->GetTrack()->SetTrackStatus(fStopAndKill);
  break;
  }
}

  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
