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


//#define DATADIR "/eos/user/i/ideadr/TB2021_H8/rawNtuple/"
#define DATADIR "/afs/cern.ch/user/g/gaudio/data/"
// change this output local dir to your
//#define OUTDIR "/afs/cern.ch/user/g/gaudio/public/Dream/Prototype2020/DataAnalysis/202108_SPS/"
#define OUTDIR "/eos/user/i/ideadr/TB2021_Desy/recoNtuple/"
//#define CALIBDIR "/eos/user/i/ideadr/TB2021_Desy/recoNtuple/CalibFiles/"

// data file name looks like sps2021data.run627.root


void CheckBeam(string run, int tower){

        gStyle->SetOptStat(111111);
        gStyle->SetPalette(1);
        gStyle->SetPaintTextFormat(".3f");
        gStyle->SetPadRightMargin(0.15);
        gStyle->SetNumberContours(100);
        gStyle->SetTextSize(0.05);

	Int_t ch= tower-1; 


	stringstream myfile;
        myfile << DATADIR  << run;
        cout << "opening ntuple input file ... " <<  myfile.str() << " steering in Tower " << tower <<endl;
        TFile *f=new TFile(myfile.str().c_str());
        TTree *t = (TTree*) f->Get("CERNSPS2021");

        Int_t EventNumber = 0 ;
        Long64_t TriggerMask =0;
        Int_t ADCs[96];

	Int_t count;

	Int_t i_ps=16;
	Int_t i_mu=32;
	Int_t i_c1=64;
	Int_t i_c2=65;


        t->SetBranchAddress("EventNumber",&EventNumber);
        t->SetBranchAddress("TriggerMask",&TriggerMask);
        t->SetBranchAddress("ADCs",ADCs);

	TH1F *h_mask = new TH1F("trigger masks","trigger masks", 8, -0.5, 7.5);

	TH1F *h_C1 = new TH1F("Cherenkov1","Cherenkov1", 1024, 0,1024);
	h_C1->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_C1_ped = new TH1F("Cherenkov1_ped","Cherenkov1_ped", 1024, 0, 1024);
	h_C1_ped->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_C2 = new TH1F("Cherenkov2","Cherenkov2", 1024, 0, 1024);
	h_C2->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_C2_ped = new TH1F("Cherenkov2_ped","Cherenkov2_ped", 1024, 0, 1024);
	h_C2_ped->GetXaxis()->SetTitle("ADC Counts");

	TH2F *h_C1C2 = new TH2F("c1_vs_c2","c1_vs_c2",	1024, 0, 1024, 1024, 0, 1024);
	h_C1C2->GetXaxis()->SetTitle("C1 (ADC Counts)");
	h_C1C2->GetYaxis()->SetTitle("C2 (ADC Counts)");

	TH2F *h_C2_vs_PS = new TH2F("c2_vs_ps","c2_vs_ps",1024, 0, 1024, 4096, 0, 4096);
	h_C2_vs_PS->GetXaxis()->SetTitle("C2(ADC Counts)");
	h_C2_vs_PS->GetYaxis()->SetTitle("PS (ADC Counts)");

	TH1F *h_PS = new TH1F("Preshower", "Preshower", 4096, 0,4096);
        h_PS->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_PS_no_muon = new TH1F("Preshower_no_muon", "Preshower_no_muon", 4096, 0,4096);
        h_PS_no_muon->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_PS_C1 = new TH1F("Preshower_C1", "Preshower_C1", 1024, 0,4096);
        h_PS_C1->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_PS_C2 = new TH1F("Preshower_C2", "Preshower_C2", 1024, 0,4096);
        h_PS_C2->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_PS_ele = new TH1F("Preshower_ele", "Preshower_ele", 1024, 0,4096);
        h_PS_ele->GetXaxis()->SetTitle("ADC Counts");

	TH1F *h_mu = new TH1F("MuonCounter", "MuonCounter", 4096, 0,4096);
        h_mu->GetXaxis()->SetTitle("ADC Counts");

	//TH2F *h_ps_vs_calo = new TH2F("Calo vs PS","Calo vs PS", 2048, 0,2048, 2048, 0,2048);
        //h_ps_vs_calo->GetYaxis()->SetTitle("Calo(ADC Counts)");
        //h_ps_vs_calo->GetXaxis()->SetTitle("Preshower(ADC Counts)");

	TH1F *h_C_over_S = new TH1F("C/S","C/S", 200, -0.5, 5);

	TH2F *h_cs = new TH2F("C_vs_S", "C_vs_S", 4096, 0,4096, 4096, 0,4096);
        h_cs->GetYaxis()->SetTitle("S(ADC Counts)");
        h_cs->GetXaxis()->SetTitle("C(ADC Counts)");

	TH2F *h_cs_ps = new TH2F("CS_vs ps", "CS_vs_ps", 4096, 0,4096, 100,0,5);
        h_cs_ps->GetYaxis()->SetTitle("C/S");
        h_cs_ps->GetXaxis()->SetTitle("Preshower(ADC Counts)");
	

	int nbin =4096;
        int xlow = 0;
        int xhigh =4096;


        TH1F *h_s[8];
        TH1F *h_s_ele[8];
        TH1F *h_s_pi[8];
        for(int n=0;n<8; n++){
                stringstream hname1;
                stringstream htitle1;
                stringstream hname2;
                stringstream htitle2;
                stringstream hname3;
                stringstream htitle3;
                hname1<<"h_S_"<<n+1;
                hname2<<"h_S_ele_"<<n+1;
                hname3<<"h_S_pi_"<<n+1;
                htitle1 << "Scint_"<<n+1;
                htitle2 << "Scint_ele_"<<n+1;
                htitle3 << "Scint_pi_"<<n+1;
                h_s[n]=new TH1F(hname1.str().c_str(), htitle1.str().c_str(), nbin, xlow,xhigh);
                h_s[n]->GetXaxis()->SetTitle("ADC counts");
                h_s_ele[n]=new TH1F(hname2.str().c_str(), htitle2.str().c_str(), nbin, xlow,xhigh);
                h_s_ele[n]->GetXaxis()->SetTitle("ADC counts");
                h_s_pi[n]=new TH1F(hname3.str().c_str(), htitle3.str().c_str(), nbin, xlow,xhigh);
                h_s_pi[n]->GetXaxis()->SetTitle("ADC counts");
		h_s_pi[n]->SetLineColor(2);
       } 

 

	Int_t n_mu =0; 
	Int_t n_ps =0;
	Int_t n_phys_in_spill=0;
	Int_t n_pi_calo=0;  
	Int_t n_e_like=0; 
	Int_t n_e_like_c1=0; 
	Int_t n_e_like_c2=0; 
	Int_t n_pi_like=0; 

	Double_t cs_ratio=0;

	count=(Int_t) t->GetEntries();


        cout<< "Numb evt " << count<<"\n";



	for(int i=0;i<count;i++){

           if(i%10000==0) cout << " Processing event" << i << endl;
	   t->GetEntry(i);

	   h_mask->Fill(TriggerMask); 


	   if(TriggerMask==6){
		h_C1_ped->Fill(ADCs[i_c1]);
		h_C2_ped->Fill(ADCs[i_c2]);
	   }


	   if(TriggerMask==5){

		n_phys_in_spill++;
	        h_PS->Fill(ADCs[i_ps]);

		for(int i=0; i<8;i++){
		  h_s[i]->Fill(ADCs[i+8]);
		}
 
		
		if(ADCs[i_mu]>270){
		   n_mu++; 
	 	   h_mu->Fill(ADCs[i_mu]); 
		}// selecting muons
 
		if(ADCs[i_ps]>230 && ADCs[i_mu]<270){ // cutting pedestal in ps, counting e,pi
		   n_ps++;
	           h_PS_no_muon->Fill(ADCs[i_ps]); 
		   //h_ps_vs_calo->Fill(ADCs[i_ps], ADCs[4]);

		   h_C1->Fill(ADCs[i_c1]);
	   	   h_C2->Fill(ADCs[i_c2]);
		   h_C1C2->Fill(ADCs[i_c1],ADCs[i_c2]);
		   h_C2_vs_PS->Fill(ADCs[i_c2],ADCs[i_ps]);
	
		   
		   if(tower!=0){
		       cs_ratio=Double_t(ADCs[ch])/Double_t(ADCs[ch+8]);
		       h_cs->Fill(ADCs[ch], ADCs[ch+8]);
		       h_cs_ps->Fill(ADCs[i_ps],cs_ratio);	
		       h_C_over_S->Fill(cs_ratio);
		   }

 		   if(ADCs[i_c1]>90){
			n_e_like_c1++;
			h_PS_C1->Fill(ADCs[i_ps]);
		   }

		   if(ADCs[i_c2]>90){
			n_e_like_c2++;
			h_PS_C2->Fill(ADCs[i_ps]);
		   }
	
	           if(ADCs[i_c1]>90 || ADCs[i_c2]> 40){
			n_e_like++;
		   	for(int i=0; i<8;i++){
		           h_PS_ele->Fill(ADCs[i_ps]);
			   h_s_ele[i]->Fill(ADCs[i+8]);
		    	}
		   }// cut on PS (ele)
		   else if(ADCs[i_c1]<90 &&  ADCs[i_c2]< 40) {
			n_pi_like++;	
		   	for(int i=0; i<8;i++){
		           h_s_pi[i]->Fill(ADCs[i+8]);
		    	}
		 }// cut on PS (pions)
		} // end cut ped in ps and muons

		
	   }// onlys phys trigger


       }// end loop on event

	cout << "Tot n. phys events: " << n_phys_in_spill << endl;
        cout << "muon events " << n_mu << endl;
        cout << "e or pi hitting the preshower: " << n_ps << endl; 
	cout << "e-like particle from C1 or C2 " << n_e_like << endl; 
	cout << "e-like particle from C1 " << n_e_like_c1 << endl; 
	cout << "e-like particle from C2 " << n_e_like_c2 << endl; 
	cout << "pi-like particle from !C1 && !C2 : " << n_pi_like << endl; 

        TCanvas *c1=new TCanvas("Trigger Masks","Trigger Masks", 700, 700);
        gPad->SetLogy();
        h_mask->Draw();
        //h_mask->Write();
        c1->Update();
        c1->Draw();

	TCanvas *c3=new TCanvas("MuonCounter","MuonCounter", 700, 700);
        gPad->SetLogy();
        h_mu->Draw();
        c3->Update();
        c3->Draw();

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

        TCanvas *c4=new TCanvas("PS_ele","PS_ele", 700, 700);
	h_PS_ele->Draw("colz");
        c4->Update();
        c4->Draw();


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
	c7->Divide(1,2); 
	c7->cd(1);
	h_C1C2->Draw("colz"); 
	c7->cd(2);
	h_C2_vs_PS->Draw("colz"); 

	if(tower!=0){

	    TCanvas *c5=new TCanvas("OneTowerScint","OneTowerScint",1000, 1000);  
            c5->Divide(2,2);
	    c5->cd(1); 
            gPad->SetLogy();
	    h_s_ele[ch]->Draw();
            h_s_pi[ch]->Draw("SAME");   
	    c5->cd(2);
	    h_C_over_S->Draw();
	    c5->cd(3);
	    h_cs->Draw("COLZ");
	    c5->cd(4); 
	    h_cs_ps->Draw("colz");

	}



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
        h_s_ele[7]->Draw();
        h_s_pi[7]->Draw("SAME");
        c11->cd(2);
        gPad->SetLogy();
        h_s_ele[6]->Draw();
        h_s_pi[6]->Draw("SAME");
        c11->cd(3);
        gPad->SetLogy();
        h_s_ele[5]->Draw();
        h_s_pi[5]->Draw("SAME");
        c11->cd(4);
        gPad->SetLogy();
        h_s_ele[4]->Draw();
        h_s_pi[4]->Draw("SAME");
        c11->cd(6);
        gPad->SetLogy();
        h_s_ele[3]->Draw();
        h_s_pi[3]->Draw("SAME");
        c11->cd(7);
        gPad->SetLogy();
        h_s_ele[2]->Draw();
        h_s_pi[2]->Draw("SAME");
        c11->cd(8);
        gPad->SetLogy();
        h_s_ele[1]->Draw();
        h_s_pi[1]->Draw("SAME");
        c11->cd(9);
        gPad->SetLogy();
        h_s_ele[0]->Draw();
        h_s_pi[0]->Draw("SAME");
        c11->Update();
        c11->Draw();


	return; 

}


