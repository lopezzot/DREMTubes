//**************************************************
// \file DREMTubesDetectorConstruction.cc
// \brief: Implementation of DREMTubesDetectorConstruction class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

//Includers from project files
//
#include "DREMTubesDetectorConstruction.hh"

//Includers from Geant4
//
#include <random>
#include <iostream>
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4LogicalSkinSurface.hh"
#include <G4GeometryTolerance.hh>
#include "G4LogicalBorderSurface.hh"
#include "G4Sphere.hh"

//Constructor
//
DREMTubesDetectorConstruction::DREMTubesDetectorConstruction()
    : G4VUserDetectorConstruction(),
    modulePV(0),
    fCheckOverlaps(true){
}

//De-constructor
//
DREMTubesDetectorConstruction::~DREMTubesDetectorConstruction() {}

//Define Construct() method
G4VPhysicalVolume* DREMTubesDetectorConstruction::Construct() {
  
    // Define volumes
    return DefineVolumes();
}

G4VPhysicalVolume* DREMTubesDetectorConstruction::DefineVolumes() {

    //--------------------------------------------------
    //Define Elements, Mixtures and Materials
    //--------------------------------------------------
    //Elements
    //
    G4String name, symbol;    
    G4double a, z;            // a=mass of a mole, z=mean number of protons;  
  
    a = 1.01*g/mole;
    G4Element* elH  = new G4Element(name="Hydrogen",symbol="H" , z= 1., a); //Hidrogen

    a = 12.01*g/mole;
    G4Element* elC  = new G4Element(name="Carbon"  ,symbol="C" , z= 6., a); //Carbon

    a = 16.00*g/mole;
    G4Element* elO  = new G4Element(name="Oxygen"  ,symbol="O" , z= 8., a); //Oxygen

    a = 28.09*g/mole;
    G4Element* elSi = new G4Element(name="Silicon", symbol="Si", z=14., a); //Silicon
  
    a = 18.9984*g/mole;
    G4Element* elF  = new G4Element("Fluorine",symbol="F" , z= 9., a); //Fluorine

    a = 63.546*g/mole;
    G4Element* elCu = new G4Element("Copper", symbol="Cu", z=29., a); //Copper

    a = 65.38*g/mole;
    G4Element* elZn = new G4Element("Zinc", symbol="Zn", z=30., a); //Zinc

    //Materials 
    //
    auto nistManager = G4NistManager::Instance();
    nistManager->FindOrBuildMaterial("G4_Cu");
    //nistManager->FindOrBuildMaterial("G4_Fe");
    //nistManager->FindOrBuildMaterial("G4_Pb");
    nistManager->FindOrBuildMaterial("G4_Si");
    //nistManager->FindOrBuildMaterial("G4_Galactic");
    nistManager->FindOrBuildMaterial("G4_AIR");

    // Polystyrene from elements (C5H5)
    G4Material* Polystyrene = new G4Material("Polystyrene", 1.05*g/cm3, 2);
    Polystyrene->AddElement(elC, 8);
    Polystyrene->AddElement(elH, 8); 

    // PMMA material from elements (C502H8)
    // 
    auto PMMA = new G4Material("PMMA", 1.19*g/cm3, 3); 
    PMMA->AddElement(elC, 5);
    PMMA->AddElement(elO, 2);
    PMMA->AddElement(elH, 8); 
    
    // Fluorinated Polymer material from elements (C2F2)
    // material for the cladding of the Cherenkov fibers
    auto fluorinatedPolymer = new G4Material("Fluorinated_Polymer", 1.43*g/cm3, 2);
    fluorinatedPolymer->AddElement(elC,2);
    fluorinatedPolymer->AddElement(elF,2);

    // Glass material from elements (SiO2)
    //
    auto Glass = new G4Material("Glass", 2.4*g/cm3, 2);
    Glass -> AddElement(elSi, 1);
    Glass -> AddElement(elO, 2); 

    // Mixtures
    //
    // Cu260 (Brass)
    //
    const double BrassDensity = 8.53*g/cm3;
    auto Cu260 = new G4Material(name="Brass", BrassDensity, 2);
    Cu260->AddElement(elCu, 70*perCent);
    Cu260->AddElement(elZn, 30*perCent);

    // Assign material to the calorimeter volumes
    //
    G4Material* defaultMaterial = G4Material::GetMaterial("G4_AIR"); 
    G4Material* absorberMaterial = G4Material::GetMaterial("G4_Cu"); 
    G4Material* ScinMaterial = G4Material::GetMaterial("Polystyrene");
    G4Material* CherMaterial = G4Material::GetMaterial("PMMA");
    G4Material* GlassMaterial = G4Material::GetMaterial("Glass");
    G4Material* SiMaterial = G4Material::GetMaterial("G4_Si");
    G4Material* CladCherMaterial = G4Material::GetMaterial("Fluorinated_Polymer");




  // Geometry parameters of world, module, fibers, SiPM

  // Geometry parameters of the module
  G4int Nofmodules = 1; //the actual number of modules is Nofmodules^2, choose 3,5,7,9
  G4int NofFibers = 16*20; // 32 of each type
  G4int NofScinFibers = NofFibers/2;
  G4int NofCherFibers = NofFibers/2;
  G4int NofFibersrow = 3*16;
  G4int NofFiberscolumn = 60;
  G4double moduleZ = (1000.)*mm;
  double tolerance = 0.05*mm;
  G4double moduleX = 3*32.*mm+1*mm+2*tolerance*NofFibersrow; 
  G4double moduleY = 59*(1.733+2*tolerance)*mm+2.0*mm;

  // Geometry parameters of the world, world is a box
  G4double worldX = 200 * moduleX;
  G4double worldY = 200 * moduleY;
  G4double worldZ = 60 * moduleZ;

  // Geometry parameters of the fiber
  G4double fiberradius = 0.5*mm;
  G4double fiberZ = moduleZ;

  // Geometry parameters of the tube
  G4double tuberadius = 1.0*mm;
  G4double tubeZ = fiberZ;

  // Geometry parameters of the core
  G4double coreradius = 0.485*mm;
  G4double coreZ = moduleZ;

  // Geometry parameters of the cladding
  G4double claddingradiusmin = 0.485*mm;
  G4double claddingradiusmax = 0.50*mm;
  G4double claddingZ = moduleZ;

  // Geometry parameters of the SiPM
  G4double SiPMX = 1.*mm;
  G4double SiPMY = SiPMX;
  G4double SiPMZ = 0.36*mm;

  // Geometry parameters of the SiPM, active silicon layer
  G4double SiX = 1.*mm;
  G4double SiY = SiX;
  G4double SiZ = 0.05*mm;

  // Absorber in front of calorimeter (to see how em resolution degrades, generally not used)
  G4double AbsorberX = 1000.*mm;
  G4double AbsorberY = 1000.*mm;
  G4double AbsorberRadLen = 56.12 *mm;  

  // Geometry parameters of the module equipped with SiPM
  // I build it so I can replicate the entire module + SiPM 
  G4double moduleequippedZ = moduleZ + SiPMZ;
  G4double moduleequippedX = moduleX; 
  G4double moduleequippedY = moduleY;



  // I need to specify the optical properties of the scintillating fiber material,
  // optical proprieties are different from scintillating proprieties and 
  // scintillating proprieties will be defined later.
  // We don't have to add WLS proprieties to scintillating fibers
  const G4int ENTRIES = 32;
  
  G4double photonEnergy[ENTRIES] =                    // Use Energy(eV)=1.24/waevelenght(um)
            { 2.034*eV, 2.068*eV, 2.103*eV, 2.139*eV, // 2.034eV is 610nm RED  
              2.177*eV, 2.216*eV, 2.256*eV, 2.298*eV,     
              2.341*eV, 2.386*eV, 2.433*eV, 2.481*eV,
              2.532*eV, 2.585*eV, 2.640*eV, 2.697*eV,
              2.757*eV, 2.820*eV, 2.885*eV, 2.954*eV, // 2.75eV is 450nm BLUE (peak of scintillating fibers)
              3.026*eV, 3.102*eV, 3.181*eV, 3.265*eV, // 3.09eV is 400nm VIOLET (end of visible)
              3.353*eV, 3.446*eV, 3.545*eV, 3.649*eV,
              3.760*eV, 3.877*eV, 4.002*eV, 4.136*eV }; //4.1eV is 300nm UV (cherenkov peak is 310-350nm)

  G4double rindexScin[ENTRIES] =
            { 1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59 };

  G4double absorptionScin[ENTRIES] =
             { 400*cm, 400*cm, 400*cm, 400*cm,
               400*cm, 400*cm, 400*cm, 400*cm,
               400*cm, 400*cm, 400*cm, 400*cm,
               400*cm, 400*cm, 400*cm, 400*cm,
               400*cm, 400*cm, 400*cm, 400*cm,
               400*cm, 400*cm, 400*cm, 400*cm,
               400*cm, 400*cm, 400*cm, 400*cm,
               400*cm, 400*cm, 400*cm, 400*cm };

  // I don't want any ABSLENGTH for the scintillating and cherenkov fibers
  // I take into account in parameterization of photon transportation
  // if you want uncomment it             
  G4MaterialPropertiesTable *MPTScin = new G4MaterialPropertiesTable();
  MPTScin -> AddProperty("RINDEX", photonEnergy, rindexScin, ENTRIES)->SetSpline(true);
  //MPTScin -> AddProperty("ABSLENGTH", photonEnergy, absorptionScin, ENTRIES)->SetSpline(true);

  // I need to specify the optical proprieties of the cherenkov fiber material
  // there are no scintillating proprieties for PMMA (clear fibres)
  // we don't have to add WLS proprieties

  G4double rindexCher[ENTRIES] =
            { 1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49 };

 G4double absorptionCher[ENTRIES] = 
            { 890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm };

  G4MaterialPropertiesTable *MPTCher = new G4MaterialPropertiesTable();
  MPTCher -> AddProperty("RINDEX", photonEnergy, rindexCher, ENTRIES)->SetSpline(true);
  //MPTCher -> AddProperty("ABSLENGTH", photonEnergy, absorptionCher, ENTRIES)->SetSpline(true);
  CherMaterial -> SetMaterialPropertiesTable(MPTCher);

  // I need to specify the optical proprieties of the cherenkov cladding material

  G4double rindexCherclad[ENTRIES] =
            { 1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42 };

  G4MaterialPropertiesTable *MPTCherclad = new G4MaterialPropertiesTable();
  MPTCherclad -> AddProperty("RINDEX", photonEnergy, rindexCherclad, ENTRIES)->SetSpline(true);
  CladCherMaterial -> SetMaterialPropertiesTable(MPTCherclad);

  // I need to specify the optical proprieties of the glass material

  G4double rindexglass[ENTRIES] =
            { 1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51 };

  G4MaterialPropertiesTable *MPTglass = new G4MaterialPropertiesTable();
  MPTglass -> AddProperty("RINDEX", photonEnergy, rindexglass, ENTRIES)->SetSpline(true);
  GlassMaterial -> SetMaterialPropertiesTable(MPTglass);

  // I need to specify the optical proprieties of the Si material

  G4double rindexSi[ENTRIES] =
            { 3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42 };

  G4double absorptionSi[ENTRIES] = 
            { 0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm };

  G4MaterialPropertiesTable *MPTSi = new G4MaterialPropertiesTable();
  MPTSi -> AddProperty("RINDEX", photonEnergy, rindexSi, ENTRIES)->SetSpline(true);
  MPTSi -> AddProperty("ABSLENGHT", photonEnergy, absorptionSi, ENTRIES)->SetSpline(true);
  SiMaterial -> SetMaterialPropertiesTable(MPTSi); 
  
  // I need to specify the SCINTILLATING proprieties of the scintillating fiber material
  // I specify also the Birk Constant of the polystyrene

  G4double Scin_FAST[ENTRIES] = // Emission spectrum for the fast component 
            { 0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0.1,
              0.2, 0.4, 0.6, 0.8,
              1., 0.8, 0.6, 0.1,
              0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0. };

  G4double Scin_SLOW[ENTRIES] = // Emission spectrum for the slow component
            { 0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0. };

  // Set Briks Constant for scintillator
  ScinMaterial->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

  MPTScin -> AddProperty("FASTCOMPONENT", photonEnergy, Scin_FAST, ENTRIES);
  MPTScin -> AddProperty("SLOWCOMPONENT", photonEnergy, Scin_SLOW, ENTRIES);
  MPTScin -> AddConstProperty("SCINTILLATIONYIELD", 10000./MeV); // Typical is 10000./MeV (this is what makes full simulations long as hell)
  MPTScin -> AddConstProperty("RESOLUTIONSCALE", 1.0); // Broad the fluctuation of photons produced
  MPTScin -> AddConstProperty("FASTTIMECONSTANT", 2.8*ns);
  MPTScin -> AddConstProperty("SLOWTIMECONSTANT", 10.*ns);
  MPTScin -> AddConstProperty("YIELDRATIO", 1.0); // I don't want a slow component, if you want it must change
  ScinMaterial -> SetMaterialPropertiesTable(MPTScin);
  
  if ( ! defaultMaterial || ! absorberMaterial || ! ScinMaterial || ! CherMaterial || ! GlassMaterial || ! CladCherMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("B4DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }
   
  // Building the calorimeter

  // Here I build the world

  G4VSolid* worldS 
    = new G4Box("World",                        // its name
                 worldX/2, worldY/2, worldZ/2); // its size
                         
  G4LogicalVolume* worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material (Galactic or Air)
                 "World");         // its name
  
  // I set the world as invisible
  worldLV->SetVisAttributes(G4VisAttributes::Invisible);

  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 worldLV,          // its logical volume                         
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 

    //absorber to calculate leakage
    G4VSolid* leakageabsorber
    = new G4Sphere("leakageabsorber",                        // its name
                1000., 1100., 0.*deg, 360.*deg, 0.*deg, 180.*deg); // its size
    
    G4LogicalVolume* leakageabsorberLV
    = new G4LogicalVolume(
                          leakageabsorber,           // its solid
                          defaultMaterial,  // its material (Galactic or Air)
                          "leakageabsorber");         // its name
    
    leakageabsorberLV->SetVisAttributes(G4VisAttributes::Invisible);   
    G4VPhysicalVolume* leakageabsorberPV
    = new G4PVPlacement(
                        0,                // no rotation
                        G4ThreeVector(),  // at (0,0,0)
                        leakageabsorberLV,          // its logical volume
                        "leakageabsorber",          // its name
                        worldLV,                // its mother  volume
                        false,            // no boolean operation
                        0,                // copy number
                        fCheckOverlaps);  // checking overlaps
   
   // Here I build the module equipped with SiPM

   G4VSolid* moduleequippedS
    = new G4Box("moduleequipped",                                          // its name
                 moduleequippedX/2, moduleequippedY/2, moduleequippedZ/2); // its size
                         
  G4LogicalVolume* moduleequippedLV
    = new G4LogicalVolume(
                 moduleequippedS,           // its solid
                 defaultMaterial,           // its material
                 "moduleequipped");         // its name

  // Here I build the calorimeter itself. As calorimeter I mean the matrix of
  // modules equipped. Uncomment it only if you want more than one module.
  
    G4VSolid* CalorimeterS 
    = new G4Box("CalorimeterS",                                                                  // its name
                 moduleequippedX*Nofmodules/2, moduleequippedY*Nofmodules/2, moduleequippedZ/2); // its size                     
    G4LogicalVolume* CalorimeterLV
    = new G4LogicalVolume(
                 CalorimeterS,           // its solid
                 defaultMaterial,        // its material 
                 "CalorimeterLV");       // its name

  // Here I place the modules equipped inside the calorimeter
  // There is no rotation of the modules, I will later rotate the entire calorimeter
  G4int copynumbermodule = 0;
  G4double m_x, m_y;
  G4ThreeVector vec_m;
  G4VPhysicalVolume* physi_moduleequipped[Nofmodules][Nofmodules];
  for(int row=0; row<Nofmodules; row++){
     for(int column=0; column<Nofmodules; column++){
        m_x = -(((Nofmodules-1)/2)*moduleX - moduleX*row);
        m_y = -(((Nofmodules-1)/2)*moduleY - moduleY*column);
           
        vec_m.setX(m_x);
        vec_m.setY(m_y);
        vec_m.setZ(0.);
        
        copynumbermodule = (1+row)+(column*Nofmodules);

        physi_moduleequipped[row][column] = new G4PVPlacement(0,
                                                        vec_m,              
                                                        moduleequippedLV,     
                                                        "moduleequipped",                        
                                                        CalorimeterLV,                      
                                                        false,                          
                                                        copynumbermodule); 
      };
   }; 
 
  // Here I place and rotate the entire calorimeter
  G4RotationMatrix rotm  = G4RotationMatrix();
  rotm.rotateY(1.0*deg);  // Set the rotation angles //0.75
  rotm.rotateX(1.0*deg);  //0.75
  G4ThreeVector position;
  position.setX(0.);
  position.setY(0.);
  position.setZ(0.);
  G4Transform3D transform = G4Transform3D(rotm,position); 

  G4VPhysicalVolume* CalorimeterPV = new G4PVPlacement(
                                                transform,        // its position and rotation
                                                CalorimeterLV,    // its logical volume                         
                                                "Calorimeter",    // its name
                                                worldLV,          // its mother  volume
                                                false,            // no boolean operation
                                                0,                // copy number
                                                fCheckOverlaps);  // checking overlaps 

  // Here I build the module: to do that I build the rectangular absorber
  // I will later put fibers into it  
  G4VSolid* moduleS
    = new G4Box("module",                          // its name
                 moduleX/2, moduleY/2, moduleZ/2); // its size
                         
  G4LogicalVolume* moduleLV
    = new G4LogicalVolume(
                 moduleS,           // its solid
                 defaultMaterial,  // its material
                 "module");         // its name

  G4ThreeVector pos_module;
  pos_module.setX(0.);
  pos_module.setY(0.);
  pos_module.setZ(-0.18);
                              
  G4VPhysicalVolume* modulePV = new G4PVPlacement(
                                                0,                // no rotation
                                                pos_module,       // at (0,0,-0.18)
                                                moduleLV,         // its logical volume                         
                                                "module",         // its name
                                                moduleequippedLV, // its mother  volume
                                                false,            // no boolean operation
                                                0,                // copy number
                                                fCheckOverlaps);  // checking overlaps 


  // Here I define the Optical Surface PROPRIETIES between the glass and the Si of the SiPM
  G4OpticalSurface* OpSurfaceGlassSi = new G4OpticalSurface("OpSurfaceGlassSi");
  
  OpSurfaceGlassSi -> SetType(dielectric_metal);
  OpSurfaceGlassSi -> SetModel(glisur);
  OpSurfaceGlassSi -> SetFinish(polished);

  G4double efficiencyOpSurfaceGlassSi[ENTRIES] =     // detection efficiency 
                                    { 0.4, 0.4, 0.4, 0.4,
                                      0.4, 0.4, 0.4, 0.4,
                                      0.4, 0.4, 0.4, 0.4,
                                      0.4, 0.4, 0.4, 0.4,
                                      0.4, 0.4, 0.4, 0.4,
                                      0.4, 0.4, 0.4, 0.4,
                                      0.4, 0.4, 0.4, 0.4,
                                      0.4, 0.4, 0.4, 0.4};
                                      

   G4double reflectivityOpSurfaceGlassSi[ENTRIES] =  // 0% reflection
                                    { 0., 0., 0., 0.,
                                      0., 0., 0., 0.,
                                      0., 0., 0., 0.,
                                      0., 0., 0., 0.,
                                      0., 0., 0., 0.,
                                      0., 0., 0., 0.,
                                      0., 0., 0., 0.,
                                      0., 0., 0., 0. };

  G4MaterialPropertiesTable* MPTOpSurfaceGlassSi = new G4MaterialPropertiesTable();
  MPTOpSurfaceGlassSi -> AddProperty("EFFICIENCY", photonEnergy, efficiencyOpSurfaceGlassSi, ENTRIES)->SetSpline(true);
  MPTOpSurfaceGlassSi -> AddProperty("REFLECTIVITY", photonEnergy, reflectivityOpSurfaceGlassSi, ENTRIES)->SetSpline(true);
  OpSurfaceGlassSi -> SetMaterialPropertiesTable(MPTOpSurfaceGlassSi);

  // Here I build the SiPM

  G4VSolid* SiPMS
    = new G4Box("SiPM",                      // its name
                 SiPMX/2, SiPMY/2, SiPMZ/2); // its size
                         
  G4LogicalVolume* SiPMLV
    = new G4LogicalVolume(
                 SiPMS,             // its solid
                 GlassMaterial,     // its material
                 "SiPM");           // its name

 // Here I build the Si of the SiPM
 
 G4VSolid* SiS
   = new G4Box("Si",                     // its name
                SiX/2, SiY/2, SiZ/2);       // its size
                         
 G4LogicalVolume* SiLV
   = new G4LogicalVolume(
                 SiS,            // its solid
                 SiMaterial,     // its material
                 "Si");          // its name

 // I put the Si inside the SiPM, I will put the SiPMs next to fibers later

 G4ThreeVector vec_Si;
 vec_Si.setX(0.);
 vec_Si.setY(0.);
 vec_Si.setZ(SiPMZ/2-SiZ/2); // Si at the end of SiPM
                             
 G4VPhysicalVolume* SiPV = new G4PVPlacement(
                                             0,                 // no rotation
                                             vec_Si,  
                                             SiLV,              // its logical volume                         
                                             "Si",              // its name
                                             SiPMLV,            // its mother  volume
                                             false,             // no boolean operation
                                             0,                 // copy number
                                             fCheckOverlaps);   // checking overlaps 
 
  // I set the visualization attributes of the Si of the SiPM
  G4VisAttributes* SiVisAtt = new G4VisAttributes(G4Colour(0.0,0.8,0.0)); //green
  SiVisAtt->SetVisibility(true);
  SiVisAtt->SetForceWireframe(true);
  SiVisAtt->SetForceSolid(true);
  SiLV->SetVisAttributes(SiVisAtt); //end of visualization attributes

  // Here I place the Logical Skin Surface around the silicon of the SiPM
  G4LogicalSkinSurface* OpsurfaceSi = new G4LogicalSkinSurface("OpsurfaceSi", SiLV, OpSurfaceGlassSi);

  // Here I define the Optical Surface PROPRIETIES between the scintillating fibers and the default material
  // air or vacuum
  // I'm trying to define an optical surface completly blacked because we absorb the light at one end of fibers

  G4OpticalSurface* OpSurfacedefault = new G4OpticalSurface("OpSurfacedefault");
  
  OpSurfacedefault -> SetType(dielectric_dielectric);
  OpSurfacedefault -> SetModel(unified);
  OpSurfacedefault -> SetFinish(polishedbackpainted); // Painted from inside the fibers, light is absorbed


  // Here I place the Scintillating fibers and the SiPM next to them
  // Attention: I place an optical surface painted (blacked) from the moduleequippedPV 
  // to the SiPMPV, in so doing I completly avoid any cross talk between SiPMs
 
  G4VPhysicalVolume* physi_S_fiber[NofFibersrow][NofFiberscolumn];
  G4VPhysicalVolume* physi_SiPM[NofFibersrow][NofFiberscolumn];  
  G4LogicalBorderSurface* logic_OpSurface_defaultAir[NofFibersrow][NofFiberscolumn];

  G4int copynumber=0;

  for(int row=0; row<NofFibersrow; row++){
     std::stringstream S_fiber_row;
     S_fiber_row.str("");
     S_fiber_row << row;
     for(int column=0; column<NofFiberscolumn; column++){
        std::stringstream S_fiber_column;
        S_fiber_column.str("");
        S_fiber_column << column;
        std::string S_name;
        std::string SiPM_name;
        S_name = "S_row" + S_fiber_row.str() + "_column_" + S_fiber_column.str(); 
        SiPM_name = "SiPMS_row" + S_fiber_row.str() + "_column_" + S_fiber_column.str();

        // I need to specify the position of each scintillating fiber before placing them
        G4double S_x, S_y, S_z;
        G4ThreeVector vec_S_fiber;
        G4ThreeVector vec_SiPM;

          if(column%2==0){
            S_x = -moduleX/2 + tuberadius + (tuberadius*2+2*tolerance)*row;
            S_y = -moduleY/2 + tuberadius + (1.733+2*tolerance*mm)*(column);
            
            vec_S_fiber.setX(S_x);
            vec_S_fiber.setY(S_y);
            vec_S_fiber.setZ(0.);

            vec_SiPM.setX(S_x);
            vec_SiPM.setY(S_y);
            vec_SiPM.setZ(fiberZ/2+SiPMZ/2-0.18);
            
            copynumber = (NofFiberscolumn*row+column);


            auto logic_S_fiber = constructscinfiber(tolerance,tuberadius,fiberZ,absorberMaterial,coreradius,coreZ,ScinMaterial,claddingradiusmin,claddingradiusmax,claddingZ,CherMaterial);
            // I need to place the scintillating fibers
            physi_S_fiber[row][column] = new G4PVPlacement(0,
                                                         vec_S_fiber,     //its position
                                                         logic_S_fiber,   //its logical volume
                                                         S_name,          //its name
                                                         moduleLV,        //its mother
                                                         false,           //no boulean operat
                                                         copynumber); 

            // I need to place the SiPMs
           physi_SiPM[row][column] = new G4PVPlacement(0,
                                                        vec_SiPM,                      //its position
                                                        SiPMLV,                        //its logical volume
                                                        SiPM_name,                    //its name
                                                        moduleequippedLV,                      //its mother
                                                        false,                        //no boulean operat
                                                        0); 
          logic_OpSurface_defaultAir[NofFibersrow][NofFiberscolumn] = new G4LogicalBorderSurface("logic_OpSurface_defaultAir", CalorimeterPV, 
            physi_SiPM[row][column], OpSurfacedefault);
          }
     };
  };

  // Here I place the Cherenkov fibers
  G4VPhysicalVolume* physi_C_fiber[NofFibersrow][NofFiberscolumn];
  
  for(int row=0; row<NofFibersrow; row++){
     std::stringstream C_fiber_row;
     C_fiber_row.str("");
     C_fiber_row << row;
     for(int column=0; column<NofFiberscolumn; column++){
        std::stringstream C_fiber_column;
        C_fiber_column.str("");
        C_fiber_column << column;
        std::string C_name;
        std::string SiPM_name;
        C_name = "C_row" + C_fiber_row.str() + "_column_" + C_fiber_column.str(); 
        SiPM_name = "SiPMC_row" + C_fiber_row.str() + "_column_" + C_fiber_column.str();

        // I need to specify the position of each cherenkov fiber
        G4double C_x, C_y, C_z;
        G4ThreeVector vec_C_fiber;
        G4ThreeVector vec_SiPM;

        if(column%2 != 0){
            C_x = -moduleX/2 + tuberadius + tuberadius + (tuberadius*2+2*tolerance)*row;
            C_y = -moduleY/2 + tuberadius + (1.733+2*tolerance*mm)*column;
         
            vec_C_fiber.setX(C_x);
            vec_C_fiber.setY(C_y);
            vec_C_fiber.setZ(0.);

            vec_SiPM.setX(C_x);
            vec_SiPM.setY(C_y);
            vec_SiPM.setZ(fiberZ/2+SiPMZ/2-0.18);

            std::cout<< row << " " << column << std::endl;
             copynumber = (NofFiberscolumn*row+column);
                        
            auto logic_C_fiber = constructcherfiber(tolerance,tuberadius,fiberZ,absorberMaterial,coreradius,coreZ,CherMaterial,claddingradiusmin,claddingradiusmax,claddingZ,CladCherMaterial);
            // I need to place the cherenkov fibers
           physi_C_fiber[row][column] = new G4PVPlacement(0,
                                                         vec_C_fiber,      //its position
                                                         logic_C_fiber,    //its logical volume
                                                         C_name,           //its name
                                                         moduleLV,         //its mother
                                                         false,            //no boulean operat
                                                         copynumber);

            // I need to place the SiPMs
            physi_SiPM[row][column] = new G4PVPlacement(0,
                                                        vec_SiPM,            //its position
                                                        SiPMLV,              //its logical volume
                                                        SiPM_name,           //its name
                                                        moduleequippedLV,    //its mother
                                                        false,               //no boulean operat
                                                        0); 
             logic_OpSurface_defaultAir[NofFibersrow][NofFiberscolumn] = new G4LogicalBorderSurface("logic_OpSurface_defaultAir", CalorimeterPV, 
             physi_SiPM[row][column], OpSurfacedefault);
          }      
     };
  };

  // I return the physical World
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4LogicalVolume* DREMTubesDetectorConstruction::constructscinfiber(double tolerance, G4double tuberadius, G4double fiberZ, G4Material* absorberMaterial, G4double coreradius, G4double coreZ, 
  G4Material* ScinMaterial, G4double claddingradiusmin, G4double claddingradiusmax, G4double claddingZ, G4Material* CherMaterial){
  // Here I build the Scintillating fiber with its core and cladding
  // I will put the fibers later inside the module
  
  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()m
  std::uniform_real_distribution<> dis(0.0, tolerance);
  double outradiussmear = dis(gen);
  tuberadius = tuberadius+outradiussmear;
  G4Tubs* S_fiber = new G4Tubs("S_fiber", 0., tuberadius, fiberZ/2, 0., 2.*pi);

  G4LogicalVolume* logic_S_fiber = new G4LogicalVolume(S_fiber,          //its solid
                                                       absorberMaterial,  //its material
                                                       "S_fiber");       //its name

  G4Tubs* Core_S_fiber = new G4Tubs("Core_S_fiber", 0., coreradius, coreZ/2, 0., 2.*pi);

  G4LogicalVolume* logic_Core_S_fiber = new G4LogicalVolume(Core_S_fiber,   //its solid
                                                            ScinMaterial,   //its material
                                                            "Core_S_fiber");//its name


  // I set the visualization attributes of the scintillating core fibers
  G4VisAttributes* ScincoreVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,0.8)); //blue
  ScincoreVisAtt->SetVisibility(true);
  ScincoreVisAtt->SetForceWireframe(true);
  ScincoreVisAtt->SetForceSolid(true);
  logic_Core_S_fiber->SetVisAttributes(ScincoreVisAtt); //end of visualization attributes

  G4ThreeVector vec_Core_S;
  vec_Core_S.setX(0.);
  vec_Core_S.setY(0.);
  vec_Core_S.setZ(0.); 
                             
  G4VPhysicalVolume* Core_S_PV = new G4PVPlacement(
                                             0,                        // no rotation
                                             vec_Core_S,               // its position
                                             logic_Core_S_fiber,       // its logical volume                         
                                             "Core_S_fiber",           // its name
                                             logic_S_fiber,            // its mother  volume
                                             false,                    // no boolean operation
                                             0,                        // copy number
                                             fCheckOverlaps);          // checking overlaps
 
  // Here I place the optical surface "OpSurfacedefault" between the scintillatinf core and the default material
  //G4LogicalBorderSurface* logic_OpSurface_SCoredefault;
  //logic_OpSurface_SCoredefault = new G4LogicalBorderSurface("logic_OpSurface_SCoredefault", Core_S_PV, worldPV, OpSurfacedefault);

  G4Tubs* Clad_S_fiber = new G4Tubs("Clad_S_fiber", claddingradiusmin, claddingradiusmax, claddingZ/2, 0., 2.*pi);

  G4LogicalVolume* logic_Clad_S_fiber = new G4LogicalVolume(Clad_S_fiber,   //its solid
                                                            CherMaterial,   //its material
                                                            "Clad_S_fiber");//its name

  // I set the visualization attributes of the scintillating clad fibers
  G4VisAttributes* ScincladVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,1.0));//light blue
  ScincladVisAtt->SetVisibility(true);
  ScincladVisAtt->SetForceWireframe(true);
  ScincladVisAtt->SetForceSolid(true);
  logic_Clad_S_fiber->SetVisAttributes(ScincladVisAtt); //end of visualization attributes

 G4ThreeVector vec_Clad_S;
 vec_Clad_S.setX(0.);
 vec_Clad_S.setY(0.);
 vec_Clad_S.setZ(0.); 
                             
 G4VPhysicalVolume* Clad_S_PV = new G4PVPlacement(
                                             0,                        // no rotation
                                             vec_Clad_S,               // its position
                                             logic_Clad_S_fiber,       // its logical volume                         
                                             "Clad_S_fiber",           // its name
                                             logic_S_fiber,            // its mother  volume
                                             false,                    // no boolean operation
                                             0,                        // copy number
                                             fCheckOverlaps);          // checking overlaps

