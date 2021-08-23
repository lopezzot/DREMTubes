#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
//#include <string.h>
#include <TH1F.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include "ReadCalibCoeff.h"

#define nChan 10
#define nCell 1024

using namespace std;
//====================================================================
//
// This program runs on a root file obtained with the program
// raw2root_DRS.C of the format DRS_runnumber(%06d).root
// It adds to the file a "tree friend" (see root documentation
// on TTree::AddFriend ) with the data transformed in mV.
// It looks for the pedestal file DRS_runnumber(%06d)_ped.txt
// if the file does not exist, a warning is printed and the 
// program continue with ped[iPix][iCell]=0 , iPix=0..79, iCell=0..1023
//
// author: Marco Incagli
//
//====================================================================

void ReadCalibCoeff(string filename="NewCalibrationCoeff.txt"){
  ifstream ifile;
  string line;
  int dummy;

  ifile.open("NewCalibrationCoeff.txt");
  for (int iPixel=1;iPixel<=80;iPixel++){
    for (int iCell=0;iCell<1024;iCell++){
      ifile>>dummy>>Adc[iPixel-1][iCell][0];     
      for (int iLevel=0;iLevel<20;iLevel++){
	ifile>>Intercept[iPixel-1][iCell][iLevel]>>Slope[iPixel-1][iCell][iLevel]>>Adc[iPixel-1][iCell][iLevel+1];
      }
    }
    /*
    */
  }

}

int main(int argc, char *argv[] ) {

  if (argc!=2) {
    cout<<"Usage is:"<<argv[0]<<" <run_number>"<<endl;
    return 1;
  }
  int nRun = atoi(argv[1]);
  //open rootfile and booking TTree;
  //=============================================================
  UInt_t eventnum, TimeSec, TimeSubSec;
  float drs1a[10][1024];
  float drs2a[10][1024];
  float drs3a[10][1024];
  float drs4a[10][1024];
  float drs5a[10][1024];
  float drs6a[10][1024];
  float drs7a[10][1024];
  float drs8a[10][1024];
  float drs1v[10][1024];
  float drs2v[10][1024];
  float drs3v[10][1024];
  float drs4v[10][1024];
  float drs5v[10][1024];
  float drs6v[10][1024];
  float drs7v[10][1024];
  float drs8v[10][1024];

  TTree::SetMaxTreeSize(1000*Long64_t(2000000000));
  char rootfilename[40];
  sprintf(rootfilename,"DRSa_%06d.root",nRun);
  printf("input file name %s\n",rootfilename);
  TFile *inrootfile = new TFile(rootfilename,"read");
  if(!inrootfile){
     cerr<<"Input ROOT File Reading Error"<<endl;
     exit(1);
  }
  TTree *ta = (TTree*)inrootfile->Get("ta");
  ta->SetBranchAddress("eventnum",&eventnum);
  ta->SetBranchAddress("TimeSec",&TimeSec);
  ta->SetBranchAddress("TimeSubSec",&TimeSubSec);
  ta->SetBranchAddress("drs1a",drs1a);
  ta->SetBranchAddress("drs2a",drs2a);
  ta->SetBranchAddress("drs3a",drs3a);
  ta->SetBranchAddress("drs4a",drs4a);
  ta->SetBranchAddress("drs5a",drs5a);
  ta->SetBranchAddress("drs6a",drs6a);
  ta->SetBranchAddress("drs7a",drs7a);
  ta->SetBranchAddress("drs8a",drs8a);

  sprintf(rootfilename,"DRSv_%06d.root",nRun);
  printf("Output file name %s\n",rootfilename);
  TFile *outrootfile = new TFile(rootfilename,"new");
  if(!outrootfile){
     cerr<<"Output ROOT File Creating Error"<<endl;
     exit(1);
  }
  TTree *tv = new TTree("tv","mVvalue");
  tv->Branch("eventnum",&eventnum,"eventnum/i");
  tv->Branch("TimeSec",&TimeSec,"TimeSec/i");
  tv->Branch("TimeSubSec",&TimeSubSec,"TimeSubSec/i");
  tv->Branch("drs1v",drs1v,"drs1v[10][1024]/F");
  tv->Branch("drs2v",drs2v,"drs2v[10][1024]/F");
  tv->Branch("drs3v",drs3v,"drs3v[10][1024]/F");
  tv->Branch("drs4v",drs4v,"drs4v[10][1024]/F");
  tv->Branch("drs5v",drs5v,"drs5v[10][1024]/F");
  tv->Branch("drs6v",drs6v,"drs6v[10][1024]/F");
  tv->Branch("drs7v",drs7v,"drs7v[10][1024]/F");
  tv->Branch("drs8v",drs8v,"drs8v[10][1024]/F");

  int nEvts = ta->GetEntries();
  //nEvts=2;

  ReadCalibCoeff();
  for (int ievt=0;ievt<nEvts;ievt++){
    ta->GetEntry(ievt);
    for(int iChan=0;iChan<nChan;iChan++){
      for(int iCell=0;iCell<nCell;iCell++){

	bool Debug = iChan==0 && iCell<0;
	if (Debug) cout<<"ADC= "<<drs1a[iChan][iCell]<<endl;
	if (drs1a[iChan][iCell]<Adc[0*8+iChan][iCell][0]) {
	  drs1v[iChan][iCell] = 0;
	  if (Debug)
	    {
	      cout<<Adc[0*8+iChan][iCell][0]<<endl;
	      cout<<"mVolt= "<<drs1v[iChan][iCell]<<endl;
	    }
	}
	else{
	  for(int iLevel=0;iLevel<20;iLevel++){
	    if (drs1a[iChan][iCell]<Adc[0*8+iChan][iCell][iLevel+1]){
	      drs1v[iChan][iCell] = drs1a[iChan][iCell]*Slope[0*8+iChan][iCell][iLevel]+Intercept[0*8+iChan][iCell][iLevel];
	      if (Debug) 
		{
		  cout<<Adc[0*8+iChan][iCell][iLevel+1]<<" "<<Slope[0*8+iChan][iCell][iLevel]<<" "<<Intercept[0*8+iChan][iCell][iLevel]<<endl;
		  cout<<"mVolt= "<<drs1v[iChan][iCell]<<endl;
		}
	      break;
	    }
	  }
	}
	drs2v[iChan][iCell] = drs2a[iChan][iCell];
	drs3v[iChan][iCell] = drs3a[iChan][iCell];
	drs4v[iChan][iCell] = drs4a[iChan][iCell];
	drs5v[iChan][iCell] = drs5a[iChan][iCell];
	drs6v[iChan][iCell] = drs6a[iChan][iCell];
	drs7v[iChan][iCell] = drs7a[iChan][iCell];
	drs8v[iChan][iCell] = drs8a[iChan][iCell];
      }
    }
    tv->Fill();
  }
outrootfile->Write();
}


