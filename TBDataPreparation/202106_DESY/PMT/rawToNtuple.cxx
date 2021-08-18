//**********************************************************
// \file DrawToNtuple.cxx 
// \brief: Conversion of raw data txt file to raw Ntuple
// \author: Gabriella Gaudio  @gaudio74
// \start date: July 2021
//**********************************************************

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <TTree.h>
#include <TBranch.h>
#include <TFile.h>  
#include <TH1.h> 
#include <TMath.h>


//#define TXTDIR "/Users/gabriella/cernbox/DualReadout/Desy/TXT/"
#define DATADIR "/eos/user/i/ideadr/TB2021_Desy/rawData/"
#define OUTDIR "/eos/user/i/ideadr/TB2021_Desy/rawNtuple/"


int main(int argc, char **argv){

	if(argc <1){
		std::cout << "filename expected" << std::endl;
	}


	std::string fname(argv[1]);
        std::string fn1;
        std::string fn2;


        fn1=fname.substr(0, fname.find(".txt"));
        std::cout << "fn1 " << fn1 << std::endl;
        fn2=fn1.substr(fn1.find("/tmp/")+5);
        std::cout << "Run name " << fn2 << std::endl;

        // root file for the ntuple
        std::ostringstream rootout;
        rootout << OUTDIR << fn2 << ".root";
        TFile *file = new TFile(rootout.str().c_str(),"RECREATE");
        //file = TFile::Open(rootout.str().c_str());
       



	// branch for the Ntuple
	unsigned int Nevtda = 0 ;
	unsigned int TowerCts[3]={0};
	unsigned int TowerMask =0; 

	unsigned int CHADCN0[32];
	unsigned int CHARGEADCN0[32] ;

	// creating the TTree
	TTree *tree = new TTree("DREAM","Desy Ntuple");

	tree->Branch("Nevtda",&Nevtda,"Nevtda/i");
	tree->Branch("TowerCts",&TowerCts,"TowerCts[3]/i");
	tree->Branch("TowerMask",&TowerMask,"TowerMask/i");

	tree->Branch("CHADCN0",CHADCN0,"CHADCN0[32]/i");
	tree->Branch("CHARGEADCN0",CHARGEADCN0,"CHARGEADCN0[32]/i");


	//GG: uncomment for SiPM alignment check
	//std::ofstream Ped;
	//std::ostringstream mypedfile;
        //mypedfile << TXTDIR << "misalign.txt" ;
        //Ped.open(mypedfile.str().c_str(), std::ofstream::out | std::ofstream::app);
	//std::cout << "Text output file: " << std::endl << mypedfile.str() << std::endl;

	std::ostringstream rawinput; 
	rawinput  << argv[1] ; 
	std::cout << rawinput.str().c_str() << " " << rootout.str().c_str() << std::endl; 

	//GG: uncomment for SiPM alignment check
	/*
	std::ostringstream tidfile; 
	tidfile << TXTDIR <<  "tid_Run47.txt"; 
	std::cout << tidfile.str().c_str() <<  std::endl; 

        std::ifstream in0(tidfile.str());
        if (!in0 || in0.bad()) return 0; // sanity check

	int id; 
	std::vector<int> id_sipm; 

        while (1) {
                in0 >> id; 
		if (!in0.good()) break;
		//std::cout << id << std::endl;
		id_sipm.push_back(id); 
	}

	std::cout << " *** size id_sipm " << id_sipm.size() << std::endl; 
	*/


	Int_t j=0;
	std::string line;
	std::string data;
	std::string head;
	int ch[32];
	int adc[32];
	unsigned int nev=0;
        unsigned int ct1 = 0; 
	unsigned int ct2 = 0;
	unsigned int ct3 = 0; 
	unsigned int mask =0; 	


	// parsing data file
	std::ifstream in1(rawinput.str());
   	if (!in1 || in1.bad()) return 0; // sanity check

   	while (1) {
     		getline (in1,line);
     		if (!in1.good()) break;
		size_t pos=0; 
		// processing header part
     		pos = line.find("values:");
		head = line.substr(4,pos-4);
                std::stringstream hh(head);
                std::string mys;
                
                while(1){
                   hh >> mys >> nev >> mys >> mys >> ct1 >> ct2 >> ct3 >> mys >> mys >> mask;
                   if (!hh.good()) break;
		}
		//std::cout << nev << " " << ct1 << " " <<  ct2 << " " <<  ct3  << " " << mask<< std::endl;  
	
		TowerMask = mask;
	
		//GG: uncomment for SiPM alignment check
		/*
		if ( std::find(id_sipm.begin(), id_sipm.end(), nev) != id_sipm.end() ){
		      	if(mask == 2){
			       	//std::cout << " ****************  ped " << std::endl;  	
				//std::cout << "nev " << nev << " mask " << mask << std::endl; 
				Ped << nev << " " << mask << std::endl; 
			}
		}
		*/



		// processing data part
     		data  = line.substr(pos+8);
		//std::cout << "**** " << data << std::endl;
		std::stringstream ss(data);
		std::string tmp;
		std::vector<std::string> values;

     		while(getline(ss, tmp, ' ' )){
        		values.push_back(tmp);
     		}

		for(int k =0; k<values.size(); k++){
        		if(k%2==0) ch[k/2]=stol (values.at(k),nullptr,10);
        		if(k%2!=0) adc[k/2]=stol (values.at(k),nullptr,16);
     		}

		for(int k=0; k <32; k++){
			CHADCN0[k]=ch[k]; 
			CHARGEADCN0[k]=adc[k];
			//std::cout << "k= "<<  k << " " << ch[k] << " " << CHADCN0[k] << " " <<  adc[k] << " " << CHARGEADCN0[k] << std::endl;	
		}

		Nevtda=nev+1;
		TowerCts[0]= ct1; 
		TowerCts[1]= ct2; 
		TowerCts[2]= ct3; 


        	tree->Fill();
		j++;
		if (j%10000==0) std::cout << "processing event " << j << std::endl; 

	}
	//std::cout << nev << " " << ct1 << " " <<  ct2 << " " <<  ct3  << " " << mask<< std::endl;  
	std::cout << "loop on evts finished " << std::endl;
	std::cout << "Total number of event " << Nevtda << std::endl; 
	std::cout << "Physics Events:"  << TowerCts[0] << std::endl; 
	std::cout << "Pedestal Events: " << TowerCts[1] << std::endl;
	std::cout << "Sum check: " << TowerCts[0] + TowerCts[1] << std::endl;

        file = tree->GetCurrentFile();
	std::cout << "ready to write to " << file->GetName() <<  std::endl; 
	tree->Write();
        file->Close();


	return 0; 



} //end main

