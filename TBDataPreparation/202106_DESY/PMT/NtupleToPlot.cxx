//**********************************************************
//// \file NtupleToPlot.cxx 
//// \brief: Analysis of raw Ntuple txt and Histo Creation
//// \author: Gabriella Gaudio  @gaudio74
//// \start date: July 2021
////**********************************************************


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <TTree.h>
#include <TBranch.h>
#include <TFile.h>
#include <TH1.h>
#include <TMath.h>


#define DATADIR "/eos/user/i/ideadr/TB2021_Desy/rawNtuple/"
#define OUTDIR "/eos/user/i/ideadr/TB2021_Desy/recoNtuple/"
#define CALIBDIR "/eos/user/i/ideadr/TB2021_Desy/recoNtuple/CalibFiles/"


void NtupleToPlot(string run, int calib, int myt){

	//used for calibration only if calib==1
	if(calib==1) cout << "**** WE ARE CALIBRATING ***** " << endl;

	// to select the correct tower for calibration
	int t_c_index = myt-1; 
	int t_s_index = myt+7; 

	// graphical options
	gStyle->SetOptStat(111111);
	gStyle->SetPalette(1);
	gStyle->SetPaintTextFormat(".3f");
	gStyle->SetPadRightMargin(0.15);
	gStyle->SetNumberContours(100);
	gStyle->SetTextSize(0.05);


	// create the root file where histos are saved
        ofstream Data;
        ostringstream mydata;
	size_t pos = run.find(".root");
	string runno = run.substr(0,pos); 
        mydata << OUTDIR << runno << ".txt" ;
        Data.open(mydata.str().c_str(), ofstream::out | ofstream::app);
        cout << "Text output file: " << endl << mydata.str() << endl;

	
        ofstream Edump;
        ostringstream myEd;
        myEd << OUTDIR << runno << "_Edump.txt" ;
        Edump.open(myEd.str().c_str(), ofstream::out | ofstream::app);
        cout << "Text output file: " << endl << myEd.str() << endl;
	

        ofstream FitData;
        ostringstream myfit;
	// Open fit data file only if tower in less then 8 (doing a scan).
	if(calib==1){
        	myfit << CALIBDIR <<  "PMT_calib.txt" ;
        	FitData.open(myfit.str().c_str(), ofstream::out | ofstream::app);
        	cout << "Text output file: " << endl << myfit.str() << endl;
	}


	ostringstream myfile;
        myfile << DATADIR  << run;
        cout << "opening ntuple input file ... " <<  myfile.str() <<endl;
        TFile *f=new TFile(myfile.str().c_str());
        TTree *t = (TTree*) f->Get("DREAM");

        unsigned int Nevtda = 0 ;
        unsigned int TowerCts[3]={0};
        unsigned int TowerMask =0;

        UInt_t CHADCN0[32];
        UInt_t CHARGEADCN0[32] ;
	
	Int_t count; 


        t->SetBranchAddress("Nevtda",&Nevtda);
        t->SetBranchAddress("TowerCts",&TowerCts);
        t->SetBranchAddress("TowerMask",&TowerMask);

        t->SetBranchAddress("CHADCN0",CHADCN0);
        t->SetBranchAddress("CHARGEADCN0",CHARGEADCN0);




   	TH1F *h[16];
   	TH1F *hnorm[16];
   	TH1F *hped[16];
   	int nbin =4096;
   	int xlow = 0;
   	int xhigh =4096;
   	//int nbin =2048;
   	//int xlow = 0;
   	//int xhigh =2048;
   	int nbin_c =2048;
   	int xlow_c = 0;
   	int xhigh_c =2048;

   	int npbin =270;
   	int xplow = 0;
   	//int xplow = 80;
   	int xphigh =350;

        int nEbin = 5000;
        int nEbin_c = 100;
        int xElow = 0;
        int xEhigh = 5;
        int xEhigh_c = 12;


	// phys histo	
   	for(int n=0;n<8; n++){
        	stringstream hname;
        	stringstream htitle;
        	hname<<"h_C_"<<n+1;
        	htitle << "Cher_"<<n+1;
        	h[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin, xlow,xhigh);
        	//h[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin_c, xlow_c,xhigh_c);
        	h[n]->GetXaxis()->SetTitle("ADC counts");

        	stringstream hnameN;
        	stringstream htitleN;
        	hnameN<<"h_C_norm"<<n+1;
        	htitleN << "Cher_norm"<<n+1;
        	if(calib!=0) {
			hnorm[n]=new TH1F(hnameN.str().c_str(), htitleN.str().c_str(), nbin_c, xlow_c,xhigh_c);
        		hnorm[n]->GetXaxis()->SetTitle("ADC counts");
		}
		else{
			hnorm[n]=new TH1F(hnameN.str().c_str(), htitleN.str().c_str(), nEbin, xElow, xEhigh);
        		hnorm[n]->GetXaxis()->SetTitle("GeV");
		}

   	}
   	for(int n=8;n<16; n++){
        	stringstream hname;
        	stringstream htitle;
        	hname<<"h_S_"<<n-7;
        	htitle << "Scint_"<<n-7;
        	h[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), nbin, xlow, xhigh);
        	h[n]->GetXaxis()->SetTitle("ADC counts");

        	stringstream hnameN;
        	stringstream htitleN;
        	hnameN<<"h_S_norm"<<n-7;
        	htitleN << "Scint_norm"<<n-7;
        	if(calib!=0) {
			hnorm[n]=new TH1F(hnameN.str().c_str(), htitleN.str().c_str(), nbin, xlow,xhigh);
        		hnorm[n]->GetXaxis()->SetTitle("ADC counts");
		}
		else{
			hnorm[n]=new TH1F(hnameN.str().c_str(), htitleN.str().c_str(), nEbin, xElow, xEhigh);
        		hnorm[n]->GetXaxis()->SetTitle("GeV");
		}

	}


	// pedestal histo
        for(int n=0;n<8; n++){
                stringstream hname;
                stringstream htitle;
                hname<<"h_ped_C_"<<n+1;
                htitle << "PedCher_"<<n+1;
                hped[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), npbin, xplow,xphigh);
                hped[n]->GetXaxis()->SetTitle("ADC counts");
        }
        for(int n=8;n<16; n++){
                stringstream hname;
                stringstream htitle;
                hname<<"h_ped_S_"<<n-7;
                htitle << "PedScint_"<<n-7;
                hped[n]=new TH1F(hname.str().c_str(), htitle.str().c_str(), npbin, xplow, xphigh);
                hped[n]->GetXaxis()->SetTitle("ADC counts");
        }


	TH1F *h_SumC = new TH1F("SumC", "SumC", 4096, 0,4096);
	h_SumC->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_SumS = new TH1F("SumS", "SumS", 4096, 0,4096);
	h_SumS->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_SumC_E = new TH1F("SumC_E", "SumC_E", nEbin, xElow, xEhigh);
	h_SumC_E->GetXaxis()->SetTitle("E (GeV)");

	TH1F *h_SumS_E = new TH1F("SumS_E", "SumS_E", nEbin, xElow, xEhigh);
	h_SumS_E->GetXaxis()->SetTitle("E (GeV)");


	TH2F *h_bar_S = new TH2F("Barycentre S", "Barycentre S", 60, 0.5, 3.5, 60, 0.5, 3.5);
	h_bar_S->GetXaxis()->SetTitle("x_S ");
	h_bar_S->GetYaxis()->SetTitle("y_S ");

	TH2F *h_bar_C = new TH2F("Barycentre C", "Barycentre C", 60, 0.5, 3.5, 60, 0.5, 3.5);
	h_bar_C->GetXaxis()->SetTitle("x_C ");
	h_bar_C->GetYaxis()->SetTitle("y_C ");

	TH2F *h_bar_corr_x = new TH2F("S_vs_C x barycentre", "S_vs_C x barycentre", 60, 0.5, 3.5, 60, 0.5, 3.5);
	h_bar_corr_x->GetXaxis()->SetTitle("x_S ");
	h_bar_corr_x->GetYaxis()->SetTitle("x_C ");

	TH2F *h_bar_corr_y = new TH2F("S_vs_C y barycentre", "S_vs_C y barycentre", 60, 0.5, 3.5, 60, 0.5, 3.5);
	h_bar_corr_y->GetXaxis()->SetTitle("y_S ");
	h_bar_corr_y->GetYaxis()->SetTitle("y_C ");

	
	TH2F *h_corr = new TH2F("h_S_vs_C", "h_S_vs_C",  nEbin, xElow, xEhigh,  nEbin_c, xElow, xEhigh_c); 
	h_corr->GetXaxis()->SetTitle("E_S (GeV)");
	h_corr->GetYaxis()->SetTitle("E_C (GeV)");


	TH2F *h_corr_tow[8]; 
	TH2F *h_corr_tow_E[8]; 
	TH2F *h_corr_tow_S[8]; 
	TH2F *h_corr_tow_C[8]; 


	for(int n=0;n<8; n++){
                stringstream hname;
                stringstream htitle;
                hname<<"h_corr_tow_"<<n+1;
                htitle << "corr_tow_"<<n+1;
                h_corr_tow[n]=new TH2F(hname.str().c_str(), htitle.str().c_str(), nbin, xlow, xhigh, 500, xlow_c,500.);
                h_corr_tow[n]->GetXaxis()->SetTitle("Scintillation");
                h_corr_tow[n]->GetYaxis()->SetTitle("Cherenkov");
              
	      	stringstream hnameE;
                stringstream htitleE;
                hnameE<<"h_corr_tow_E_"<<n+1;
                htitleE << "corr_tow_E_"<<n+1;
                h_corr_tow_E[n]=new TH2F(hnameE.str().c_str(), htitleE.str().c_str(), nEbin, xElow, xEhigh, nEbin_c, xElow, xEhigh_c);
                h_corr_tow_E[n]->GetXaxis()->SetTitle("Scintillation (GeV)");
                h_corr_tow_E[n]->GetYaxis()->SetTitle("Cherenkov (GeV)");
                
		stringstream hnameS;
                stringstream htitleS;
                hnameS<<"h_corr_tow_S_"<<n+1;
                htitleS << "corr_tow_S_"<<n+1;
                h_corr_tow_S[n]=new TH2F(hnameS.str().c_str(), htitleS.str().c_str(), nbin, xlow, xhigh, nEbin, xElow, xEhigh );
                h_corr_tow_S[n]->GetXaxis()->SetTitle("Scintillation");
                h_corr_tow_S[n]->GetYaxis()->SetTitle("Scintillation (GeV)");
        
		stringstream hnameC;
                stringstream htitleC;
                hnameC<<"h_corr_tow_C_"<<n+1;
                htitleC << "corr_tow_C_"<<n+1;
                h_corr_tow_C[n]=new TH2F(hnameC.str().c_str(), htitleC.str().c_str(), nbin_c, xlow_c, xhigh_c, nEbin_c, xElow, xEhigh_c );
                h_corr_tow_C[n]->GetXaxis()->SetTitle("Cherenkov");
                h_corr_tow_C[n]->GetYaxis()->SetTitle("Cherenkov (GeV)");
        
	}



	TH2F *C_map = new TH2F("C_map_cumulative", "C_map_cumulative", 3, 0.5, 3.5,  3, 0.5, 3.5); 
	TH2F *S_map = new TH2F("S_map_cumulative", "S_map_cumulative", 3, 0.5, 3.5,  3, 0.5, 3.5); 


	TH1F *h_SumPedC = new TH1F("SumPEDC", "SumPEDC", npbin*8, xplow*8, xphigh*8);
	h_SumPedC->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_SumPedS = new TH1F("SumPEDS", "SumPEDS", npbin*8, xplow*8, xphigh*8);
	h_SumPedS->GetXaxis()->SetTitle("ADC Counts");


	TH1F *h_mask = new TH1F("trigger masks","trigger masks", 3, 0.5, 3.5); 


	int mych[16]={0,2,4,6,8,10,12,14,16,18,21,22,24,27,28,31};
	int myxbin[8]={3,2,1,3,1,3,2,1};
	int myybin[8]={1,1,1,2,2,3,3,3};
	
	count=(Int_t) t->GetEntries();

	cout<< "Numb evt " << count<<"\n";

	// Get Pedestal Info

       for(int i=0;i<count;i++){

                if(i%10000==0) cout << " Searching ped in evt " << i << endl;
                t->GetEntry(i);
		Int_t SumPedS=0; 
		Int_t SumPedC=0; 
		
		if(TowerMask ==2){   // ped histo
		   //Ped << Nevtda <<  endl;
		   for(int n=0; n<16; n++){
			for(int k=0; k<32; k++){
				if(CHADCN0[k]==mych[n]) {
					hped[n]->Fill(CHARGEADCN0[k]);
					if(n<8){
					     SumPedC+=CHARGEADCN0[k];
					}	
					else {
					     SumPedS+=CHARGEADCN0[k];
					}
				} // if channel selection
			} // fon on adcch info
			//cout << SumC << " " << SumS << endl; 
		   }//for on channels
		h_SumPedC->Fill(SumPedC); 
		h_SumPedS->Fill(SumPedS); 
		} // PED histo
		if(TowerMask !=2 && TowerMask !=1){
			cout << " TowerMask " << TowerMask << " in evt " << i << endl;  
		}	
       }// end loop on evet to extract pedestal info

	Float_t myped[16]={0};


        for(int n=0;n<16; n++){
		myped[n] = hped[n]->GetMean(); 
		cout << hped[n]->GetName() << " mean " << hped[n]->GetMean() << " rms " << hped[n]->GetRMS() << endl; 
		Data << hped[n]->GetName() << " mean " << hped[n]->GetMean() << " rms " << hped[n]->GetRMS() << endl; 
	}
	

	// Get the calibration constants from file

        ostringstream mean;

	// this is before step 2
	if(calib==2) mean << CALIBDIR <<  "equalization.txt";
	// this is for final calib
	if(calib==0) mean << CALIBDIR <<  "calib_constant_filter.txt";
        cout << mean.str().c_str() << std::endl;

        double c,s;
        vector<double> c_scale;
        vector<double> s_scale;
        double c0, s0;


	if(calib!=1){
        	std::ifstream in0(mean.str());
        	if (!in0 || in0.bad()) return 0; // sanity check

        	while (1) {
                	in0 >> s >> c;
                	if (!in0.good()) break;
			if(calib==2){
                		c_scale.push_back(c);
                		s_scale.push_back(s);
        		}
			if(calib==0){
                		c_scale.push_back(1/c);
                		s_scale.push_back(1/s);
        		}


		}
	}
	if(calib==1){
        	for(int k=0; k<8; k++){
                	c_scale.push_back(1);
                	s_scale.push_back(1);
		}
	}

	for(int k=0; k<c_scale.size(); k++){
                cout << s_scale.at(k) << " " << c_scale.at(k) << endl;
        }



	// filling histo Ped Subtracted

	for(int i=0;i<count;i++){
	
		if(i%10000==0) cout << " processing evt " << i << endl; 
        	t->GetEntry(i);
		Float_t SumS=0; 
		Float_t SumC=0; 
		Float_t SumS_E=0; 
		Float_t SumC_E=0; 
		//Float_t s_4=0; 
		//Float_t c_4=0;
		Float_t s[8]={0};
		Float_t c[8]={0};
		Float_t Es[8]={0};
		Float_t Ec[8]={0};
		


		//cout << i << " " << Nevtda << " "  << TowerCts[0] << " " << TowerCts[1] << " " << TowerCts[2] 
		     //<< " " << TowerMask << endl;
		h_mask->Fill(TowerMask); 

		if(TowerMask ==1){
		   for(int n=0; n<16; n++){
			for(int k=0; k<32; k++){
				if(CHADCN0[k]==mych[n]) {
					//cout << n << " mych[n] " << mych[n] << " CHADCN0[k] " << CHADCN0[k] 
					//<< " CHARGEADCN0[k] " << CHARGEADCN0[k] << " myped[n] " << myped[n]<<  endl;
					h[n]->Fill(CHARGEADCN0[k] - myped[n]);
					if(n<8){
					     hnorm[n]->Fill((CHARGEADCN0[k] - myped[n])*c_scale.at(n));	
					     SumC_E+=(CHARGEADCN0[k] - myped[n])*c_scale.at(n);
					     SumC+=CHARGEADCN0[k] - myped[n];
					     C_map->Fill(myxbin[n], myybin[n], (CHARGEADCN0[k] - myped[n])*c_scale.at(n));
					     c[n]=(CHARGEADCN0[k] - myped[n]); 
					     Ec[n]=(CHARGEADCN0[k] - myped[n])*c_scale.at(n); 
					     //if(n==3) c_4 = (CHARGEADCN0[k] - myped[n])*c_scale.at(n);
					     //cout << "C: x,y,ADC: " << myxbin[n] << " " << myybin[n] << " " 
					     //		  <<  CHARGEADCN0[k] << endl; 
					}	
					else {
					     hnorm[n]->Fill((CHARGEADCN0[k] - myped[n])*s_scale.at(n-8));
					     SumS_E+=(CHARGEADCN0[k] - myped[n])*s_scale.at(n-8);
					     SumS+=CHARGEADCN0[k] - myped[n];
					     S_map->Fill(myxbin[n-8], myybin[n-8], (CHARGEADCN0[k] - myped[n])*s_scale.at(n-8));
					     s[n-8]=(CHARGEADCN0[k] - myped[n]);   
					     Es[n-8]=(CHARGEADCN0[k] - myped[n])*s_scale.at(n-8);   
					     //if(n==11) s_4 = (CHARGEADCN0[k] - myped[n])*s_scale.at(n-8);
					     //cout << "S: x,y,ADC: " << myxbin[n-8] << " " << myybin[n-8] << " " 
					     //	  <<  CHARGEADCN0[k] << endl; 
					}
				} // if channel selection
			} // fon on adcch info
			//cout << SumC << " " << SumS << endl; 
		   }//for on channels
		h_SumC->Fill(SumC); 
		h_SumS->Fill(SumS); 
		h_SumC_E->Fill(SumC_E); 
		h_SumS_E->Fill(SumS_E);
	
		float bar_xS =0; 
		float bar_yS =0;
		float bar_xC =0; 
		float bar_yC =0;


		for(int n=0; n<8; n++){
			h_corr_tow[n]->Fill(s[n],c[n]);
			h_corr_tow_E[n]->Fill(Es[n],Ec[n]);
			//h_corr_tow_S[n]->Fill(s[n],Es[n]);
			//h_corr_tow_C[n]->Fill(c[n],Ec[n]);
			bar_xS +=Es[n]*myxbin[n];
			bar_yS +=Es[n]*myybin[n];
			bar_xC +=Ec[n]*myxbin[n];
			bar_yC +=Ec[n]*myybin[n];

			if(i%100000==0) {
				//cout << n << " " << s[n] << " " << Es[n] << " " << s[n]/Es[n] << endl; 
				//cout << n << " " << c[n] << " " << Ec[n] << " " << c[n]/Ec[n] << endl; 
				//cout << bar_xS << " " << bar_xC << " " << endl;
			}		
		}
		
		h_bar_S->Fill(bar_xS/SumS_E, bar_yS/SumS_E); 
		h_bar_C->Fill(bar_xC/SumC_E, bar_yC/SumC_E); 
		h_bar_corr_x->Fill(bar_xS/SumS_E, bar_xC/SumC_E); 
		h_bar_corr_y->Fill(bar_yS/SumS_E, bar_yC/SumC_E); 
		
		
		h_corr->Fill(SumS_E,SumC_E);
	        if(calib==0) Edump << Nevtda << " " << bar_xS/SumS_E  << " " <<  bar_yS/SumS_E  << endl; 	
		} // Phys histo


		

	} // end loop on events



	ostringstream rootfile;
   	rootfile  << OUTDIR << run ;
   	TFile myroot(rootfile.str().c_str(), "RECREATE");
   	cout << "Root output file: " << endl << rootfile.str() << endl;
	
	TCanvas *c9=new TCanvas("Trigger Masks","Trigger Masks", 700, 700);
	gPad->SetLogy();
	h_mask->Draw(); 
	h_mask->Write(); 
        c9->Update(); 
        c9->Draw();


	TCanvas *s1=new TCanvas("Scintillation","Scintillation",1000, 1000);
   	s1->Divide(3,3);
   	s1->cd(1);
   	gPad->SetLogy();
   	h[15]->Draw();
   	h[15]->Write();
   	s1->cd(2);
   	gPad->SetLogy();
   	h[14]->Draw();
   	h[14]->Write();
   	s1->cd(3);
   	gPad->SetLogy();
  	h[13]->Draw();
   	h[13]->Write();
   	s1->cd(4);
   	gPad->SetLogy();
   	h[12]->Draw();
   	h[12]->Write();
   	s1->cd(6);
   	gPad->SetLogy();
   	h[11]->Draw();
   	h[11]->Write();
   	s1->cd(7);
   	gPad->SetLogy();
   	h[10]->Draw();
   	h[10]->Write();
   	s1->cd(8);
   	gPad->SetLogy();
   	h[9]->Draw();
   	h[9]->Write();
   	s1->cd(9);
   	gPad->SetLogy();
   	h[8]->Draw();
   	h[8]->Write();
   	s1->Write();
	s1->Update(); 
	s1->Draw();

	TCanvas *c1=new TCanvas("Cherenkov","Cherenkov",1000, 1000);
   	c1->Divide(3,3);
   	c1->cd(1);
   	gPad->SetLogy();
   	h[7]->Draw();
   	h[7]->Write();
   	c1->cd(2);
   	gPad->SetLogy();
   	h[6]->Draw();
   	h[6]->Write();
   	c1->cd(3);
   	gPad->SetLogy();
   	h[5]->Draw();
   	h[5]->Write();
   	c1->cd(4);
   	gPad->SetLogy();
   	h[4]->Draw();
   	h[4]->Write();
   	c1->cd(6);
   	gPad->SetLogy();
   	h[3]->Draw();
   	h[3]->Write();
   	c1->cd(7);
   	gPad->SetLogy();
   	h[2]->Draw();
   	h[2]->Write();
   	c1->cd(8);
   	gPad->SetLogy();
   	h[1]->Draw();
   	h[1]->Write();
   	c1->cd(9);
   	gPad->SetLogy();
   	h[0]->Draw();
   	h[0]->Write();
   	c1->Write();
	c1->Update(); 
	c1->Draw();


	TCanvas *s11=new TCanvas("Scintillation_norm","Scintillation_norm",1000, 1000);
   	s11->Divide(3,3);
   	s11->cd(1);
   	gPad->SetLogy();
   	hnorm[15]->Draw();
   	hnorm[15]->Write();
   	s11->cd(2);
   	gPad->SetLogy();
   	hnorm[14]->Draw();
   	hnorm[14]->Write();
   	s11->cd(3);
   	gPad->SetLogy();
  	hnorm[13]->Draw();
   	hnorm[13]->Write();
   	s11->cd(4);
   	gPad->SetLogy();
   	hnorm[12]->Draw();
   	hnorm[12]->Write();
   	s11->cd(6);
   	gPad->SetLogy();
   	hnorm[11]->Draw();
   	hnorm[11]->Write();
   	s11->cd(7);
   	gPad->SetLogy();
   	hnorm[10]->Draw();
   	hnorm[10]->Write();
   	s11->cd(8);
   	gPad->SetLogy();
   	hnorm[9]->Draw();
   	hnorm[9]->Write();
   	s11->cd(9);
   	gPad->SetLogy();
   	hnorm[8]->Draw();
   	hnorm[8]->Write();
   	s11->Write();
	s11->Update(); 
	s11->Draw();

	TCanvas *c11=new TCanvas("Cherenkov_norm","Cherenkov_norm",1000, 1000);
   	c11->Divide(3,3);
   	c11->cd(1);
   	gPad->SetLogy();
   	hnorm[7]->Draw();
   	hnorm[7]->Write();
   	c11->cd(2);
   	gPad->SetLogy();
   	hnorm[6]->Draw();
   	hnorm[6]->Write();
   	c11->cd(3);
   	gPad->SetLogy();
   	hnorm[5]->Draw();
   	hnorm[5]->Write();
   	c11->cd(4);
   	gPad->SetLogy();
   	hnorm[4]->Draw();
   	hnorm[4]->Write();
   	c11->cd(6);
   	gPad->SetLogy();
   	hnorm[3]->Draw();
   	hnorm[3]->Write();
   	c11->cd(7);
   	gPad->SetLogy();
   	hnorm[2]->Draw();
   	hnorm[2]->Write();
   	c11->cd(8);
   	gPad->SetLogy();
   	hnorm[1]->Draw();
   	hnorm[1]->Write();
   	c11->cd(9);
   	gPad->SetLogy();
   	hnorm[0]->Draw();
   	hnorm[0]->Write();
   	c11->Write();
	c11->Update(); 
	c11->Draw();


	TCanvas *c3=new TCanvas("Sum_ADC","Sum_ADC", 1000, 700);
	c3->Divide(1,2);
   	c3->cd(1);
   	gPad->SetLogy();
	h_SumC->Draw();
	h_SumC->Write();
   	c3->cd(2);
   	gPad->SetLogy();
	h_SumS->Draw();
	h_SumS->Write();
	c3->Write();
	c3->Update(); 
	c3->Draw();

	TCanvas *c33=new TCanvas("Sum_E","Sum_E", 1000, 700);
	c33->Divide(1,2);
   	c33->cd(1);
   	gPad->SetLogy();
	h_SumC_E->Draw();
	h_SumC_E->Write();
   	c33->cd(2);
   	gPad->SetLogy();
	h_SumS_E->Draw();
	h_SumS_E->Write();
	c33->Write();
	c33->Update(); 
	c33->Draw();

        TCanvas *c34=new TCanvas("SumES_vs_SumEC","SumES_vs_SumEC", 1000, 1000);
        h_corr->Draw("colz");
        h_corr->SetStats(kFALSE);
        h_corr->Write();
	c34->Write();
        c34->Update();
        c34->Draw();



	TCanvas *c44=new TCanvas("S_vs_C","s_vs_C", 1000, 1000);
	c44->Divide(2,1);
	c44->cd(1);
	h_corr_tow[myt]->Draw("colz"); 
	h_corr_tow[myt]->SetStats(kFALSE);
	h_corr_tow[myt]->Write();
	c44->cd(2);
	h_corr_tow_E[myt]->Draw("colz");
	h_corr_tow_E[myt]->SetStats(kFALSE);
        h_corr_tow_E[myt]->Write();
	//c44->cd(3);
	//h_corr_tow_S[myt]->Draw("colz"); 
	//h_corr_tow_S[myt]->SetStats(kFALSE);
	//h_corr_tow_S[myt]->Write();
	//c44->cd(4);
	//h_corr_tow_C[myt]->Draw("colz");
	//h_corr_tow_C[myt]->SetStats(kFALSE);
        //h_corr_tow_C[myt]->Write();
	c44->Write();
        c44->Update();
        c44->Draw();



	TCanvas *c46=new TCanvas("barycentre","barycentre", 1000, 1000);
	c46->Divide(2,2);
	c46->cd(1);
	h_bar_S->Draw("colz"); 
	h_bar_S->SetStats(kFALSE);
	h_bar_S->Write();
	c46->cd(2);
	h_bar_C->Draw("colz");
	h_bar_C->SetStats(kFALSE);
        h_bar_C->Write();
	c46->cd(3);
	h_bar_corr_x->Draw("colz"); 
	h_bar_corr_x->SetStats(kFALSE);
	h_bar_corr_x->Write();
	c46->cd(4);
	h_bar_corr_y->Draw("colz");
	h_bar_corr_y->SetStats(kFALSE);
        h_bar_corr_y->Write();
	c46->Write();
        c46->Update();
        c46->Draw();




	TCanvas *c4=new TCanvas("C(left)_S(right)_Display","C(left)_S(right)_Display",1000,500);
	c4->Divide(2,1); 
	c4->cd(1);
	C_map->Draw("colz1");
	C_map->SetStats(kFALSE);
	C_map->Write();
	c4->cd(2); 
	S_map->Draw("colz1");
	S_map->SetStats(kFALSE);
	S_map->Write();
	c4->Update(); 
	c4->Draw();


	// Pedestal plots

	TCanvas *s2=new TCanvas("Pedestal Scintillation","Pedestal Scintillation",1000, 1000);
   	s2->Divide(3,3);
   	s2->cd(1);
   	gPad->SetLogy();
   	hped[15]->Draw();
   	hped[15]->Write();
   	s2->cd(2);
   	gPad->SetLogy();
   	hped[14]->Draw();
   	hped[14]->Write();
   	s2->cd(3);
   	gPad->SetLogy();
  	hped[13]->Draw();
   	hped[13]->Write();
   	s2->cd(4);
   	gPad->SetLogy();
   	hped[12]->Draw();
   	hped[12]->Write();
   	s2->cd(6);
   	gPad->SetLogy();
   	hped[11]->Draw();
   	hped[11]->Write();
   	s2->cd(7);
   	gPad->SetLogy();
   	hped[10]->Draw();
   	hped[10]->Write();
   	s2->cd(8);
   	gPad->SetLogy();
   	hped[9]->Draw();
   	hped[9]->Write();
   	s2->cd(9);
   	gPad->SetLogy();
   	hped[8]->Draw();
   	hped[8]->Write();
   	s2->Write();
	s2->Update(); 
	s2->Draw();

	TCanvas *c2=new TCanvas("Pedestal Cherenkov","Pedestal Cherenkov",1000, 1000);
   	c2->Divide(3,3);
   	c2->cd(1);
   	gPad->SetLogy();
   	hped[7]->Draw();
   	hped[7]->Write();
   	c2->cd(2);
   	gPad->SetLogy();
   	hped[6]->Draw();
   	hped[6]->Write();
   	c2->cd(3);
   	gPad->SetLogy();
   	hped[5]->Draw();
   	hped[5]->Write();
   	c2->cd(4);
   	gPad->SetLogy();
   	hped[4]->Draw();
   	hped[4]->Write();
   	c2->cd(6);
   	gPad->SetLogy();
   	hped[3]->Draw();
   	hped[3]->Write();
   	c2->cd(7);
   	gPad->SetLogy();
   	hped[2]->Draw();
   	hped[2]->Write();
   	c2->cd(8);
   	gPad->SetLogy();
   	hped[1]->Draw();
   	hped[1]->Write();
   	c2->cd(9);
   	gPad->SetLogy();
   	hped[0]->Draw();
   	hped[0]->Write();
   	c2->Write();
	c2->Update(); 
	c2->Draw();


	TCanvas *c6=new TCanvas("Sum_Pedestal","Sum_Pedestal", 1000, 700);
	c6->Divide(1,2);
   	c6->cd(1);
   	gPad->SetLogy();
	h_SumPedC->Draw();
	h_SumPedC->Write();
   	c6->cd(2);
   	gPad->SetLogy();
	h_SumPedS->Draw();
	h_SumPedS->Write();
	c6->Write();
	c6->Update(); 
	c6->Draw();


	// doing this part only for calibration
	if(calib==1){
		gStyle->SetOptFit(1);
		TCanvas *c10 = new TCanvas("fit pan", "fit pan", 1000, 700);
 		c10->Divide(1,2); 	
		c10->cd(1); 
		gPad->SetLogy();
		h[t_s_index]->Draw(); 
		//TF1 *g1 = new TF1("g1","gaus", 200, 600);
		//g1->SetLineColor(kRed); 
		//h[t_s_index]->Fit(g1, "R+", "SAME");
		h[t_s_index]->GetXaxis()->SetRangeUser(150, 3000);
		int binmax1 = h[t_s_index]->GetMaximumBin();
		double x_S = h[t_s_index]->GetXaxis()->GetBinCenter(binmax1);
		c10->cd(2); 
		gPad->SetLogy();
		h[t_c_index]->Draw(); 
		//TF1 *g2 = new TF1("g2","gaus", 70, 170);
		//g2->SetLineColor(kRed); 
		//h[t_c_index]->Fit(g2, "R+", "SAME");
		h[t_c_index]->GetXaxis()->SetRangeUser(40, 1000);
		int binmax2 = h[t_c_index]->GetMaximumBin();
		double x_C = h[t_c_index]->GetXaxis()->GetBinCenter(binmax2);
		c10->Update(); 
		cout << x_S << " " << x_C << endl; 	
		FitData << myt << " " << x_S << " " << x_C << endl; 

	}




        for(int n=0;n<16; n++){
		//cout << h[n]->GetName() << " mean " << h[n]->GetMean() << " rms " << h[n]->GetRMS() << endl; 
		//cout << hnorm[n]->GetName() << " mean " << hnorm[n]->GetMean() << " rms " << hnorm[n]->GetRMS() << endl; 
		Data << h[n]->GetName() << " mean " << h[n]->GetMean() << " rms " << h[n]->GetRMS() << endl; 
	}

	
	return;
}