// Here I place the optical surface "OpSurfacedefault" between the scintillating clad and the default material
//G4LogicalBorderSurface* logic_OpSurface_SCladdefault;
//logic_OpSurface_SCladdefault = new G4LogicalBorderSurface("logic_OpSurface_SCladdefault", Clad_S_PV, worldPV, OpSurfacedefault);

 // I set the visualization attributes of the tubs
  G4VisAttributes* TubeVisAtt = new G4VisAttributes(G4Colour(0.6,0.3,0.3)); //blue
  TubeVisAtt->SetVisibility(true);
  TubeVisAtt->SetForceWireframe(true);
  TubeVisAtt->SetForceSolid(true);
  logic_S_fiber->SetVisAttributes(TubeVisAtt);
return logic_S_fiber;
}

G4LogicalVolume* DREMTubesDetectorConstruction::constructcherfiber(double tolerance, G4double tuberadius, G4double fiberZ, G4Material* absorberMaterial, G4double coreradius, G4double coreZ, 
  G4Material* CherMaterial, G4double claddingradiusmin, G4double claddingradiusmax, G4double claddingZ, G4Material* CladCherMaterial){ 
// Here I build the Cherenkov fiber with its cladding
// I will put the fibers later inside the module
 
std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()m
std::uniform_real_distribution<> dis(0.0, tolerance);
double outradiussmear = dis(gen);
tuberadius = tuberadius+outradiussmear;

G4Tubs* C_fiber = new G4Tubs("C_fiber", 0., tuberadius, fiberZ/2, 0., 2.*pi);

G4LogicalVolume* logic_C_fiber = new G4LogicalVolume(C_fiber,       //it solid
                                                     absorberMaterial,  //its material
                                                     "C_fiber");     //its name

G4Tubs* Core_C_fiber = new G4Tubs("Core_C_fiber", 0., coreradius, coreZ/2, 0., 2.*pi);

G4LogicalVolume* logic_Core_C_fiber = new G4LogicalVolume(Core_C_fiber,   //its solid
                                                          CherMaterial,   //its material
                                                          "Core_C_fiber");//its name

// I set the visualization attributes of the cherenkov core fibers
G4VisAttributes* ChercoreVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,0.0)); //red
ChercoreVisAtt->SetVisibility(true);
ChercoreVisAtt->SetForceWireframe(true);
ChercoreVisAtt->SetForceSolid(true);
logic_Core_C_fiber->SetVisAttributes(ChercoreVisAtt); //end of visualization attributes

 G4ThreeVector vec_Core_C;
 vec_Core_C.setX(0.);
 vec_Core_C.setY(0.);
 vec_Core_C.setZ(0.); 
                             
 G4VPhysicalVolume* Core_C_PV = new G4PVPlacement(
                                             0,                        // no rotation
                                             vec_Core_C,               // its position
                                             logic_Core_C_fiber,       // its logical volume                         
                                             "Core_C_fiber",           // its name
                                             logic_C_fiber,            // its mother  volume
                                             false,                    // no boolean operation
                                             0,                        // copy number
                                             fCheckOverlaps);          // checking overlaps

