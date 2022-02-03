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


#define DATADIR  "/eos/user/i/ideadr/TB2021_Desy/mergedNtuple/"
#define OUTDIR  "/eos/user/i/ideadr/TB2021_Desy/PMT_calibration/"

void DoPlot(int myconf){
	// myconf flag allows to switch among the three configuration with which DESY data has been taken


	//PMT ADC mapping (C1-C8,S1-S8)
	int mych[16]={0,2,4,6,8,10,12,14,16,18,21,22,24,27,28,31};

	// select variables as function of the calib
	std:vector<int> runs;
	switch(myconf){ 
		case 0 : // yellow filter
		  runs={73,79,80,81,82,83,84,85,86};
		  break;
		case 1 : // no yellow filter
		  runs={180,182,183,185,190,186,189,188,187};
		  break;
		case 2 : // no yellow filter and PMT_C ampliefied	
		  runs={266,274,275,276,273,277,280,279,278};
		  break;
	}
	
	cout << myconf << " run series starting with " <<runs[0] << endl; 


	// vectors for storing peaks and pedestals for C and S  for all the PMT towers
	std::vector<float> ped_s; 
	std::vector<float> ped_c;
	std::vector<float> eq_s;
	std::vector<float> eq_c;
	float a,b;


        ostringstream ped;
        ped << OUTDIR  <<  "pedestal_" << myconf << ".txt";
        cout << ped.str().c_str() << std::endl;

        std::ifstream in0(ped.str());
        if (!in0 || in0.bad()) return 0; // sanity check

        while (1) {
              in0 >> a >> b ;
              if (!in0.good()) break;
              ped_s.push_back(a);
              ped_c.push_back(b);
        }

        ostringstream eq;
        eq << OUTDIR  <<  "equal_" << myconf << ".txt";
        cout << eq.str().c_str() << std::endl;

        std::ifstream in1(eq.str());
        if (!in1 || in1.bad()) return 0; // sanity check

        while (1) {
              in1 >> a >> b ;
              if (!in1.good()) break;
              eq_s.push_back(a);
              eq_c.push_back(b);
        }

	cout << "check constant " << endl; 
	for(int i=0; i< ped_s.size(); i++){
		cout <<"eq " << eq_s.at(i) << " " <<  eq_c.at(i) << "  ped "  << ped_s.at(i) << " " <<  ped_c.at(i) << endl;
	}

 
	int c_idx=0;
	int s_idx=0;

        TH1F *h0[16];
        TH1F *h[16];
        int nbin =2048;
        int xlow = 0;
        int xhigh =4096;

	// Histograms per Tower	
        for(int n=0;n<8; n++){
                stringstream hname;
                stringstream htitle;
                hname<<"h0_C_"<<n+1;
                htitle << "0_Cher_"<<n+1;
                if(myconf==2) h0[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin, xlow,xhigh);
                else h0[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin/4, xlow,nbin/2);
                h0[n]->GetXaxis()->SetTitle("ADC counts");
        }
	for(int n=8;n<16; n++){
                stringstream hname;
                stringstream htitle;
                hname<<"h0_S_"<<n-7;
                htitle << "0_Scint_"<<n-7;
                if(myconf==0) h0[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin/4, xlow, nbin/2);
                else h0[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin, xlow, xhigh);
                h0[n]->GetXaxis()->SetTitle("ADC counts");
	}

	
        // Histograms per Tower	
        for(int n=0;n<8; n++){
                stringstream hname;
                stringstream htitle;
                hname<<"h_C_"<<n+1;
                htitle << "Cher_"<<n+1;
                if(myconf==2) h[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin, xlow,xhigh);
                else h[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin/4, xlow,nbin/2);
                h[n]->GetXaxis()->SetTitle("ADC counts");
        }
	for(int n=8;n<16; n++){
                stringstream hname;
                stringstream htitle;
                hname<<"h_S_"<<n-7;
                htitle << "Scint_"<<n-7;
                if(myconf==0) h[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin/4, xlow, nbin/2);
                else h[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin, xlow, xhigh);
                h[n]->GetXaxis()->SetTitle("ADC counts");
	}

	TH1F *h_SumC_SIPM = new TH1F("SumC_SIPM", "SumC_SIPM", 2048/4, 0, 4096/4);
        h_SumC_SIPM->GetXaxis()->SetTitle("Energy (GeV)");

        TH1F *h_SumS_SIPM = new TH1F("SumS_SIPM", "SumS_SIPM", 2048, 0, 4096);
        h_SumS_SIPM->GetXaxis()->SetTitle("Energy (GeV)");




	// using data file with beam steered in the center
	TFile *f;
	ostringstream infile;

	for(int tow=0; tow<9; tow++ ) {
	cout << "tow " << tow << endl; 
        
        infile.str(""); 
        infile <<  DATADIR <<  "merged_desy2021_run" << std::to_string(runs[tow]) << ".root";
        std::cout<<"Using file: "<<infile.str()<<std::endl;
        f=new TFile(infile.str().c_str());
        TTree *t = (TTree*) f->Get("DESY2021");
        TTree *tSIPM = (TTree*) f->Get("SiPMDESY2021");
        cout << t->GetEntries()<< " " << tSIPM->GetEntries() << endl;

	// using SiPM Calibration from Class Event
	auto ev = new Event();
        auto evout = new EventOut();

	SiPMCalibration sipmCalibration("RunXXXcalib.json");

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

        int ADC[32];
        Long64_t TriggerMask =0;
        t->SetBranchAddress("ADCs",&ADC);
        t->SetBranchAddress("TriggerMask",&TriggerMask);


	Float_t sumc=0; 
	Float_t sums=0; 

	if (tow ==0){

	cout << " looking at the sipm, tower " << tow << endl;
	// loop on ntuple entries to get adc peak position in T0 for equalization 
	for( unsigned int i=0; i<t->GetEntries(); i++){
	
           tSIPM->GetEntry(i);	
           t->GetEntry(i);

	   // Calibration of SiPM
	   ev->calibrate(sipmCalibration, evout);
	   if (i%50000 ==0) cout << i << " " << evout->totSiPMSene << " " << evout->totSiPMCene<< endl; 

	   // Sum of calibrated SiPM energy deposition
           if(TriggerMask ==1){   // phys histo 
	      h_SumS_SIPM->Fill(evout->totSiPMSene);
	      h_SumC_SIPM->Fill(evout->totSiPMCene);

	   }// end phys case


	   //reset SiPM sum value 
	   evout->totSiPMCene = 0;
    	   evout->totSiPMSene = 0;

	} // end for on events

	} // end if on tow 0


	if(tow>0){

	   cout << " looking at PMT, tow " << tow << endl; 
           // index for ADC channels       
           c_idx= mych[tow-1];
           s_idx= mych[tow+7];
	   cout << c_idx << " " << ped_c.at(tow-1) << " " << eq_c.at(tow-1) << endl;
	   cout << s_idx << " " << ped_s.at(tow-1) << " " << eq_s.at(tow-1) << endl;
	
	for( unsigned int i=0; i<t->GetEntries(); i++){
	
           tSIPM->GetEntry(i);	
           t->GetEntry(i);

           if(TriggerMask ==1){   // phys histo 

		 //per tower histo, adc distributiom, ped substracted and equalised
                 h[tow-1]->Fill((ADC[c_idx]-ped_c.at(tow-1))/eq_c.at(tow-1));
                 h[tow+7]->Fill((ADC[s_idx]-ped_s.at(tow-1))/eq_s.at(tow-1));
                 h0[tow-1]->Fill((ADC[c_idx]-ped_c.at(tow-1)));
                 h0[tow+7]->Fill((ADC[s_idx]-ped_s.at(tow-1)));
	   
	   }// end phys case

	} // end for on events

	} // end else
} // end loop on tow

	TCanvas *c0_all = new TCanvas("c0_distrib", "c0_distrib", 1000, 700);
	c0_all->Divide(3,3); 
        c0_all->cd(1);
        gPad->SetLogy();
        h0[7]->Draw();
        c0_all->cd(2);
        gPad->SetLogy();
        h0[6]->Draw();
        c0_all->cd(3);
        gPad->SetLogy();
        h0[5]->Draw();
        c0_all->cd(4);
        gPad->SetLogy();
        h0[4]->Draw();
        c0_all->cd(5);
        gPad->SetLogy();
	h_SumC_SIPM->Draw(); 	
	c0_all->cd(6);
        gPad->SetLogy();
        h0[3]->Draw();
        c0_all->cd(7);
        gPad->SetLogy();
        h0[2]->Draw();
        c0_all->cd(8);
        gPad->SetLogy();
        h0[1]->Draw();
        c0_all->cd(9);
        gPad->SetLogy();
        h0[0]->Draw();

	TCanvas *s0_all = new TCanvas("s0_distrib", "s0_distrib", 1000, 700);
	s0_all->Divide(3,3); 
        s0_all->cd(1);
        gPad->SetLogy();
        h0[15]->Draw();
        s0_all->cd(2);
        gPad->SetLogy();
        h[14]->Draw();
        s0_all->cd(3);
        gPad->SetLogy();
        h[13]->Draw();
        s0_all->cd(4);
        gPad->SetLogy();
        h0[12]->Draw();
        s0_all->cd(5);
        gPad->SetLogy();
	if(myconf==0) h_SumS_SIPM->GetXaxis()->SetRangeUser(0, 1000);
        h_SumS_SIPM->Draw(); 	
        s0_all->cd(6);
        gPad->SetLogy();
        h0[11]->Draw();
        s0_all->cd(7);
        gPad->SetLogy();
        h0[10]->Draw();
        s0_all->cd(8);
        gPad->SetLogy();
        h0[9]->Draw();
        s0_all->cd(9);
        gPad->SetLogy();
        h0[8]->Draw();

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
	h_SumC_SIPM->Draw(); 	
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
	if(myconf==0) h_SumS_SIPM->GetXaxis()->SetRangeUser(0, 1000);
        h_SumS_SIPM->Draw(); 	
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


	return; 
 }





