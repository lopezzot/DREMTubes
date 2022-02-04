//**************************************************
// \file PhysicsEvent.h
// \brief: implementation of Event class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
// 			@lopezzot
//          Edoardo Proserpio (Uni Insubria)
//          Iacopo Vivarelli (Uni Sussex)
// \start date: 20 August 2021
//**************************************************

#include <iostream>
#include <array>
#include <stdint.h>
#include <string>
#include "json.hpp"
#include <fstream>

#ifndef Event_H
#define Event_H

using json = nlohmann::json;

struct SiPMCalibration{
    std::array<double,320> highGainPedestal,highGainDpp,lowGainPedestal,lowGainDpp;
    double PheGeVS,PheGeVC;
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
    PheGeVS = jFile["Calibrations"]["SiPM"]["PhetoGeVS"];
    PheGeVC = jFile["Calibrations"]["SiPM"]["PhetoGeVC"];
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

struct DWCCalibration{
    std::array<double,4> DWC_sl, DWC_offs;
    double DWC_tons;
    std::array<double,2> DWC_z;
    DWCCalibration(const std::string&);
};

DWCCalibration::DWCCalibration(const std::string& fname){
    std::ifstream inFile(fname,std::ifstream::in);
    json jFile;
    inFile >> jFile;
    DWC_sl = jFile["Calibrations"]["DWC"]["DWC_sl"];
    DWC_offs = jFile["Calibrations"]["DWC"]["DWC_offs"];
    DWC_tons = jFile["Calibrations"]["DWC"]["DWC_tons"];
    DWC_z = jFile["Calibrations"]["DWC"]["DWC_z"];
}

class EventOut{
  public:
	EventOut(){};
	~EventOut(){};
	uint32_t EventID;

  float SPMT1, SPMT2, SPMT3, SPMT4, SPMT5, SPMT6, SPMT7, SPMT8;
  float CPMT1, CPMT2, CPMT3, CPMT4, CPMT5, CPMT6, CPMT7, CPMT8;
  float SiPMPheC[160] = {0};
  float SiPMPheS[160] = {0};
	float totSiPMCene = 0.;
	float totSiPMSene = 0.;
	float SPMTenergy = 0.;
	float CPMTenergy = 0.;
	float XDWC1,XDWC2,YDWC1,YDWC2;
	int PShower, MCounter, C1, C2;
  uint32_t NSiPMZero = 0;

	void CompSPMTene(){SPMTenergy = SPMT1+SPMT2+SPMT3+SPMT4+SPMT5+SPMT6+SPMT7+SPMT8;}
	void CompCPMTene(){CPMTenergy = CPMT1+CPMT2+CPMT3+CPMT4+CPMT5+CPMT6+CPMT7+CPMT8;}
};


class Event{

	public:
		//Constructor and de-constructor
		//
		Event(){};
		~Event(){};

		//Data members
		//
		int SPMT1, SPMT2, SPMT3, SPMT4, SPMT5, SPMT6, SPMT7, SPMT8;
		int CPMT1, CPMT2, CPMT3, CPMT4, CPMT5, CPMT6, CPMT7, CPMT8;
		int DWC1L, DWC1R, DWC1U, DWC1D, DWC2L, DWC2R, DWC2U, DWC2D;

		UShort_t SiPMHighGain[320];
		UShort_t SiPMLowGain[320];