// Here I place the optical surface "OpSurfacedefault" between the cherenkov core and the default material
//G4LogicalBorderSurface* logic_OpSurface_CCoredefault;
//logic_OpSurface_CCoredefault = new G4LogicalBorderSurface("logic_OpSurface_CCoredefault", Core_C_PV, worldPV, OpSurfacedefault);

  G4Tubs* Clad_C_fiber = new G4Tubs("Clad_C_fiber", claddingradiusmin, claddingradiusmax, claddingZ/2, 0., 2.*pi);

  G4LogicalVolume* logic_Clad_C_fiber = new G4LogicalVolume(Clad_C_fiber,   //its solid
                                                            CladCherMaterial,   //its material
                                                            "Clad_C_fiber");//its name

  // I set the visualization attributes of the cherenkov clad fibers
  G4VisAttributes* ChercladVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));//yellow 
  ChercladVisAtt->SetVisibility(true);
  ChercladVisAtt->SetForceWireframe(true);
  ChercladVisAtt->SetForceSolid(true);
  logic_Clad_C_fiber->SetVisAttributes(ChercladVisAtt); //end of visualization attributes

 G4ThreeVector vec_Clad_C;
 vec_Clad_C.setX(0.);
 vec_Clad_C.setY(0.);
 vec_Clad_C.setZ(0.); 
                             
 G4VPhysicalVolume* Clad_C_PV = new G4PVPlacement(
                                             0,                        // no rotation
                                             vec_Clad_C,               // its position
                                             logic_Clad_C_fiber,       // its logical volume                         
                                             "Clad_C_fiber",           // its name
                                             logic_C_fiber,            // its mother  volume
                                             false,                    // no boolean operation
                                             0,                        // copy number
                                             fCheckOverlaps);          // checking overlaps

 // Here I place the optical surface "OpSurfacedefault" between the cherenkov clad and the default material
//G4LogicalBorderSurface* logic_OpSurface_CCladdefault;
//logic_OpSurface_CCladdefault = new G4LogicalBorderSurface("logic_OpSurface_CCladdefault", Clad_C_PV, worldPV, OpSurfacedefault);

// I set the visualization attributes of the tubs
  G4VisAttributes* TubeVisAtt = new G4VisAttributes(G4Colour(0.6,0.3,0.3)); //blue
  TubeVisAtt->SetVisibility(true);
  TubeVisAtt->SetForceWireframe(true);
  TubeVisAtt->SetForceSolid(true);
  logic_C_fiber->SetVisAttributes(TubeVisAtt);

return logic_C_fiber;
}


//**************************************************
