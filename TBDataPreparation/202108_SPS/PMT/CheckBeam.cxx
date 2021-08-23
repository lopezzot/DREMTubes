//**********************************************************
// \file CheckBeam.cxx 
// \brief: Quick analysis on raw Ntuple to check beam quality
// \author: Gabriella Gaudio  @gaudio74
// \start date: August 2021
//**********************************************************


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

#define DATADIR "/eos/user/i/ideadr/TB2021_H8/rawNtuple/"
//#define DATADIR "/afs/cern.ch/user/g/gaudio/data/"
// change this output local dir to your
#define OUTDIR "/afs/cern.ch/user/g/gaudio/public/Dream/Prototype2020/DataAnalysis/202108_SPS/"
//#define OUTDIR "/eos/user/i/ideadr/TB2021_Desy/recoNtuple/"
//#define CALIBDIR "/eos/user/i/ideadr/TB2021_Desy/recoNtuple/CalibFiles/"

// data file name looks like sps2021data.run627.root


void CheckBeam(string run, int E, int tower){

        gStyle->SetOptStat(111111);
        gStyle->SetPalette(1);
        gStyle->SetPaintTextFormat(".3f");
        gStyle->SetPadRightMargin(0.15);
        gStyle->SetNumberContours(100);
        gStyle->SetTextSize(0.05);

	Int_t ch= tower-1; 

	// opening data file
	stringstream myfile;
        myfile << DATADIR  << run;
        cout << "opening ntuple input file ... " <<  myfile.str() << " Energy " << E << " steering in Tower " << tower <<endl;
        TFile *f=new TFile(myfile.str().c_str());
        TTree *t = (TTree*) f->Get("CERNSPS2021");

	// Variable for data
        Int_t EventNumber = 0 ;
        Long64_t TriggerMask =0;
        Int_t ADCs[96];
	
	// accessing branches
        t->SetBranchAddress("EventNumber",&EventNumber);
        t->SetBranchAddress("TriggerMask",&TriggerMask);
        t->SetBranchAddress("ADCs",ADCs);

	Int_t count;

	// ancillary detectors adc channels
	Int_t i_ps=16;
	Int_t i_mu=32;
	Int_t i_c1=64;
	Int_t i_c2=65;

	
	TH1F *h_mask = new TH1F("trigger masks","trigger masks", 8, -0.5, 7.5);
	
	TH1F *h_mu = new TH1F("MuonCounter", "MuonCounter", 4096, 0,4096);
        h_mu->GetXaxis()->SetTitle("ADC Counts");

	//Cherenkov related Histo 
	TH1F *h_C1 = new TH1F("Cherenkov1","Cherenkov1", 512, 0,4096);
	h_C1->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_C1_ped = new TH1F("Cherenkov1_ped","Cherenkov1_ped", 512, 0, 512);
	h_C1_ped->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_C2 = new TH1F("Cherenkov2","Cherenkov2", 512, 0, 4096);
	h_C2->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_C2_ped = new TH1F("Cherenkov2_ped","Cherenkov2_ped", 512, 0, 512);
	h_C2_ped->GetXaxis()->SetTitle("ADC Counts");

	TH2F *h_C1C2 = new TH2F("c1_vs_c2","c1_vs_c2",	512, 0, 4096, 512, 0, 4096);
	h_C1C2->GetXaxis()->SetTitle("C1 (ADC Counts)");
	h_C1C2->GetYaxis()->SetTitle("C2 (ADC Counts)");

	TH2F *h_C1_vs_PS = new TH2F("c1_vs_ps","c1_vs_ps",4096, 0, 4096, 4096, 0, 4096);
	h_C1_vs_PS->GetXaxis()->SetTitle("C1(ADC Counts)");
	h_C1_vs_PS->GetYaxis()->SetTitle("PS (ADC Counts)");

	TH2F *h_C2_vs_PS = new TH2F("c2_vs_ps","c2_vs_ps",4096, 0, 4096, 4096, 0, 4096);
	h_C2_vs_PS->GetXaxis()->SetTitle("C2(ADC Counts)");
	h_C2_vs_PS->GetYaxis()->SetTitle("PS (ADC Counts)");


	// Preshower histo
	TH1F *h_PS = new TH1F("Preshower_nocut", "Preshower_nocut", 1024, 0,4096);
        h_PS->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_PS_no_muon = new TH1F("Preshower_no_muon", "Preshower_no_muon", 4096, 0,4096);
        h_PS_no_muon->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_PS_C1 = new TH1F("Preshower_C1", "Preshower_C1", 1024, 0,4096);
        h_PS_C1->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_PS_C2 = new TH1F("Preshower_C2", "Preshower_C2", 1024, 0,4096);
        h_PS_C2->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_PS_ele_loose = new TH1F("Preshower_ele_loose", "Preshower_ele_or", 1024, 0,4096);
        h_PS_ele_loose->GetXaxis()->SetTitle("ADC Counts");
        h_PS_ele_loose->SetLineColor(kMagenta+1); 

        TH1F *h_PS_ele_medium = new TH1F("Preshower_ele_medium", "Preshower_ele_and", 1024, 0,4096);
        h_PS_ele_medium->GetXaxis()->SetTitle("ADC Counts");
        h_PS_ele_medium->SetLineColor(kRed);

        TH1F *h_PS_ele_medium2 = new TH1F("Preshower_ele_medium2", "Preshower_ele_psonly", 1024, 0,4096);
        h_PS_ele_medium2->GetXaxis()->SetTitle("ADC Counts");
        h_PS_ele_medium2->SetLineColor(kGreen+1);



 	// Calo info

	TH1F *h_C_over_S = new TH1F("C/S","C/S", 200, -0.5, 5);
        h_C_over_S->GetXaxis()->SetTitle("C/S");

	TH2F *h_cs = new TH2F("C_vs_S", "C_vs_S", 4096, 0,4096, 4096, 0,4096);
        h_cs->GetYaxis()->SetTitle("S(ADC Counts)");
        h_cs->GetXaxis()->SetTitle("C(ADC Counts)");

	TH2F *h_cs_ps = new TH2F("CS_vs ps", "CS_vs_ps", 4096, 0,4096, 100,0,5);
        h_cs_ps->GetYaxis()->SetTitle("C/S");
        h_cs_ps->GetXaxis()->SetTitle("Preshower(ADC Counts)");
	

	int nbin =1024;
        int xlow = 0;
        int xhigh =4096;


        TH1F *h_s[8];
        TH1F *h_s_ele_loose[8];
        TH1F *h_s_ele_medium[8];
        TH1F *h_s_ele_medium2[8];
        TH1F *h_s_ele_tight1[8];
        TH1F *h_s_ele_tight2[8];
        TH1F *h_s_pi[8];
        for(int n=0;n<8; n++){
                stringstream hname1;
                stringstream htitle1;
                stringstream hname2;
                stringstream htitle2;
                stringstream hname3;
                stringstream htitle3;
                stringstream hname4;
                stringstream htitle4;
                stringstream hname5;
                stringstream htitle5;
                stringstream hname6;
                stringstream htitle6;
                stringstream hname7;
                stringstream htitle7;
                hname1<<"h_S_"<<n+1;
                hname2<<"h_S_ele_loose_"<<n+1;
                hname3<<"h_S_ele_medium_"<<n+1;
                hname7<<"h_S_ele_medium2_"<<n+1;
                hname4<<"h_S_ele_tight1_"<<n+1;
                hname5<<"h_S_ele_tight2_"<<n+1;
                hname6<<"h_S_pi_"<<n+1;
                htitle1 << "Scint_"<<n+1;
                htitle2 << "Scint_ele_loose_"<<n+1;
                htitle3 << "Scint_ele_medium_"<<n+1;
                htitle7 << "Scint_ele_medium22_"<<n+1;
                htitle4 << "Scint_ele_tight1_"<<n+1;
                htitle5 << "Scint_ele_tight2_"<<n+1;
                htitle6 << "Scint_pi_"<<n+1;
                h_s[n]=new TH1F(hname1.str().c_str(), htitle1.str().c_str(), nbin, xlow,xhigh);
                h_s[n]->GetXaxis()->SetTitle("ADC counts");
		h_s[n]->SetLineColor(kBlue);
                h_s_ele_loose[n]=new TH1F(hname2.str().c_str(), htitle2.str().c_str(), nbin, xlow,xhigh);
                h_s_ele_loose[n]->GetXaxis()->SetTitle("ADC counts");
		h_s_ele_loose[n]->SetLineColor(kMagenta+1);
                h_s_ele_medium[n]=new TH1F(hname3.str().c_str(), htitle3.str().c_str(), nbin, xlow,xhigh);
                h_s_ele_medium[n]->GetXaxis()->SetTitle("ADC counts");
		h_s_ele_medium[n]->SetLineColor(kRed);
                h_s_ele_medium2[n]=new TH1F(hname7.str().c_str(), htitle7.str().c_str(), nbin, xlow,xhigh);
                h_s_ele_medium2[n]->GetXaxis()->SetTitle("ADC counts");
		h_s_ele_medium2[n]->SetLineColor(kGreen+1);
                h_s_ele_tight1[n]=new TH1F(hname4.str().c_str(), htitle4.str().c_str(), nbin, xlow,xhigh);
                h_s_ele_tight1[n]->GetXaxis()->SetTitle("ADC counts");
		h_s_ele_tight1[n]->SetLineColor(kGreen+2);
                h_s_ele_tight2[n]=new TH1F(hname5.str().c_str(), htitle5.str().c_str(), nbin, xlow,xhigh);
                h_s_ele_tight2[n]->GetXaxis()->SetTitle("ADC counts");
		h_s_ele_tight2[n]->SetLineColor(kBlack);
                h_s_pi[n]=new TH1F(hname6.str().c_str(), htitle6.str().c_str(), nbin, xlow,xhigh);
                h_s_pi[n]->GetXaxis()->SetTitle("ADC counts");
		h_s_pi[n]->SetLineColor(kCyan+2);
       } 


	// cut parameter	
	Int_t C1_cut=85;  // C1 pedesta
	Int_t C2_cut=30;  // C2 pedestal
	Int_t mu_cut=270; // mu pedestal
	Int_t ps_ped_cut=230; // ps pedestal   
	Int_t ps_phys_cut=400; // ps pedestal   

	bool isMuon=false;
	bool isEleLoose=false;
	bool isEleMedium=false; 
	bool isEleMedium2=false; 
	bool isEleTight1=false;
	bool isEleTight2=false;
	bool isPion=false; 


	// counters
	Int_t n_phys_in_spill=0;
	Int_t n_mu =0;
	Int_t n_e_c1=0; 
	Int_t n_e_c2=0;  
	Int_t n_e_loose=0; 
	Int_t n_e_medium=0; 
	Int_t n_e_medium2=0; 
	Int_t n_e_tight1=0; 
	Int_t n_e_tight2=0; 
	Int_t n_pi=0; 

	Double_t cs_ratio=0;


	// Running on events
	count=(Int_t) t->GetEntries();
        cout<< "Numb evt " << count<<"\n";

	for(int i=0;i<count;i++){

           if(i%10000==0) cout << " Processing event" << i << endl;
	   t->GetEntry(i);

	  isMuon=false;
	  isEleLoose=false;
	  isEleMedium=false; 
	  isEleMedium2=false; 
	  isEleTight1=false;
	  isEleTight2=false;
	  isPion=false; 
	   
	  h_mask->Fill(TriggerMask); 

	   // get Pedestal for Cherenkov
	   if(TriggerMask==6){
		h_C1_ped->Fill(ADCs[i_c1]);
		h_C2_ped->Fill(ADCs[i_c2]);
	   }

	   //Phys events
	   if(TriggerMask==5){

		// Counts phys evts
		n_phys_in_spill++;

		// uncutted histo
	 	h_mu->Fill(ADCs[i_mu]); 
	        h_PS->Fill(ADCs[i_ps]);
		for(int i=0; i<8;i++){
		  h_s[i]->Fill(ADCs[i+8]);
		}
		
	        // defining cuts
	        // isMuon
	        if(ADCs[i_mu]> mu_cut){
		   isMuon=true;
		   n_mu++; 
		}
 
	        // cutting pedestal in ps and muons
		if(ADCs[i_ps]>ps_ped_cut && ADCs[i_mu]<mu_cut){ 
	           h_PS_no_muon->Fill(ADCs[i_ps]); 

		   h_C1->Fill(ADCs[i_c1]);
	   	   h_C2->Fill(ADCs[i_c2]);
		   h_C1C2->Fill(ADCs[i_c1],ADCs[i_c2]);
		   h_C1_vs_PS->Fill(ADCs[i_c1],ADCs[i_ps]);
		   h_C2_vs_PS->Fill(ADCs[i_c2],ADCs[i_ps]);
	
		   
		   if(tower!=0){
		       cs_ratio=Double_t(ADCs[ch])/Double_t(ADCs[ch+8]);
		       h_cs->Fill(ADCs[ch], ADCs[ch+8]);
		       h_cs_ps->Fill(ADCs[i_ps],cs_ratio);	
		       h_C_over_S->Fill(cs_ratio);
		   }

 		   if(ADCs[i_c1]>C1_cut){
			n_e_c1++;
			h_PS_C1->Fill(ADCs[i_ps]);
		   }

		   if(ADCs[i_c2]>C2_cut){
			n_e_c2++;
			h_PS_C2->Fill(ADCs[i_ps]);
		   }

		   // loose ele
		   if(ADCs[i_c1]>C1_cut ||  ADCs[i_c2]> C2_cut){
	                isEleLoose=true;
			n_e_loose++;
                        h_PS_ele_loose->Fill(ADCs[i_ps]);
		   	for(int i=0; i<8;i++){
			   h_s_ele_loose[i]->Fill(ADCs[i+8]);
		    	}
		   }

		   // medium ele
		   if(ADCs[i_c1]>C1_cut &&  ADCs[i_c2]> C2_cut){
	                isEleMedium=true;
			n_e_medium++;
                        h_PS_ele_medium->Fill(ADCs[i_ps]);
		   	for(int i=0; i<8;i++){
			   h_s_ele_medium[i]->Fill(ADCs[i+8]);
		    	}
		   }

		   if(ADCs[i_ps]>ps_phys_cut){
                        isEleMedium2=true;
                        n_e_medium2++;
                        h_PS_ele_medium2->Fill(ADCs[i_ps]);
                        for(int i=0; i<8;i++){
                           h_s_ele_medium2[i]->Fill(ADCs[i+8]);
                        }
                   }

		   // tight 1 ele (isEleLoose && ps_phys_cut)
	           if(isEleLoose && ADCs[i_ps]>ps_phys_cut ){
			n_e_tight1++;
			isEleTight1=true;
		   	for(int i=0; i<8;i++){
			   h_s_ele_tight1[i]->Fill(ADCs[i+8]);
		    	}
		   }
		   // tight 2 ele (isEleLoose && ps_phys_cut)
	           if(isEleMedium && ADCs[i_ps]>ps_phys_cut ){
			n_e_tight2++;
			isEleTight2=true;
		   	for(int i=0; i<8;i++){
			   h_s_ele_tight2[i]->Fill(ADCs[i+8]);
		    	}
		   }
		   if(!isEleLoose){
			n_pi++;
			isPion=true;	
		   	for(int i=0; i<8;i++){
		           h_s_pi[i]->Fill(ADCs[i+8]);
		    	}
		   }// cut on PS (pions)
		} // end cut ped in ps and muons

		
	   }// onlys phys trigger


       }// end loop on event


        ofstream Data;
        ostringstream mydata;
        mydata << OUTDIR << "beamcomposition.txt" ;
        Data.open(mydata.str().c_str(), ofstream::out | ofstream::app);
        cout << "Text output file1: " << endl << mydata.str() << endl;


	cout << "Tot n. phys events: " << n_phys_in_spill << endl;
        cout << "muon events " << n_mu << " fraction " << std::fixed << std::setprecision(1) << double(n_mu)*100/double(n_phys_in_spill) <<endl;
	cout << "e-like particle from C1 "<< n_e_c1 << endl;  
	cout << "e-like particle from C2 "<< n_e_c2 << endl;  
	cout << "e_loose (C1 || C2) " << n_e_loose << " fraction " << std::fixed << std::setprecision(1) <<double(n_e_loose)*100/double(n_phys_in_spill) << endl; 
	cout << "e_medium (C1 && C2) " << n_e_medium << " fraction " << std::fixed << std::setprecision(1) << double(n_e_medium)*100/double(n_phys_in_spill) <<endl; 
	cout << "e_tight1 (loose & PS) " << n_e_tight1 << " fraction " << std::fixed << std::setprecision(1) << double(n_e_tight1)*100/double(n_phys_in_spill) <<endl; 
	cout << "e_tight2 (medium & PS) " << n_e_tight2 << " fraction " << std::fixed << std::setprecision(1) << double(n_e_tight2)*100/double(n_phys_in_spill) <<endl; 
	cout << "e_medium2 (PS ONLY) " << n_e_medium2 << " fraction " << std::fixed << std::setprecision(1) << double(n_e_medium2)*100/double(n_phys_in_spill) <<endl; 
	cout << "pi-like particle (!EleLoose) : " << n_pi << " fraction " << std::fixed << std::setprecision(1) << double(n_pi)*100/double(n_phys_in_spill) << endl; 


	Data << E << "\t" << std::fixed << std::setprecision(1) << double(n_mu)*100/double(n_phys_in_spill) 
		  << "\t" << std::fixed << std::setprecision(1) << double(n_e_loose)*100/double(n_phys_in_spill) 
		  << "\t" << std::fixed << std::setprecision(1) << double(n_e_medium)*100/double(n_phys_in_spill)
		  << "\t" << std::fixed << std::setprecision(1) << double(n_e_tight1)*100/double(n_phys_in_spill)
		  << "\t" << std::fixed << std::setprecision(1) << double(n_e_tight2)*100/double(n_phys_in_spill)
		  << "\t" << std::fixed << std::setprecision(1) << double(n_e_medium2)*100/double(n_phys_in_spill)
		  << "\t" << std::fixed << std::setprecision(1) << double(n_pi)*100/double(n_phys_in_spill) << endl;


        TCanvas *c1=new TCanvas("Trigger Masks","Trigger Masks", 700, 700);
        c1->Divide(1,2); 
	c1->cd(1);
	gPad->SetLogy();
        h_mask->Draw();
        c1->cd(2);
	gPad->SetLogy();
        h_mu->Draw();
	c1->Update();
        c1->Draw();


        TCanvas *c2=new TCanvas("Preshower","Preshower", 700, 700);
        c2->Divide(2,2);
	c2->cd(1);       
        gPad->SetLogy();
	h_PS->Draw();
	c2->cd(2);
	gPad->SetLogy();
        h_PS_no_muon->Draw();
	c2->cd(3);
	gPad->SetLogy();
        h_PS_C1->Draw();
	c2->cd(4);
	gPad->SetLogy();
        h_PS_C2->Draw();
        c2->Update();
        c2->Draw();

        TCanvas *c6=new TCanvas("Cherenkov counters","Cherenkov counters", 1000, 1000);
	c6->Divide(2,2); 
	c6->cd(1);
	gPad->SetLogy();
        h_C1->Draw(); 
	c6->cd(2);
	gPad->SetLogy();
	h_C2->Draw(); 
	c6->cd(3);
	gPad->SetLogy();
	h_C1_ped->Draw(); 
	c6->cd(4);
	gPad->SetLogy();
	h_C2_ped->Draw(); 

        TCanvas *c7=new TCanvas("Cherenkov scatter","Cherenkov scatter", 1000, 1000);
	c7->Divide(2,2); 
	c7->cd(1);
	h_C1C2->Draw("colz"); 
	c7->cd(3);
	h_C1_vs_PS->Draw("colz"); 
	c7->cd(4);
	h_C2_vs_PS->Draw("colz"); 

        TCanvas *c4=new TCanvas("PS_ele","PS_ele", 700, 700);
	gPad->SetLogy();
	gStyle->SetOptTitle(0);
	h_PS->Draw();
	h_PS_ele_loose->Draw("SAME");
	h_PS_ele_medium->Draw("SAME");
	h_PS_ele_medium2->Draw("SAME");
	TLegend *l = new TLegend(0.4,0.7,0.7,0.9);
	l->AddEntry(h_PS,"Preshower no cut","l");
	l->AddEntry(h_PS_ele_loose,"Preshower EleLoose","l");
	l->AddEntry(h_PS_ele_medium,"Preshower EleMedium","l");
	l->AddEntry(h_PS_ele_medium2,"Preshower Only","l");
        l->Draw();
	c4->Update();
        c4->Draw();

	if(tower!=0){

	    TCanvas *c5=new TCanvas("OneTowerScint","OneTowerScint",1000, 1000);  
            c5->Divide(1,3);
	    c5->cd(1); 
	    h_C_over_S->Draw();
	    c5->cd(2);
	    h_cs->Draw("COLZ");
	    c5->cd(3); 
	    h_cs_ps->Draw("colz");

	}

	

        TCanvas *c3=new TCanvas("Calorimeter","Calorimeter", 700, 700);
	gPad->SetLogy();
	gStyle->SetOptTitle(0);
	h_s[ch]->Draw();
	h_s_ele_loose[ch]->Draw("SAME");
	h_s_ele_medium[ch]->Draw("SAME");
	h_s_ele_medium2[ch]->Draw("SAME");
	h_s_ele_tight1[ch]->Draw("SAME");
	h_s_ele_tight2[ch]->Draw("SAME");
	h_s_pi[ch]->Draw("SAME");
	TLegend *l1 = new TLegend(0.4,0.7,0.7,0.9);
	l1->AddEntry(h_s[ch],"Calo Scint no cut","l");
	l1->AddEntry(h_s_ele_loose[ch],"Calo Scint EleLoose","l");
	l1->AddEntry(h_s_ele_medium[ch],"Calo Scint EleMedium","l");
	l1->AddEntry(h_s_ele_medium2[ch],"Calo Scint EleMedium2","l");
	l1->AddEntry(h_s_ele_tight1[ch],"Calo Scint EleTight1","l");
	l1->AddEntry(h_s_ele_tight2[ch],"Calo Scint EleTight2","l");
	l1->AddEntry(h_s_pi[ch],"Calo Scint Pions","l");
        l1->Draw();
        c3->Update();
        c3->Draw();




/*

	TCanvas *c10=new TCanvas("Scintillation","Scintillation",1000, 1000);
        c10->Divide(3,3);
        c10->cd(1);
        gPad->SetLogy();
        h_s[7]->Draw();
        c10->cd(2);
        gPad->SetLogy();
        h_s[6]->Draw();
        c10->cd(3);
        gPad->SetLogy();
        h_s[5]->Draw();
        c10->cd(4);
        gPad->SetLogy();
        h_s[4]->Draw();
        c10->cd(6);
        gPad->SetLogy();
        h_s[3]->Draw();
        c10->cd(7);
        gPad->SetLogy();
        h_s[2]->Draw();
        c10->cd(8);
        gPad->SetLogy();
        h_s[1]->Draw();
        c10->cd(9);
        gPad->SetLogy();
        h_s[0]->Draw();
        c10->Update();
        c10->Draw();


	TCanvas *c11=new TCanvas("Scintillation_cut","Scintillation_cut",1000, 1000);
        c11->Divide(3,3);
        c11->cd(1);
        gPad->SetLogy();
        h_s_pi[7]->Draw();
        h_s_ele[7]->Draw("SAME");
        c11->cd(2);
        gPad->SetLogy();
        h_s_pi[6]->Draw();
        h_s_ele[6]->Draw("SAME");
        c11->cd(3);
        gPad->SetLogy();
        h_s_pi[5]->Draw();
        h_s_ele[5]->Draw("SAME");
        c11->cd(4);
        gPad->SetLogy();
        h_s_pi[4]->Draw();
        h_s_ele[4]->Draw("SAME");
        c11->cd(6);
        gPad->SetLogy();
        h_s_pi[3]->Draw();
        h_s_ele[3]->Draw("SAME");
        c11->cd(7);
        gPad->SetLogy();
        h_s_pi[2]->Draw();
        h_s_ele[2]->Draw("SAME");
        c11->cd(8);
        gPad->SetLogy();
        h_s_pi[1]->Draw();
        h_s_ele[1]->Draw("SAME");
        c11->cd(9);
        gPad->SetLogy();
        h_s_pi[0]->Draw();
        h_s_ele[0]->Draw("SAME");
        c11->Update();
        c11->Draw();
*/

	return; 

}


