//**************************************************
// \file DREMTubesDetectorConstruction.hh
// \brief: Definition of DREMTubesDetectorConstruction class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Prevent including header multplie times
//
#ifndef DREMTubesDetectorConstruction_h
#define DREMTubesDetectorConstruction_h 1

//Includers from Geant4
//
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"

//Forward declaration
//
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
				const G4VPhysicalVolume* GetLeakCntPV() const;
				const G4VPhysicalVolume* GetWorldPV() const;

				//Other methods
				//
				G4int GetTowerID( const G4int& cpno ) const;
        G4int GetSiPMID(const G4int& cpno ) const; 
    private:
        
        //Mandatory method for Geant4
        //
        G4VPhysicalVolume* DefineVolumes();

				//Members
				//
        G4bool  fCheckOverlaps; // option for checking volumes overlaps
				
				G4VPhysicalVolume* fLeakCntPV; //PV: lekage counter
				G4VPhysicalVolume* fWorldPV;   //PV: wourld volume
};

inline G4int DREMTubesDetectorConstruction::GetTowerID( const G4int& cpno ) const {

		const G4int row = cpno / 30;
    const G4int column = (cpno - (row*30)) / 10;
    G4int TowerID = 99;
		if (row < 16) {
			if (column < 1 ){TowerID = 3;}
			else if ( column < 2 ) {TowerID = 5;}
			else if ( column < 3 ) {TowerID = 8;}
		}
		else if (row < 32) {
			if (column < 1 ){TowerID = 2;}
			else if ( column < 2 ) {TowerID = 0;}
			else if ( column < 3 ) {TowerID = 7;}
		}
		else if (row < 48) {
			if (column < 1 ){TowerID = 1;}
			else if ( column < 2 ) {TowerID = 4;}
			else if ( column < 3 ) {TowerID = 6;}
		}
	  //G4cout<<row<<" "<<column<<G4endl; 
		return TowerID;
}

inline G4int DREMTubesDetectorConstruction::GetSiPMID( const G4int& cpno ) const {
		
	  // For Tower0 tubes only
		//
		const G4int row = (cpno / 30);
		const G4int column = ((cpno - (row*30)));
		G4int index = (row-16)*10+(column-10);
		
		if (index > 160) {
			G4cout<<"ERROR in SiPM indexing!!!"<<G4endl;
			abort();
		}

		return index;
}

inline const G4VPhysicalVolume* DREMTubesDetectorConstruction::GetLeakCntPV() const {
		return fLeakCntPV;
}

inline const G4VPhysicalVolume* DREMTubesDetectorConstruction::GetWorldPV() const {
		return fWorldPV;
}

#endif

//**************************************************
