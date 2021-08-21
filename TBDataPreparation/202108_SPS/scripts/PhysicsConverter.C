//usage: root -l .x PhysicsConverter.C++
//
#include <TTree.h>
#include <TFile.h>
#include <iostream>
#include <array>
#include <stdint.h>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

struct SiPMCalibration{
    std::array<double,320> highGainPedestal,highGainDpp,lowGainPedestal,lowGainDpp;
    SiPMCalibration(const std::string&);
};

class Event{

  public:
    //Constructor and de-constructor
    //
    Event(){};
    ~Event(){};

    //Data members
    //
    uint32_t EventID;
    int SPMT1, SPMT2, SPMT3, SPMT4, SPMT5, SPMT6, SPMT7, SPMT8;
    int CPMT1, CPMT2, CPMT3, CPMT4, CPMT5, CPMT6, CPMT7, CPMT8;
    int PShower, MCounter, C1, C2;
    uint16_t SiPMHighGain[320];
    uint16_t SiPMLowGain[320];
    double SiPMPhe[320] = {0}; // Needs to be initialized at 0.

    void calibrate(const SiPMCalibration&);
};

void Event::calibrate(const SiPMCalibration& calibration){

    // >>> SIPM CALIBRATION <<< //
    for(uint16_t i=0;i<320;++i){
        // If SiPM is 0 do not subtract pede and leave 0! (board was not triggered)
        if (SiPMHighGain[i] > 0){
            double highGainPe = (SiPMHighGain[i] - calibration.highGainPedestal[i]) / calibration.highGainDpp[i];
            double lowGainPe = (SiPMLowGain[i] - calibration.lowGainPedestal[i]) / calibration.lowGainDpp[i];
            // use HG if pe < 140 else use LG. Use bool casting to avoid if/else branching
            SiPMPhe[i] = highGainPe * (int)(highGainPe < 140.) + lowGainPe * (int)(highGainPe > 140.);
        }
    }

    // >>> PMT CALIBRATION <<< //
}

SiPMCalibration::SiPMCalibration(const std::string& fname){
    std::ifstream inFile(fname);
    json jFile;
    inFile >> jFile;
    highGainPedestal = jFile["Calibrations"]["SiPM"]["highGainPedestal"];
    highGainDpp = jFile["Calibrations"]["SiPM"]["highGainDpp"];
    lowGainPedestal = jFile["Calibrations"]["SiPM"]["lowGainPedestal"];
    lowGainDpp = jFile["Calibrations"]["SiPM"]["lowGainDpp"];
}

ClassImp(Event)

void PhysicsConverter(){

  //Open merge ntuples
  //
  auto Mergfile = new TFile("merged_sps2021_run631.root", "READ");
  auto Outfile = new TFile("file.root","RECREATE");
  auto *PMTtree = (TTree*) Mergfile->Get("CERNSPS2021");
  auto *SiPMtree = (TTree*) Mergfile->Get("SiPMSPS2021");
  auto ftree = new TTree("Ftree","Ftree");
  SiPMCalibration sipmCalibration("RunXXX.json");
  ftree->SetDirectory(Outfile);
  auto ev = new Event();
  ftree->Branch("Events",ev);

  //Check entries in trees
  //
  std::cout<<"Entries in PMT / SiPM tree "<<PMTtree->GetEntries()<<" / "<<SiPMtree->GetEntries()<<std::endl;

  int EventID;
  int ADCs[96];
  int HG_b0[64];
  int HG_b1[64];
  int HG_b2[64];
  int HG_b3[64];
  int HG_b4[64];
  int LG_b0[64];
  int LG_b1[64];
  int LG_b2[64];
  int LG_b3[64];
  int LG_b4[64];
  uint16_t HG_all[320];
  uint16_t LG_all[320];
  PMTtree->SetBranchAddress("EventNumber",&EventID);
  PMTtree->SetBranchAddress("ADCs",ADCs);
  SiPMtree->SetBranchAddress("HG_Board0",HG_b0);
  SiPMtree->SetBranchAddress("HG_Board1",HG_b1);
  SiPMtree->SetBranchAddress("HG_Board2",HG_b2);
  SiPMtree->SetBranchAddress("HG_Board3",HG_b3);
  SiPMtree->SetBranchAddress("HG_Board4",HG_b4);
  SiPMtree->SetBranchAddress("LG_Board0",HG_b0);
  SiPMtree->SetBranchAddress("LG_Board1",HG_b1);
  SiPMtree->SetBranchAddress("LG_Board2",HG_b2);
  SiPMtree->SetBranchAddress("LG_Board3",HG_b3);
  SiPMtree->SetBranchAddress("LG_Board4",HG_b4);


  for( unsigned int i=0; i<PMTtree->GetEntries(); i++){
    PMTtree->GetEntry(i);
    SiPMtree->GetEntry(i);

    for(int i=0;i<64;++i){
      HG_all[i] = HG_b0[i];
      HG_all[i+64] = HG_b1[i];
      HG_all[i+64*2] = HG_b2[i];
      HG_all[i+64*3] = HG_b3[i];
      HG_all[i+64*4] = HG_b4[i];
      LG_all[i] = LG_b0[i];
      LG_all[i+64] = LG_b1[i];
      LG_all[i+64*2] = LG_b2[i];
      LG_all[i+64*3] = LG_b3[i];
      LG_all[i+64*4] = LG_b4[i];
    }

    ev->EventID = EventID;
    ev->SPMT1 = ADCs[8];
    ev->SPMT2 = ADCs[9];
    ev->SPMT3 = ADCs[10];
    ev->SPMT4 = ADCs[11];
    ev->SPMT5 = ADCs[12];
    ev->SPMT6 = ADCs[13];
    ev->SPMT7 = ADCs[14];
    ev->SPMT8 = ADCs[15];
    ev->CPMT1 = ADCs[0];
    ev->CPMT2 = ADCs[1];
    ev->CPMT3 = ADCs[2];
    ev->CPMT4 = ADCs[3];
    ev->CPMT5 = ADCs[4];
    ev->CPMT6 = ADCs[5];
    ev->CPMT7 = ADCs[6];
    ev->CPMT8 = ADCs[7];
    ev->PShower = ADCs[16];
    ev->MCounter = ADCs[32];
    ev->C1 = ADCs[64];
    ev->C2 = ADCs[65];
    for(int j=0;j<320;++j){
      ev->SiPMHighGain[j] = HG_all[j];
      ev->SiPMLowGain[j] = LG_all[j];
    }
    ev->calibrate(sipmCalibration);

    ftree->Fill();

  }

  //Write and close
  //
  Mergfile->Close();
  ftree->Write();
  Outfile->Close();

  //example on how to read events
  //
  auto file = new TFile("file.root");

  auto *tree = (TTree*) file->Get("Ftree");
  auto evento = new Event();
  tree->SetBranchAddress("Events",&evento);
  for (unsigned int i=0; i<tree->GetEntries(); i++){
    tree->GetEntry(i);
    // std::cout<<evento->EventID<<std::endl;
  }



}
