//**************************************************
// \file DREMTubesOpticalPhysics.hh 
// \brief: Definition of DREMTubesOpticalPhysics class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************//

// Prevent including header multiple times
//
#ifndef DREMTubesOpticalPhysics_h
#define DREMTubesOpticalPhysics_h 1

// Includers from Geant4
//
#include "globals.hh"
#include "G4OpWLS.hh"
#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpMieHG.hh"
#include "G4OpRayleigh.hh"
#include "G4OpAbsorption.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4VPhysicsConstructor.hh"

class DREMTubesOpticalPhysics : public G4VPhysicsConstructor {
    
    public: 
        // Constructor
        //
        DREMTubesOpticalPhysics(G4bool toggle=true);
        // Deconstructor
        //
        virtual ~DREMTubesOpticalPhysics();
    
        virtual void ConstructParticle();
        virtual void ConstructProcess();
   
        // Getters for processes
        //
        G4OpWLS* GetWLSProcess() {return theWLSProcess;}
        G4Cerenkov* GetCerenkovProcess() {return theCerenkovProcess;}
        G4Scintillation* GetScintillationProcess() {return theScintProcess;}
        G4OpAbsorption* GetAbsorptionProcess() {return theAbsorptionProcess;}
        G4OpRayleigh* GetRayleighScatteringProcess() {return theRayleighScattering;}
        G4OpMieHG* GetMieHGScatteringProcess() {return theMieHGScatteringProcess;}
        G4OpBoundaryProcess* GetBoundaryProcess() { return theBoundaryProcess;}
    
    private:
    
        G4OpWLS*             theWLSProcess;
        G4Cerenkov*          theCerenkovProcess;
        G4Scintillation*     theScintProcess;
        G4OpAbsorption*      theAbsorptionProcess;
        G4OpRayleigh*        theRayleighScattering;
        G4OpMieHG*           theMieHGScatteringProcess;
        G4OpBoundaryProcess* theBoundaryProcess;
    
        G4bool AbsorptionOn;
    
};

#endif

//**************************************************//