		void calibrate(const SiPMCalibration&, EventOut*);
		void calibratePMT(const PMTCalibration&, EventOut*);
		void calibrateDWC(const DWCCalibration&, EventOut*);

};

void Event::calibrate(const SiPMCalibration& calibration, EventOut* evout){

	//SiPM calibration
	//
	int ccount = 0;
	int scount = 0;
	int nmiss=0;
	for(uint16_t i=0;i<320;++i){
		//  encode row and columns from i
		//  and zero output vectors
		const int column=i%16;
		const int row=i/16;
		// If SiPM is 0 do not subtract pede and leave 0! (board was not triggered)
		const double highGainPe = (SiPMHighGain[i] - calibration.highGainPedestal[i]) / calibration.highGainDpp[i];
		const double lowGainPe = (SiPMLowGain[i] - calibration.lowGainPedestal[i]) / calibration.lowGainDpp[i];
		double SiPMPhe = highGainPe * (int)(highGainPe < 140.) + lowGainPe * (int)(highGainPe > 140.);
		// use HG if pe < 140 else use LG. Use bool casting to avoid if/else branching
    if(highGainPe == 0){
      SiPMPhe = 0;
      NSiPMZero++;
    }
		if(row % 2 == 0){
			// Cher
			evout->SiPMPheC[ccount] = SiPMPhe/calibration.PheGeVC;
			evout->totSiPMCene += SiPMPhe/calibration.PheGeVC;
			ccount++;
		} else {
			// Scin
			evout->SiPMPheS[scount] = SiPMPhe/calibration.PheGeVS;
			evout->totSiPMSene += SiPMPhe/calibration.PheGeVS;
			scount++;
		}
	}
}

void Event::calibratePMT(const PMTCalibration& pmtcalibration, EventOut* evout){

    //PMT calibration
    //
    evout->SPMT1 = (SPMT1-pmtcalibration.PMTSpd[0]) * 15./(pmtcalibration.PMTSpk[0]-pmtcalibration.PMTSpd[0]);
    evout->SPMT2 = (SPMT2-pmtcalibration.PMTSpd[1]) * 15./(pmtcalibration.PMTSpk[1]-pmtcalibration.PMTSpd[1]);
    evout->SPMT3 = (SPMT3-pmtcalibration.PMTSpd[2]) * 15./(pmtcalibration.PMTSpk[2]-pmtcalibration.PMTSpd[2]);
    evout->SPMT4 = (SPMT4-pmtcalibration.PMTSpd[3]) * 15./(pmtcalibration.PMTSpk[3]-pmtcalibration.PMTSpd[3]);
    evout->SPMT5 = (SPMT5-pmtcalibration.PMTSpd[4]) * 15./(pmtcalibration.PMTSpk[4]-pmtcalibration.PMTSpd[4]);
    evout->SPMT6 = (SPMT6-pmtcalibration.PMTSpd[5]) * 15./(pmtcalibration.PMTSpk[5]-pmtcalibration.PMTSpd[5]);
    evout->SPMT7 = (SPMT7-pmtcalibration.PMTSpd[6]) * 15./(pmtcalibration.PMTSpk[6]-pmtcalibration.PMTSpd[6]);
    evout->SPMT8 = (SPMT8-pmtcalibration.PMTSpd[7]) * 15./(pmtcalibration.PMTSpk[7]-pmtcalibration.PMTSpd[7]);

    evout->CPMT1 = (CPMT1-pmtcalibration.PMTCpd[0]) * 15./(pmtcalibration.PMTCpk[0]-pmtcalibration.PMTCpd[0]);
    evout->CPMT2 = (CPMT2-pmtcalibration.PMTCpd[1]) * 15./(pmtcalibration.PMTCpk[1]-pmtcalibration.PMTCpd[1]);
    evout->CPMT3 = (CPMT3-pmtcalibration.PMTCpd[2]) * 15./(pmtcalibration.PMTCpk[2]-pmtcalibration.PMTCpd[2]);
    evout->CPMT4 = (CPMT4-pmtcalibration.PMTCpd[3]) * 15./(pmtcalibration.PMTCpk[3]-pmtcalibration.PMTCpd[3]);
    evout->CPMT5 = (CPMT5-pmtcalibration.PMTCpd[4]) * 15./(pmtcalibration.PMTCpk[4]-pmtcalibration.PMTCpd[4]);
    evout->CPMT6 = (CPMT6-pmtcalibration.PMTCpd[5]) * 15./(pmtcalibration.PMTCpk[5]-pmtcalibration.PMTCpd[5]);
    evout->CPMT7 = (CPMT7-pmtcalibration.PMTCpd[6]) * 15./(pmtcalibration.PMTCpk[6]-pmtcalibration.PMTCpd[6]);
    evout->CPMT8 = (CPMT8-pmtcalibration.PMTCpd[7]) * 15./(pmtcalibration.PMTCpk[7]-pmtcalibration.PMTCpd[7]);
}

void Event::calibrateDWC(const DWCCalibration& dwccalibration, EventOut* evout){
    evout->XDWC1 = (DWC1R-DWC1L)*dwccalibration.DWC_sl[0]*dwccalibration.DWC_tons+dwccalibration.DWC_offs[0];
    evout->YDWC1 = (DWC1D-DWC1U)*dwccalibration.DWC_sl[1]*dwccalibration.DWC_tons+dwccalibration.DWC_offs[1];
    evout->XDWC2 = (DWC2R-DWC2L)*dwccalibration.DWC_sl[2]*dwccalibration.DWC_tons+dwccalibration.DWC_offs[2];
    evout->YDWC2 = (DWC2D-DWC2U)*dwccalibration.DWC_sl[3]*dwccalibration.DWC_tons+dwccalibration.DWC_offs[3];
}
#endif

//**************************************************
