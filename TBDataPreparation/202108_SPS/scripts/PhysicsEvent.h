//**************************************************
// \file PhysicsEvent.h
// \brief: implementation of Event class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
//          Edoardo Proserpio (Uni Insubria)
// \start date: 20 August 2021
//**************************************************

#include <iostream>
#include <array>
#include <stdint.h>
#include <string>
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

struct SiPMCalibration{
    std::array<double,320> highGainPedestal,highGainDpp,lowGainPedestal,lowGainDpp;
    SiPMCalibration(const std::string&);
};

SiPMCalibration::SiPMCalibration(const std::string& fname){
    std::ifstream inFile(fname,std::ifstream::in);
    json jFile;
    inFile >> jFile;
    highGainPedestal = jFile["Calibrations"]["SiPM"]["highGainPedestal"];
    highGainDpp = jFile["Calibrations"]["SiPM"]["highGainDpp"];
    lowGainPedestal = jFile["Calibrations"]["SiPM"]["lowGainPedestal"];
    lowGainDpp = jFile["Calibrations"]["SiPM"]["lowGainDpp"];
}

struct PMTCalibration{
    std::array<double,8> PMTSpd, PMTSpk, PMTCpd, PMTCpk;
    PMTCalibration(const std::string&);
};

PMTCalibration::PMTCalibration(const std::string& fname){
    std::ifstream inFile(fname,std::ifstream::in);
    json jFile;
    inFile >> jFile;
    PMTSpd = jFile["Calibrations"]["PMT"]["PMTS_pd"];
    PMTSpk = jFile["Calibrations"]["PMT"]["PMTS_pk"];
    PMTCpd = jFile["Calibrations"]["PMT"]["PMTC_pd"];
    PMTCpk = jFile["Calibrations"]["PMT"]["PMTC_pk"];
}

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
    double SiPMPheC[160] = {0};
    double SiPMPheS[160] = {0};
    double totSiPMPheC = 0.;
    double totSiPMPheS = 0.;

    void calibrate(const SiPMCalibration&);
    void calibratePMT(const PMTCalibration&);
};

void Event::calibrate(const SiPMCalibration& calibration){

    //SiPM calibration
    //
    int ccount = 0;
    int scount = 0;
    // TODO: please find a way to avoid theese counters :)
    for(uint16_t i=0;i<320;++i){      
        // If SiPM is 0 do not subtract pede and leave 0! (board was not triggered)
        if (SiPMHighGain[i] > 0){
            double highGainPe = (SiPMHighGain[i] - calibration.highGainPedestal[i]) / calibration.highGainDpp[i];
            double lowGainPe = (SiPMLowGain[i] - calibration.lowGainPedestal[i]) / calibration.lowGainDpp[i];
            double SiPMPhe = highGainPe * (int)(highGainPe < 140.) + lowGainPe * (int)(highGainPe > 140.);
            // use HG if pe < 140 else use LG. Use bool casting to avoid if/else branching
            if((i / 16) % 2 == 0){
                // Cher
                SiPMPheC[ccount] = SiPMPhe;
		totSiPMPheC += SiPMPhe;
                ccount++;
            } else {
                // Scin
            	SiPMPheS[scount] = SiPMPhe;
		totSiPMPheS += SiPMPhe;
            	scount++;
            }
        }
    }
   
}

void Event::calibratePMT(const PMTCalibration& pmtcalibration){

    //PMT calibration
    //
    SPMT1 = (SPMT1-pmtcalibration.PMTSpd[0]) * 14./(pmtcalibration.PMTSpk[0]-pmtcalibration.PMTSpd[0]);
    SPMT2 = (SPMT2-pmtcalibration.PMTSpd[1]) * 14./(pmtcalibration.PMTSpk[1]-pmtcalibration.PMTSpd[1]);
    SPMT3 = (SPMT3-pmtcalibration.PMTSpd[2]) * 14./(pmtcalibration.PMTSpk[3]-pmtcalibration.PMTSpd[0]);
    SPMT4 = (SPMT4-pmtcalibration.PMTSpd[3]) * 14./(pmtcalibration.PMTSpk[4]-pmtcalibration.PMTSpd[0]);
    SPMT5 = (SPMT5-pmtcalibration.PMTSpd[4]) * 14./(pmtcalibration.PMTSpk[5]-pmtcalibration.PMTSpd[0]);
    SPMT6 = (SPMT6-pmtcalibration.PMTSpd[5]) * 14./(pmtcalibration.PMTSpk[6]-pmtcalibration.PMTSpd[0]);
    SPMT7 = (SPMT7-pmtcalibration.PMTSpd[6]) * 14./(pmtcalibration.PMTSpk[7]-pmtcalibration.PMTSpd[0]);
    SPMT8 = (SPMT8-pmtcalibration.PMTSpd[7]) * 14./(pmtcalibration.PMTSpk[8]-pmtcalibration.PMTSpd[0]);
	
    CPMT1 = (CPMT1-pmtcalibration.PMTCpd[0]) * 14./(pmtcalibration.PMTCpk[0]-pmtcalibration.PMTCpd[0]);
    CPMT2 = (CPMT2-pmtcalibration.PMTCpd[1]) * 14./(pmtcalibration.PMTCpk[1]-pmtcalibration.PMTCpd[1]);
    CPMT3 = (CPMT3-pmtcalibration.PMTCpd[2]) * 14./(pmtcalibration.PMTCpk[3]-pmtcalibration.PMTCpd[0]);
    CPMT4 = (CPMT4-pmtcalibration.PMTCpd[3]) * 14./(pmtcalibration.PMTCpk[4]-pmtcalibration.PMTCpd[0]);
    CPMT5 = (CPMT5-pmtcalibration.PMTCpd[4]) * 14./(pmtcalibration.PMTCpk[5]-pmtcalibration.PMTCpd[0]);
    CPMT6 = (CPMT6-pmtcalibration.PMTCpd[5]) * 14./(pmtcalibration.PMTCpk[6]-pmtcalibration.PMTCpd[0]);
    CPMT7 = (CPMT7-pmtcalibration.PMTCpd[6]) * 14./(pmtcalibration.PMTCpk[7]-pmtcalibration.PMTCpd[0]);
    CPMT8 = (CPMT8-pmtcalibration.PMTCpd[7]) * 14./(pmtcalibration.PMTCpk[8]-pmtcalibration.PMTCpd[0]);
}

//**************************************************
