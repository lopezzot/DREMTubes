//root -l 'DoCalibrationSPS.C'
//pmt_json.txt is produced
//it contains the calibration constants for PMT towers and p.e. to GeV conversion factors for SiPM tower

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <TTree.h>
#include <TBranch.h>
#include <TFile.h>
#include <TH1.h>
#include <TMath.h>
#include "../scripts/json.hpp"
#include "../scripts/PhysicsEvent.h"

using json = nlohmann::json;
ClassImp(EventOut)

#define DATADIR  "/eos/user/i/ideadr/TB2021_H8/CERNDATA/v1.3/mergedNtuple/"
//#define OUTDIR "/afs/cern.ch/user/j/jagarwal/workspace/public/emsizedPT/DREMTubes/TBDataPreparation/202108_SPS/PMT/textfiles/calibV1.3.5/"

void DoCalibrationSPS(){

	vector<float> peakFinder(int run, int runno, int ch_S, int ch_C,  string file);

	//PMT ADC mapping (C1-C8,S1-S8)
	int mych[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

	std:vector<int> runs;
	runs={670,660,662,663,668,664,667,666,665}; //beam shot at tower0,...,tower8
	
	cout<<"run series starting with " <<runs[0] << endl; 

	// write pedestal
        ostringstream ped;
        ped << OUTDIR << "pedestal.txt";
        cout<<"pedestal output file: "<< ped.str() << endl;
	
	// write equalisation
        ostringstream equal;
        equal << OUTDIR << "equalisation.txt";
        cout<<"equalisation output file: "<< equal.str() << endl;

	// vectors for storing peaks and pedestals for S and C  for eight the PMT towers
	std::vector<float> myAdc; 
	std::vector<float> ped_s; 
	std::vector<float> ped_c;
	std::vector<float> myAdc_s; 
	std::vector<float> myAdc_c;
	std::vector<float> eq_s;
	std::vector<float> eq_c;
 
	int c_idx=0;
	int s_idx=0;

	cout << " equalization .... " << endl; 

	// loop on towers to get all ped and S and C peaks
	for(int tow=1; tow<9; tow++){ 

 	   // index for ADC channels	
           c_idx= mych[tow-1]; 
	   s_idx= mych[tow+7];

	   myAdc = peakFinder (tow, runs[tow], s_idx, c_idx,  ped.str());	
	   ped_s.push_back(myAdc.at(0));
           ped_c.push_back(myAdc.at(1));
	   myAdc_s.push_back(myAdc.at(2));
           myAdc_c.push_back(myAdc.at(3));
	}// end loop on towers


	// using data file for T0
	TFile *f;
	ostringstream infile;
        infile <<  DATADIR <<  "merged_sps2021_run" << std::to_string(runs[0]) << ".root";
        std::cout<<"Using file: "<<infile.str()<<std::endl;
        f=new TFile(infile.str().c_str());
        TTree *t = (TTree*) f->Get("CERNSPS2021");
        TTree *tSIPM = (TTree*) f->Get("SiPMSPS2021");
        cout <<"T0: tPMT entries  "<< t->GetEntries()<< "  T0: tSIPM entries " << tSIPM->GetEntries() << endl;

	// using SiPM Calibration from Class Event
	auto ev = new Event();
        auto evout = new EventOut();

	//SiPMCalibration sipmCalibration("RunXXX.json");
	SiPMCalibration sipmCalibration("RunXXXcalib_v1.3.5.json");

	// SiPM branches
	tSIPM->SetBranchAddress("HG_Board0",&ev->SiPMHighGain[0]);
	tSIPM->SetBranchAddress("HG_Board1",&ev->SiPMHighGain[64]);
	tSIPM->SetBranchAddress("HG_Board2",&ev->SiPMHighGain[128]);
 	tSIPM->SetBranchAddress("HG_Board3",&ev->SiPMHighGain[192]);
 	tSIPM->SetBranchAddress("HG_Board4",&ev->SiPMHighGain[256]);
 	tSIPM->SetBranchAddress("LG_Board0",&ev->SiPMLowGain[0]);
 	tSIPM->SetBranchAddress("LG_Board1",&ev->SiPMLowGain[64]);
 	tSIPM->SetBranchAddress("LG_Board2",&ev->SiPMLowGain[128]);
 	tSIPM->SetBranchAddress("LG_Board3",&ev->SiPMLowGain[192]);
 	tSIPM->SetBranchAddress("LG_Board4",&ev->SiPMLowGain[256]);

        //int ADC[32];
	int ADC[96];
        Long64_t TriggerMask =0;
        t->SetBranchAddress("ADCs",&ADC);
        t->SetBranchAddress("TriggerMask",&TriggerMask);


	Float_t sumc=0; 
	Float_t sums=0; 

	TH1F *h_SumC_SIPM = new TH1F("SumC_SIPM", "SumC_SIPM", 2048, 0, 4096);
        h_SumC_SIPM->GetXaxis()->SetTitle("ADC");

        TH1F *h_SumS_SIPM = new TH1F("SumS_SIPM", "SumS_SIPM", 2000, 0, 6000);
        h_SumS_SIPM->GetXaxis()->SetTitle("ADC");
	
	int counter2=0;
	// loop on ntuple entries to get adc peak position in T0 for equalization 
	for( unsigned int i=0; i<t->GetEntries(); i++){
	
           tSIPM->GetEntry(i);	
           t->GetEntry(i);

	   int ps=16;
           int c1=64;
           int c2=65;
           float pd = 210;
           float pd1 = 78.5;
           float pd2 = 15.8;
           double Ch1=ADC[c1]-pd1;
           double Ch2=ADC[c2]-pd2;
           double PSP=ADC[ps]-pd;
           double MIP=60.;
           double pscut_4point5mip=4.5*MIP;
           bool pscut=PSP>pscut_4point5mip;
	   bool chercut_loose = (Ch1>2 || Ch2 >10);
           bool chercut_tight = (Ch1>5 || Ch2 >18);
	   // Calibration of SiPM
	   ev->calibrate(sipmCalibration, evout);
	   if (i%50000 ==0) cout << i << " " << evout->totSiPMSene << " " << evout->totSiPMCene<< endl; 

	   // Sum of calibrated SiPM energy deposition
           if(TriggerMask == 5){   // phys histo 
	     //if(pscut && chercut_loose){// lateral leakage for PS>4.5mip
	     if(chercut_tight){// event selection: tight cuts on both Cherenkov counters
	       counter2++;
	       h_SumS_SIPM->Fill(evout->totSiPMSene);
	       h_SumC_SIPM->Fill(evout->totSiPMCene);
	     }
	   }// end phys case


	   //reset SiPM sum value 
	   evout->totSiPMCene = 0;
    	   evout->totSiPMSene = 0;

	} // end for on events

	TH1F *hsipmS = (TH1F*)h_SumS_SIPM->Clone("hsipmS");
	TH1F *hsipmC = (TH1F*)h_SumC_SIPM->Clone("hsipmC");

        TCanvas *sipm = new TCanvas("SiPM", "SiPM", 1000, 700);
	sipm->Divide(1,2);
	sipm->cd(1);
	gPad->SetLogy();
	h_SumS_SIPM->GetXaxis()->SetRangeUser(100, 6000);
	h_SumS_SIPM->Draw();
        int binmaxSSIPM = h_SumS_SIPM->GetMaximumBin();
        double adc_SSIPM = h_SumS_SIPM->GetXaxis()->GetBinCenter(binmaxSSIPM);
	sipm->cd(2);
	gPad->SetLogy();
	h_SumC_SIPM->GetXaxis()->SetRangeUser(100, 1000);
	h_SumC_SIPM->Draw();
        int binmaxCSIPM = h_SumC_SIPM->GetMaximumBin();
        double adc_CSIPM = h_SumC_SIPM->GetXaxis()->GetBinCenter(binmaxCSIPM);
  
        myAdc_s.push_back(adc_SSIPM);
        myAdc_c.push_back(adc_CSIPM);

	// calculate equalization constants
	cout << "myAdc size:  " << myAdc_s.size() << "	" << myAdc_c.size() <<  endl; 
	for(int i=0; i< myAdc_s.size(); i++){
		cout <<"adc "  << i << " " << myAdc_s.at(i) << " " <<  myAdc_c.at(i) << endl;	
		eq_s.push_back(myAdc_s.at(i)/myAdc_s.at(8));
		eq_c.push_back(myAdc_c.at(i)/myAdc_c.at(8));
	}
        ofstream eqfile;
        eqfile.open(equal.str(), ofstream::out | ofstream::app);
	cout << "equalization constant " << endl; 
	for(int i=0; i< eq_s.size(); i++){
		cout <<"eq:  " << i << " : " << eq_s.at(i) << "  " <<  eq_c.at(i) << endl;
		eqfile <<  eq_s.at(i) << " " <<  eq_c.at(i) << endl;
	}

	cout << "calibrating now "<< endl; 

        TH1F *h[16];
        int nbin =2048;
        int xlow = 0;
        int xhigh =4096;

	// Histograms per Tower	
        for(int n=0;n<8; n++){
                stringstream hname;
                stringstream htitle;
                hname<<"h_C_"<<n+1;
                htitle << "Cher_"<<n+1;
                h[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin, xlow, xhigh);
                h[n]->GetXaxis()->SetTitle("ADC counts");
        }
	for(int n=8;n<16; n++){
                stringstream hname;
                stringstream htitle;
                hname<<"h_S_"<<n-7;
                htitle << "Scint_"<<n-7;
                h[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin, xlow, xhigh);
                h[n]->GetXaxis()->SetTitle("ADC counts");
	}

        TH1F *h_SumC = new TH1F("SumC", "SumC", 400, 0, 2000);
        h_SumC->GetXaxis()->SetTitle("ADC Counts");

        TH1F *h_SumS = new TH1F("SumS", "SumS", 2000, 0, 8000);
        h_SumS->GetXaxis()->SetTitle("ADC Counts");

	int counter3=0;
	for( unsigned int i=0; i<t->GetEntries(); i++){
	
           tSIPM->GetEntry(i);	
           t->GetEntry(i);

	   int ps=16;
           int c1=64;
           int c2=65;
           float pd = 210;
           float pd1 = 78.5;
           float pd2 = 15.8;
           double Ch1=ADC[c1]-pd1;
           double Ch2=ADC[c2]-pd2;
           double PSP=ADC[ps]-pd;
           double MIP=60.;
           double pscut_4point5mip=4.5*MIP;
           bool pscut=PSP>pscut_4point5mip;
	   bool chercut_loose = (Ch1>2 || Ch2 >10);
           bool chercut_tight = (Ch1>5 || Ch2 >18);

	   // Sum of calibrated SiPM energy deposition
           if(TriggerMask==5){   // physics trigger 
	     //if(pscut && chercut_loose){
	     if(chercut_tight){ // event selection: tight cuts on both Cherenkov counters
	       counter3++;
	       for(int tow=1; tow<9; tow++){

                 // index for ADC channels       
                 c_idx= mych[tow-1];
                 s_idx= mych[tow+7];

		 // sum up on all the towers (ADC counts)
		 sumc+=(ADC[c_idx]-ped_c.at(tow-1))/eq_c.at(tow-1);
		 sums+=(ADC[s_idx]-ped_s.at(tow-1))/eq_s.at(tow-1);

		 //per tower histo, adc distributiom, ped substracted and equalised
                 h[tow-1]->Fill((ADC[c_idx]-ped_c.at(tow-1))/eq_c.at(tow-1));
                 h[tow+7]->Fill((ADC[s_idx]-ped_s.at(tow-1))/eq_s.at(tow-1));

	       } // end loop on tower

	       // Calibration of SiPM
	       ev->calibrate(sipmCalibration, evout);
	       //if (i%50000 ==0) cout << i << " " << evout->totSiPMSene << " " << evout->totSiPMCene<< endl; 
	       if (i%50000 ==0) cout << i << " " << evout->totSiPMSene << " " << evout->totSiPMCene<< endl;
	       sums+= evout->totSiPMSene;
	       sumc+= evout->totSiPMCene;
	     }
	   }// end phys case

	   h_SumC->Fill(sumc);
	   h_SumS->Fill(sums);

	   //reset SiPM sum value 
	   evout->totSiPMCene = 0;
    	   evout->totSiPMSene = 0;
	   //reset PMT sum value 
	   sumc=0;
	   sums=0;

	} // end for on events

	cout<<"counter2: "<<counter2<<endl;
	cout<<"counter3: "<<counter3<<endl;

	gStyle->SetOptStat(111111);
	gStyle->SetStatX(0.90);
	gStyle->SetStatY(0.90);

	TCanvas *c_all = new TCanvas("c_distrib", "c_distrib", 1000, 700);
	c_all->Divide(3,3); 
        c_all->cd(1);
        gPad->SetLogy();
        h[7]->Draw();
        c_all->cd(2);
        gPad->SetLogy();
        h[6]->Draw();
        c_all->cd(3);
        gPad->SetLogy();
        h[5]->Draw();
        c_all->cd(4);
        gPad->SetLogy();
        h[4]->Draw();
        c_all->cd(5);
        gPad->SetLogy();
	hsipmC->Draw(); 	
	hsipmC->GetXaxis()->SetRangeUser(100, 1000);
	c_all->cd(6);
        gPad->SetLogy();
        h[3]->Draw();
        c_all->cd(7);
        gPad->SetLogy();
        h[2]->Draw();
        c_all->cd(8);
        gPad->SetLogy();
        h[1]->Draw();
        c_all->cd(9);
        gPad->SetLogy();
        h[0]->Draw();
	//c_all->SaveAs("C_equalized.pdf");
	//c_all->SaveAs("C_equalized.png");

	TCanvas *s_all = new TCanvas("s_distrib", "s_distrib", 1000, 700);
	s_all->Divide(3,3); 
        s_all->cd(1);
        gPad->SetLogy();
        h[15]->Draw();
        s_all->cd(2);
        gPad->SetLogy();
        h[14]->Draw();
        s_all->cd(3);
        gPad->SetLogy();
        h[13]->Draw();
        s_all->cd(4);
        gPad->SetLogy();
        h[12]->Draw();
        s_all->cd(5);
        gPad->SetLogy();
	hsipmS->Draw(); 	
	hsipmS->GetXaxis()->SetRangeUser(100, 6000);
        s_all->cd(6);
        gPad->SetLogy();
        h[11]->Draw();
        s_all->cd(7);
        gPad->SetLogy();
        h[10]->Draw();
        s_all->cd(8);
        gPad->SetLogy();
        h[9]->Draw();
        s_all->cd(9);
        gPad->SetLogy();
        h[8]->Draw();
	//s_all->SaveAs("S_equalized.pdf");
        //s_all->SaveAs("S_equalized.png");

        TCanvas *sum = new TCanvas("sum_distrib", "sum_distrib", 1000, 700);
	sum->Divide(2,1);
	sum->cd(1);
	gPad->SetLogy();
	h_SumS->GetXaxis()->SetRangeUser(4000, 8000);
	h_SumS->Draw();
        int binmaxS = h_SumS->GetMaximumBin();
        double E_S = h_SumS->GetXaxis()->GetBinCenter(binmaxS);
	sum->cd(2);
	gPad->SetLogy();
	h_SumC->GetXaxis()->SetRangeUser(400, 2000);
	h_SumC->Draw();
        int binmaxC = h_SumC->GetMaximumBin();
        double E_C = h_SumC->GetXaxis()->GetBinCenter(binmaxC);

	//sum->SaveAs("Sum-PMT-SiPM.pdf");
        //sum->SaveAs("Sum-PMT-SiPM.png");

	float Ks, Kc; 
	std::vector<float> CS;
	std::vector<float> CC;

	Float_t totE = 18.8; // Energy contained in the module 0.94*20; should be different for S and C 


        // scaling factor (ADC/GeV)	
	Ks = E_S/totE;
	Kc = E_C/totE;

	cout << "Scaling factors:  " << endl; 
	cout<<"mean of sum ADC S: "<< E_S << "  Ks: " << Ks << endl;  
	cout<<"mean of sum ADC C: "<< E_C << "  Kc: " << Kc << endl;  

		
	cout << "Equalizazion and calibration constant " << endl; 
	for(int i=0; i< eq_s.size(); i++){
	   cout <<"EQ " << eq_s.at(i) << " " <<  eq_c.at(i) << endl;
	   CS.push_back(Ks*eq_s.at(i)); 
	   CC.push_back(Kc*eq_c.at(i));
	   cout <<"Calib " << CS.at(i) << " " <<  CC.at(i) << endl;
	}

	// write calibration constant for JSON file
        ostringstream pmtjsonfile;
        pmtjsonfile << OUTDIR << "pmt_json.txt" ;
        cout << "pmtjson output file: " << endl << pmtjsonfile.str() << endl;
	ofstream pmtjson;
	pmtjson.open(pmtjsonfile.str(), ofstream::out | ofstream::app);

	pmtjson << " \"PMTS_pd\": [" << ped_s.at(0) <<", " << ped_s.at(1) <<", " << ped_s.at(2) <<", " <<ped_s.at(3) <<", " <<ped_s.at(4) <<", " <<ped_s.at(5) <<", " <<ped_s.at(6) <<", " <<ped_s.at(7) <<"], " << endl;    
	pmtjson << " \"PMTS_pk\": [" << CS.at(0) <<", " << CS.at(1) <<", " << CS.at(2) <<", " <<CS.at(3) <<", " <<CS.at(4) <<", " <<CS.at(5) <<", " <<CS.at(6) <<", " <<CS.at(7) <<"], " << endl;    
	pmtjson << " \"PMTC_pd\": [" << ped_c.at(0) <<", " << ped_c.at(1) <<", " << ped_c.at(2) <<", " <<ped_c.at(3) <<", " <<ped_c.at(4) <<", " <<ped_c.at(5) <<", " <<ped_c.at(6) <<", " <<ped_c.at(7) <<"], " << endl;    
	pmtjson << " \"PMTC_pk\": [" << CC.at(0) <<", " << CC.at(1) <<", " << CC.at(2) <<", " <<CC.at(3) <<", " <<CC.at(4) <<", " <<CC.at(5) <<", " <<CC.at(6) <<", " <<CC.at(7) <<"] " << endl;    


	pmtjson << " \"PhetoGeVS\": [" << CS.at(8) <<"], " << endl;    
	pmtjson << " \"PhetoGeVC\": [" << CC.at(8) <<"] " << endl;    

	return; 
 }

// find average pedestal value and peak of the distribution for each tower, for both C and S
std::vector<float> peakFinder(int tow, int runno, int s_idx, int c_idx,  string pedfile){

	std::vector<float> myadc;
	TFile *f;

	cout << "tow: " << tow << "  c_idx:  " << c_idx << " s_idx:  " << s_idx << endl;

	// write pedestal value to file
        ofstream myPed;
	myPed.open(pedfile, ofstream::out | ofstream::app);

	//input file
	ostringstream infile;
	infile <<  DATADIR <<  "merged_sps2021_run" << std::to_string(runno) << ".root";
	std::cout<<"Using file: "<<infile.str()<<std::endl;
	f=new TFile(infile.str().c_str());
	TTree *t = (TTree*) f->Get("CERNSPS2021");
	cout <<"Inside PeakFinder Method- total Entries: "<< t->GetEntries()<< endl;


	//Allocate branch pointer
	int ADC[96];
	Long64_t TriggerMask =0;
	t->SetBranchAddress("ADCs",&ADC);
	t->SetBranchAddress("TriggerMask",&TriggerMask);
	
	
	int nbin =4096;
        int xlow = 0;
        int xhigh =4096;
        int npbin =270;
        int xplow = 0;
        int xphigh =350;
        

	TH1F *h_adc_S = new TH1F("adc distrib S", "adc_distribS", nbin, xlow,xhigh);
	h_adc_S->GetXaxis()->SetTitle("ADC counts");

	TH1F *h_ped_S =new TH1F("ped distrib S", "ped_distribS", npbin, xplow, xphigh);
	h_ped_S ->GetXaxis()->SetTitle("ADC counts");

        TH1F *h_adc_C =new TH1F("adc distrib C", "adc_distribC", nbin, xlow,xhigh);
        h_adc_C ->GetXaxis()->SetTitle("ADC counts");

        TH1F *h_ped_C =new TH1F("ped distrib C", "ped_distribC", npbin, xplow, xphigh);
        h_ped_C ->GetXaxis()->SetTitle("ADC counts");


	//Loop over events for pedestal 
	for( unsigned int i=0; i<t->GetEntries(); i++){
	   
	   t->GetEntry(i);
	
	   if(TriggerMask == 6){   // pedestal events
		h_ped_S->Fill(ADC[s_idx]);			
		h_ped_C->Fill(ADC[c_idx]);			
	   }
	} // end loop for ped

	Float_t s_ped =  h_ped_S->GetMean();
	Float_t c_ped =  h_ped_C->GetMean();
	
        cout << s_ped << " " << c_ped << endl;
	myPed << s_ped << "\t " << c_ped << endl;

	int counter1=0;	
	// Loop over events for ADC
        for( unsigned int i=0; i<t->GetEntries(); i++){

          t->GetEntry(i);
	  int ps=16;
          int c1=64;
          int c2=65;
          float pd = 210;
          float pd1 = 78.5;
          float pd2 = 15.8;
	  double Ch1=ADC[c1]-pd1;
          double Ch2=ADC[c2]-pd2;
          double PSP=ADC[ps]-pd;
          double MIP=60.;
          double pscut_4point5mip=4.5*MIP;
          bool pscut=PSP>pscut_4point5mip;
	  bool chercut_tight = (Ch1>5 || Ch2 >18);

	  if(TriggerMask == 5){   // physics trigger
	    if(chercut_tight){ // event selection: tights cuts on both Cherenkov counters
		counter1++;
		h_adc_S->Fill(ADC[s_idx]-s_ped);			
		h_adc_C->Fill(ADC[c_idx]-c_ped);			
	    }
	  }
	} // end loop on event

	cout<<"counter1:  "<<counter1<<endl;
        TCanvas *c1=new TCanvas("mytower","mytower",1000, 1000);
        c1->Divide(2,2);
        c1->cd(1);
        gPad->SetLogy();
        h_ped_S->Draw();
        c1->cd(2);
        gPad->SetLogy();
        h_ped_C->Draw();
        c1->cd(3);
        gPad->SetLogy();
        h_adc_S->Draw();
        c1->cd(4);
        gPad->SetLogy();
        h_adc_C->Draw();
	myadc.push_back(s_ped);
	myadc.push_back(c_ped);

        TCanvas *c10 = new TCanvas("fit pan", "fit pan", 1000, 700);
        c10->Divide(1,2);
        c10->cd(1);
        gPad->SetLogy();
        h_adc_S->Draw();
        h_adc_S->GetXaxis()->SetRangeUser(400, 2000);//150 changed
        int binmax1 = h_adc_S->GetMaximumBin();
        double x_S = h_adc_S->GetXaxis()->GetBinCenter(binmax1);
        c10->cd(2);
        gPad->SetLogy();
        h_adc_C->Draw();
	h_adc_C->GetXaxis()->SetRangeUser(400, 2000);
        int binmax2 = h_adc_C->GetMaximumBin();
        double x_C = h_adc_C->GetXaxis()->GetBinCenter(binmax2);
        c10->Update();
        cout << x_S << " " << x_C << endl;
	myadc.push_back(x_S);
	myadc.push_back(x_C);

	return myadc;
}
