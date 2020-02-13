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
//
// $Id: PhysicsList.cc,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsList.hh"

#include "globals.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4PhysListFactory.hh"
#include "G4SystemOfUnits.hh"
#include "G4ProcessTable.hh"
#include "G4UnitsTable.hh"
#include "G4NeutronTrackingCut.hh"

//#include "EMPhysics.hh"
#include "OpticalPhysics.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList(G4String physName):  G4VModularPhysicsList()
{
    // default cut value  (1.0mm)
    defaultCutValue = 1.0*mm;
    // SetVerboseLevel(1);
    
    //--- EM+Hadron physics ---
    G4PhysListFactory factory;
    G4VModularPhysicsList* phys = NULL;
    
    // This check if the physicslist is already implemented
    if (factory.IsReferencePhysList(physName))
    {
        phys = factory.GetReferencePhysList(physName);
        if(!phys)
        {
            G4Exception("PhysicsList::PhysicsList","InvalidSetup",
                        FatalException,"PhysicsList does not exist");

        }
    }
    
    for (G4int i = 0; ; ++i)
    {
        G4VPhysicsConstructor* elem = const_cast<G4VPhysicsConstructor*> (phys->GetPhysics(i));
        
        if (elem == NULL) break;
        
        G4cout << "RegisterPhysics: " << elem->GetPhysicsName() << G4endl;
        
        // Not sure if we need it  
        if (std::strcmp(elem->GetPhysicsName(), "neutronTrackingCut") == 0)
        {
            nCut = new G4NeutronTrackingCut("neutronTrackingCut", 1);
            nCut->SetKineticEnergyLimit(10.*keV);
            nCut->SetTimeLimit(300.*ns);
            RegisterPhysics(nCut);
            continue;
        }
        
        RegisterPhysics(elem);
    }

    
    nCut = new G4NeutronTrackingCut("Neutron tracking cut", 1);
    nCut->SetKineticEnergyLimit(10.*keV);
    nCut->SetTimeLimit(300.*ns);
    RegisterPhysics(nCut);
    
    
    //--- Optical physics ---
    // Turn on and off the absorption of optical photons in materials
    // Register the optical physics
    AbsorptionOn = true;
    RegisterPhysics(OpPhysics = new OpticalPhysics(AbsorptionOn));
    
    // EM Physics
    //RegisterPhysics(new EMPhysics());
    
    // Muon Physics
    //RegisterPhysics(new GCDMuonPhysics());
    
    
}


PhysicsList::~PhysicsList()
{
}

void PhysicsList::SetCuts()
{
    //  " G4VUserPhysicsList::SetCutsWithDefault" method sets
    //   the default cut value for all particle types
    //SetCutsWithDefault();
    
    G4cout << "PhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
    
    SetCutValue(defaultCutValue, "gamma");
    SetCutValue(defaultCutValue, "e-");
    SetCutValue(defaultCutValue, "e+");
}
