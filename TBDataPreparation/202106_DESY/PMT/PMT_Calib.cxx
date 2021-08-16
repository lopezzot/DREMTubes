#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <TTree.h>
#include <TBranch.h>
#include <TFile.h>
#include <TH1.h>
#include <TMath.h>


#define path "/Users/gabriella/cernbox/DualReadout/Desy/Plot/"


void PMT_Calib(int step){

	gStyle->SetOptStat(1); 


	// STEP 1: find PMT equalisation and write equalisation constant
	// STEP 2: get all the histo and add them to find the best energy calib constant
		
	if(step==1){


	        std::ofstream common;
	        std::ostringstream equalfile;
        	equalfile << path << "equalization.txt" ;
        	common.open(equalfile.str().c_str(), std::ofstream::out | std::ofstream::app);
        	std::cout << "Text output file: " << std::endl << equalfile.str() << std::endl;


        	ostringstream mean;
        	mean << path <<  "PMT_calib.txt";
        	cout << mean.str().c_str() << std::endl;

		int tow; 
        	double c,s; 
        	vector<double> c_scale; 
        	vector<double> s_scale; 
		double c0, s0; 

        	std::ifstream in0(mean.str());
        	if (!in0 || in0.bad()) return 0; // sanity check

		int j=0; 
        	while (1) {
                	in0 >> tow >> s >> c;
                	if (!in0.good()) break;
			if(j==0) {
				c0=c; 
				s0=s; 
			}
                	std::cout << s0 << " " << s  << " " << s0/s <<  std::endl;
                	std::cout << c0 << " " << c  << " " << c0/c <<  std::endl;
                	c_scale.push_back(c0/c);
                	s_scale.push_back(s0/s);
			j++; 

        	}


		for(int k=0; k<c_scale.size(); k++){
			cout << s_scale.at(k) << " " << c_scale.at(k) << endl;
			common << s_scale.at(k) << " " << c_scale.at(k) << endl;
		}

	
	} // end step 0=equal. 
	
        if(step==2){
		//with filters
		//int myrun[8]={79, 80, 81, 82, 83, 84, 85, 86};
		//without filters on S
		int myrun[8]={182,183,185,190,186,189,188,187};
		ostringstream hfile[8];
		TFile *f[8]; 

        	TH1F *h[16];
        	TH1F *h_norm[16];
		stringstream hname_C[8];
		stringstream hname_S[8];
		//stringstream hname_NC[8];
		//stringstream hname_NS[8];

		int nbin =4096;
        	int xlow = 0;
        	int xhigh =4096;
        	int nbin_c =2048;
        	int xlow_c = 0;
        	int xhigh_c =2048;

		TH1F *allS; 
		TH1F *allC;


		for(int i=0; i<8; i++){
		//for(int i=0; i<1; i++){
			hfile[i] << path << "desydata.run" << myrun[i] << ".root";
			cout << hfile[i].str() << endl; 

			f[i] = new TFile(hfile[i].str().c_str());
			hname_C[i] << "h_C_norm" << i+1;
			hname_S[i] << "h_S_norm" << i+1;
			cout << hname_C[i].str() << " " << hname_S[i].str() << endl;

			h[i]=(TH1F*)f[i]->Get(hname_C[i].str().c_str());
			h[i]->GetXaxis()->SetRangeUser(40, 1000);
                	int binmax1 = h[i]->GetMaximumBin();
                	double x_C = h[i]->GetXaxis()->GetBinCenter(binmax1);


			h[i+8]=(TH1F*)f[i]->Get(hname_S[i].str().c_str());
			h[i+8]->GetXaxis()->SetRangeUser(150, 3000);
                	int binmax2 = h[i+8]->GetMaximumBin();
                	double x_S = h[i+8]->GetXaxis()->GetBinCenter(binmax2);

			cout << i << " S_peak " << x_S << " C_peak " << x_C << endl; 


			if(i==0){
			   allC=(TH1F*)h[i]->Clone("allC");
			   allS=(TH1F*)h[i+8]->Clone("allS");
			}
			else{
			   allS->Add(h[i+8]);
			   allC->Add(h[i]);
			
			}
		}



	        TCanvas *s1=new TCanvas("Scintillation","Scintillation",1000, 1000);
       		s1->Divide(3,3);
        	s1->cd(1);
        	gPad->SetLogy();
        	h[15]->Draw();
        	s1->cd(2);
        	gPad->SetLogy();
        	h[14]->Draw();
        	s1->cd(3);
        	gPad->SetLogy();
        	h[13]->Draw();
        	s1->cd(4);
        	gPad->SetLogy();
        	h[12]->Draw();
        	s1->cd(6);
        	gPad->SetLogy();
        	h[11]->Draw();
        	s1->cd(7);
        	gPad->SetLogy();
        	h[10]->Draw();
        	s1->cd(8);
        	gPad->SetLogy();
        	h[9]->Draw();
        	s1->cd(9);
        	gPad->SetLogy();
        	h[8]->Draw();
        	s1->Update();
        	s1->Draw();




		TCanvas *c1=new TCanvas("Cherenkov","Cherenkov",1000, 1000);
        	c1->Divide(3,3);
        	c1->cd(1);
        	gPad->SetLogy();
        	h[7]->Draw();
        	c1->cd(2);
        	gPad->SetLogy();
        	h[6]->Draw();
        	c1->cd(3);
        	gPad->SetLogy();
        	h[5]->Draw();
        	c1->cd(4);
        	gPad->SetLogy();
        	h[4]->Draw();
        	c1->cd(6);
        	gPad->SetLogy();
        	h[3]->Draw();
        	c1->cd(7);
        	gPad->SetLogy();
        	h[2]->Draw();
        	c1->cd(8);
        	gPad->SetLogy();
        	h[1]->Draw();
        	c1->cd(9);
        	gPad->SetLogy();
        	h[0]->Draw();
        	c1->Draw();


		TCanvas *c6=new TCanvas("Cumulative","Cumulative", 1000, 700);
	        c6->Divide(1,2);
       		c6->cd(1);
        	gPad->SetLogy();
        	allC->Draw();
        	c6->cd(2);
        	gPad->SetLogy();
        	allS->Draw();
        	c6->Update();
        	c6->Draw();


		int binmax3 = allC->GetMaximumBin();
                double C_max = allC->GetXaxis()->GetBinCenter(binmax3);

		int binmax4 = allS->GetMaximumBin();
                double S_max = allS->GetXaxis()->GetBinCenter(binmax4);

		cout << " S_max "  << S_max << " C_max " << C_max << endl; 
		
		double contain = 0.70; // just to plug a number
		double energy = 6; // GeV (energy used for calib)
		
		double c_k = C_max/(contain*energy);
		double s_k = S_max/(contain*energy);

		cout << "my k factor: contain:  " << contain << " energy " << energy << " GeV " <<  endl; 
		cout << " S_k "  << s_k  << " C_k " << c_k << endl;	
		

                std::ofstream common;
                std::ostringstream constant;
                constant << path << "calib_constant_filter.txt" ;
                common.open(constant.str().c_str(), std::ofstream::out | std::ofstream::app);
                std::cout << "Text output file: " << std::endl << constant.str() << std::endl;
		
		double s, c; 
                ostringstream mean;
                mean << path <<  "equalization.txt";
                cout << mean.str().c_str() << std::endl;

                std::ifstream in0(mean.str());
                if (!in0 || in0.bad()) return 0; // sanity check

                int j=0;
                while (1) {
                        in0  >> s >> c;
                        if (!in0.good()) break;
			cout << s << " " << c << endl; 
			common << s_k *s << " " << c_k *c << endl; 
			cout << "s konstant " <<  s_k *s << " c konstant " << c_k *c << endl; 
                }


		}// end step 2	


	return; 

}


