#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <TTree.h>
#include <TBranch.h>
#include <TFile.h>
#include <TH1.h>
#include <TMath.h>
#include "../../TBDataPreparation/202106_DESY/scripts/PhysicsEvent.h"

ClassImp(EventOut)


#define DATADIR  "/eos/user/i/ideadr/TB2021_Desy/recoNtuple/"
#define OUTDIR  "/eos/user/i/ideadr/TB2021_Desy/PMT_calibration/"

void CheckCalib(int myconf){
	// myconf flag allows to switch among the three configuration with which DESY data has been taken


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

        TH1F *h[16];
        int nbin =150;
        int xlow = 0;
        int xhigh =15;
	
        // Histograms per Tower	
        for(int n=0;n<8; n++){
                stringstream hname;
                stringstream htitle;
                hname<<"h_C_"<<n+1;
                htitle << "Cher_"<<n+1;
                h[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin, xlow,xhigh);
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

	TH1F *h_SumC_SIPM = new TH1F("SumC_SIPM", "SumC_SIPM", nbin,xlow,xhigh);
        h_SumC_SIPM->GetXaxis()->SetTitle("Energy (GeV)");

        TH1F *h_SumS_SIPM = new TH1F("SumS_SIPM", "SumS_SIPM", nbin, xlow, xhigh);
        h_SumS_SIPM->GetXaxis()->SetTitle("Energy (GeV)");




	// using data file with beam steered in the center
	TFile *f;
	ostringstream infile;

	for(int tow=0; tow<9; tow++ ) {
	cout << "tow " << tow << endl; 
        
        infile.str(""); 
        infile <<  DATADIR <<  "physics_desy2021_run" << std::to_string(runs[tow]) << ".root";
        std::cout<<"Using file: "<<infile.str()<<std::endl;
        f=new TFile(infile.str().c_str());
        TTree *t = (TTree*) f->Get("Ftree");
        cout << t->GetEntries()<<  endl;

	auto evt = new EventOut();
        t->SetBranchAddress("Events",&evt);


        cout << " looking at tow " << tow << endl; 
        if(tow==0){
	          
	   for( unsigned int i=0; i<t->GetEntries(); i++){
              t->GetEntry(i);
	      h_SumS_SIPM->Fill(evt->totSiPMSene);
	      h_SumC_SIPM->Fill(evt->totSiPMCene);
	   }
        }
	else{
	
	   for( unsigned int i=0; i<t->GetEntries(); i++){
              t->GetEntry(i);
	      switch (tow){
		case 1 :
                 h[tow-1]->Fill(evt->CPMT1);
                 h[tow+7]->Fill(evt->SPMT1);
		 break; 

		case 2 :
                 h[tow-1]->Fill(evt->CPMT2);
                 h[tow+7]->Fill(evt->SPMT2);
		 break; 
	  
		case 3 :
                 h[tow-1]->Fill(evt->CPMT3);
                 h[tow+7]->Fill(evt->SPMT3);
		 break; 
	  
		case 4 :
                 h[tow-1]->Fill(evt->CPMT4);
                 h[tow+7]->Fill(evt->SPMT4);
		 break; 
	  
		case 5 :
                 h[tow-1]->Fill(evt->CPMT5);
                 h[tow+7]->Fill(evt->SPMT5);
		 break; 
	  
		case 6 :
                 h[tow-1]->Fill(evt->CPMT6);
                 h[tow+7]->Fill(evt->SPMT6);
		 break; 
		
		case 7 :
                 h[tow-1]->Fill(evt->CPMT7);
                 h[tow+7]->Fill(evt->SPMT7);
		 break; 
	  
		case 8 :
                 h[tow-1]->Fill(evt->CPMT8);
                 h[tow+7]->Fill(evt->SPMT8);
		 break; 
	  
	  
		default:
		 cout<< "error"<< endl; 
	     } // end switch	 

	} // end for on events

	} // end else
} // end loop on tow

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





