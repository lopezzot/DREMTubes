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
    std::array<double,1> PheGeVS,PheGeVC;
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
	int NSiPMZero= 0.;
	float SPMTenergy = 0.;
	float CPMTenergy = 0.;

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

		UShort_t SiPMHighGain[320];
		UShort_t SiPMLowGain[320];

		void calibrate(const SiPMCalibration&, EventOut*);
		void calibratePMT(const PMTCalibration&, EventOut*);

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
		int column=i%16;
		int row=i/16;
		int ind=(row/2)*16+column;
		if(row % 2 == 0)evout->SiPMPheC[ind]=0;
		if(row % 2 == 1)evout->SiPMPheS[ind]=0;
		// Count cells where Sipm is zero
		if (SiPMHighGain[i] <= 0) nmiss++;
		// If SiPM is 0 do not subtract pede and leave 0! (board was not triggered)
		//          cout << " Missing cell  " << i << endl;
		if (SiPMHighGain[i] > 0){
			double highGainPe = (SiPMHighGain[i] - calibration.highGainPedestal[i]) / calibration.highGainDpp[i];
			double lowGainPe = (SiPMLowGain[i] - calibration.lowGainPedestal[i]) / calibration.lowGainDpp[i];
			double SiPMPhe = highGainPe * (int)(highGainPe < 140.) + lowGainPe * (int)(highGainPe > 140.);
			//std::cout<<"sipm "<<i<<" hg "<<SiPMHighGain[i]<<" lg "<<SiPMLowGain[i]<<" hgpe "<<highGainPe<<" lgpe "<<lowGainPe<<" phe "<<SiPMPhe<<" hgped "<<calibration.highGainPedestal[i]<<" hgdpp "<<calibration.highGainDpp[i]<<" lgped "<<calibration.lowGainPedestal[i]<<" hgdpp "<<calibration.lowGainDpp[i]<<std::endl;
			// use HG if pe < 140 else use LG. Use bool casting to avoid if/else branching
			if(row % 2 == 0){
				// Cher
				evout->SiPMPheC[ind] = SiPMPhe/calibration.PheGeVC[0];
				evout->totSiPMCene += SiPMPhe/calibration.PheGeVC[0];
//				if(ind != ccount && nmiss==0)
//					cout << " ind " << ind << " ccount " << ccount << endl;
				ccount++;
			} else {
				// Scin
				evout->SiPMPheS[ind] = SiPMPhe/calibration.PheGeVS[0];
				evout->totSiPMSene += SiPMPhe/calibration.PheGeVS[0];
//				if(ind != scount && nmiss==0)
//					cout << " ind " << ind << " scount " << scount << endl;
				scount++;
			}
		}
	}
	evout->NSiPMZero=nmiss;
}

void Event::calibratePMT(const PMTCalibration& pmtcalibration, EventOut* evout){

    //PMT calibration
    //
    evout->SPMT1 = (SPMT1-pmtcalibration.PMTSpd[0])/(pmtcalibration.PMTSpk[0]);
    evout->SPMT2 = (SPMT2-pmtcalibration.PMTSpd[1])/(pmtcalibration.PMTSpk[1]);
    evout->SPMT3 = (SPMT3-pmtcalibration.PMTSpd[2])/(pmtcalibration.PMTSpk[2]);
    evout->SPMT4 = (SPMT4-pmtcalibration.PMTSpd[3])/(pmtcalibration.PMTSpk[3]);
    evout->SPMT5 = (SPMT5-pmtcalibration.PMTSpd[4])/(pmtcalibration.PMTSpk[4]);
    evout->SPMT6 = (SPMT6-pmtcalibration.PMTSpd[5])/(pmtcalibration.PMTSpk[5]);
    evout->SPMT7 = (SPMT7-pmtcalibration.PMTSpd[6])/(pmtcalibration.PMTSpk[6]);
    evout->SPMT8 = (SPMT8-pmtcalibration.PMTSpd[7])/(pmtcalibration.PMTSpk[7]);
	
    evout->CPMT1 = (CPMT1-pmtcalibration.PMTCpd[0])/(pmtcalibration.PMTCpk[0]);
    evout->CPMT2 = (CPMT2-pmtcalibration.PMTCpd[1])/(pmtcalibration.PMTCpk[1]);
    evout->CPMT3 = (CPMT3-pmtcalibration.PMTCpd[2])/(pmtcalibration.PMTCpk[2]);
    evout->CPMT4 = (CPMT4-pmtcalibration.PMTCpd[3])/(pmtcalibration.PMTCpk[3]);
    evout->CPMT5 = (CPMT5-pmtcalibration.PMTCpd[4])/(pmtcalibration.PMTCpk[4]);
    evout->CPMT6 = (CPMT6-pmtcalibration.PMTCpd[5])/(pmtcalibration.PMTCpk[5]);
    evout->CPMT7 = (CPMT7-pmtcalibration.PMTCpd[6])/(pmtcalibration.PMTCpk[6]);
    evout->CPMT8 = (CPMT8-pmtcalibration.PMTCpd[7])/(pmtcalibration.PMTCpk[7]);
}

#endif

//**************************************************
