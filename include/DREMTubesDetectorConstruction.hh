//**************************************************
// \file DREMTubesDetectorConstruction.hh
// \brief: Definition of DREMTubesDetectorConstruction class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Prevent including header multplie times
//
#ifndef DREMTubesDetectorConstruction_h
#define DREMTUbesDetectorConstruction_h 1

//Includers from Geant4
//
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;

class DREMTubesDetectorConstruction : public G4VUserDetectorConstruction {
  
    public:
        //Constructor
        //
        DREMTubesDetectorConstruction();
        //De-constructor
        //
        virtual ~DREMTubesDetectorConstruction();

    public:
        virtual G4VPhysicalVolume* Construct();
        virtual void ConstructSDandField();

        G4LogicalVolume* constructscinfiber(double tolerance,
                                            G4double tuberadius,
                                            G4double fiberZ,
                                            G4Material* absorberMaterial,
                                            G4double coreradius,
                                            G4double coreZ, 
                                            G4Material* ScinMaterial, 
                                            G4double claddingradiusmin,
                                            G4double claddingradiusmax,
                                            G4double claddingZ,
                                            G4Material* CherMaterial);
    
        G4LogicalVolume* constructcherfiber(double tolerance, 
                                            G4double tuberadius,
                                            G4double fiberZ,
                                            G4Material* absorberMaterial,
                                            G4double coreradius,
                                            G4double coreZ, 
                                            G4Material* CherMaterial, 
                                            G4double claddingradiusmin,
                                            G4double claddingradiusmax,
                                            G4double claddingZ,
                                            G4Material* CladCherMaterial);

        //Getters
        //
        const G4VPhysicalVolume* GetmodulePV() const;
     
    private:
        
        // methods
        //
        void DefineMaterials();
        G4VPhysicalVolume* DefineVolumes();
  
        // data members
        //
        static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger; 
     
        G4VPhysicalVolume*   modulePV; // the module physical volume
    
        G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps

};

// inline functions
//
inline const G4VPhysicalVolume* DREMTubesDetectorConstruction::GetmodulePV() const { 
    return modulePV; 
}
     
#endif

//**************************************************
