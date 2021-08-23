#include<TCanvas.h>
#include <TPad.h>
#include <TVirtualPad.h>
#include <TChain.h>
#include <TSystem.h>
#include <TFile.h>
#include <TLatex.h>
#include <TMarker.h>
#include <TLine.h>
#include <TImageDump.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TF1.h>

#include <iostream>
#include <fstream>
#include "MagicTypes.h"
#include "MagicRawData.h"

using namespace std;

int main(int argc, char *argv[]) {
  char infile[FILENAME_MAX_SIZE];
  FILE *fptr;
  int  NumSamplesInChannel, nPix;
  Int_t nRun, Nevtstot;
  run_header_str    *RunHeader = NULL;
  event_header_str   EventHeader;
  channel_header_str ChannelHeader;
  U16 *ChannelData;
  

  if (argc < 3) {
    printf("Usage: %s <Neventslevel> <rawdata_file1> <rawdata_file2> <...>\n", argv[0]);
    exit(1);
  }

  Int_t Neventslevel = atoi(argv[1]);
  if (Neventslevel<0 || Neventslevel>1000) {
    cout<<"Limits on Neventslevel are 0-1000; current value is:"<<Neventslevel<<endl;
    return 1;
  }

  int iargc=2;

  //////////////////////////////////////////////
  // hardcoded numbers:
  //////////////////////////////////////////////
  // numbers of cells in a domino 
  static  const Int_t nSampPix = 1024;
  // max numbers of pixels (10xDomino) 
  static  const Int_t nMaxPix = 80;
  // numbers of calibration levels considered
  static  const Int_t Nlevels = 21; //14
  // name of the output file
  TString txtfilename = "./NewCalibrationCoeff.txt";
  ////////////////////////////////////////////
  // input DAC counts corresponding to each calibration level
  //  Double_t voltsArray[Nlevels] = {0., 100., 200., 300., 400., 500., 600., 1000., 1500., 2000., 2500., 3000., 3500., 4000., 4500., 5000., 6000., 7000., 8000., 9000.};//DAC
  //  Double_t voltsArray[Nlevels] = {150., 200., 250., 500., 1000., 1500., 2000., 2500., 3000., 3500., 4000., 5000., 6000., 7000., 9000.};//DAC
  //  Double_t voltsArray[Nlevels] = {0.,     150.,  200.,     250.,   400.,   600.,   1000.,   1500.,    2000.,   3000.,   3500.,   4000.,   5000.,    6000.,    7000.,    9000.};//DAC

  //  Double_t voltsArray[Nlevels] = {-5.e+1, 15.e+1, 25.e+1, 33.e+1, 60.e+1, 96.e+1, 168.e+1, 256.e+1, 345.e+1, 521.e+1, 608.e+1, 695.e+1, 866.e+1, 1033.e+1, 1189.e+1, 1375.e+1};//Volts

  //  Double_t voltsArray[Nlevels] = {0.,     100.,  200.,   300.,   400.,   500.,   600.,   700.,   800.,   900.,   1000.,   1100.,   1200.,   1300.}; //DAC
  //  Double_t voltsArray[Nlevels] = {0.,  100.,  200.,   300.,   400.,   500.,   600.,   700.,   800.,   900.,   1000.,   1100., 1200.,  1300., 1400., 1500., 1600.}; //DAC

  // measured on the DRS input (in mV)
  Double_t voltsArray[Nlevels] = {-125.,  -53.,   -18.,   16.,   61.,   87.,   158.,   230.,   301.,   372.,   443.,  513.,  585., 655., 726., 797., 868., 939., 1010., 1081., 1150.}; //DAC

  //  Double_t voltsArray[Nlevels] = {0.,     100.,  200.,   300.,   400.,   600.,   1000.,   1500.,   2000.,   3000.,   3500.,   4000.,   5000.,   6000.,    7000.,    8000.}; //DAC
  //  Double_t voltsArray[Nlevels] = {-5.e+1, 7.e+1, 25.e+1, 42.e+1, 60.e+1, 96.e+1, 168.e+1, 256.e+1, 345.e+1, 521.e+1, 608.e+1, 695.e+1, 866.e+1, 1033.e+1, 1189.e+1, 1336.e+1};//Volts
  ////////////////////////////////////////////

  // declare the fit graphs objects
  TGraphErrors *gcurve[nMaxPix][nSampPix];
  TH1D *hmean[nMaxPix][nSampPix];
  TH1D *hrms[nMaxPix][nSampPix];

  // and define them
  for (Int_t iPix = 0; iPix<nMaxPix; iPix++) {
    for (Int_t iCell = 0; iCell<nSampPix; iCell++) {   
      gcurve[iPix][iCell] = new TGraphErrors(Nlevels-1);      
      gcurve[iPix][iCell]->SetTitle(Form("Pixpos %d Cell %d calibration curve", iPix, iCell));
      gcurve[iPix][iCell]->SetName(Form("Pixpos_%d_Cell_%d_calibration_curve", iPix, iCell));
      gcurve[iPix][iCell]->SetMarkerStyle(kCircle);
      hmean[iPix][iCell] = new TH1D(Form("hist_mean_Pixel_%d_Cell_%d", iPix, iCell), Form("Pixpos %d Level %d histogram (mean + rms)", iPix, iCell), Nlevels, 0+(1/2.), Nlevels+(1/2.));
      hrms[iPix][iCell]  = new TH1D(Form("hist_rms_Pixel_%d_Cell_%d", iPix, iCell), Form("Pixpos %d Level %d histogram (rms)", iPix, iCell), Nlevels,0+(1/2.), Nlevels+(1/2.));
    }
  }
  // check consistency //////////////////////////////////////
  //if (Nevtstot/Nlevels != Neventslevel || Nevtstot%Neventslevel != 0) {
  //printf("ERROR the total numner of events is not multiple of the expected number of events per file.\n");
  //return 1;  
  //}

  while(iargc<argc){

    sprintf(infile, "%s", argv[iargc]);
    if ((fptr = fopen(infile, "r")) == NULL) {
      printf("Could not open file: %s\n",infile);
      return 1;
    }


    if (!(RunHeader = fReadRunHeader(fptr))) {
      printf("Cannot handle format version: %d\n", RunHeader->FormatVersion);
      exit(RunHeader->FormatVersion);
    }
    //PrintRunHeader(RunHeader, stdout);
 
    NumSamplesInChannel = RunHeader->NumPixInChannel * RunHeader->NumSamplesPerPixel;
    ChannelData = (U16 *)malloc(NumSamplesInChannel * NUM_BYTES_PER_SAMPLE);

    nRun = RunHeader->RunNumber;
    Nevtstot = RunHeader->NumEvents;
    cout<<"Run Number = "<<nRun<<endl;
    cout<<"Number of events = "<<Nevtstot<<endl;


    printf("Total number of levels considered (hard coded): %d\n", Nlevels);
    printf("Number of calibration points per cell: %d\n,",Neventslevel);

    //normally nPix=nMaxPix=80, but it could be less (e.g. <4 mezzanines)
    nPix = RunHeader->NumPixInChannel * RunHeader->NumChannels;

    //////////////////////////////////////////////
    //Define counters and initialize them for first file in the (eventual) loop
    Int_t iLevel,totEvts;
    if (iargc==2){
      iLevel = 0;
      totEvts = 0;
    }
    // main loop over events /////////////////////
    for (Int_t ievt = 1; ievt<=Nevtstot; ievt++) {
      totEvts++;
      if ((totEvts-1)%Neventslevel == 0) { 
	printf("Processing level %d over %d...\n", (totEvts-1)/Neventslevel+1, Nlevels);
	// increment the counter if a new level
	iLevel++;
	if (iLevel>Nlevels){
	  cout<<"Too many events in file or more entries per level than expected"<<endl;
	  break;
	}
      }

      if(fReadEventHeader(&EventHeader, fptr) == EVENT_HEADER_SIZE)
	//loop on channels (8 channels (=DRS) per pulsar, 2 each mezzanine)
	for (UInt_t iChannel=0; iChannel<RunHeader->NumChannels; iChannel++) {
	  fReadChannelHeader(&ChannelHeader, fptr);
	  fread(ChannelData, NumSamplesInChannel, NUM_BYTES_PER_SAMPLE, fptr);
	  for (UInt_t i=0; i<RunHeader->NumPixInChannel; i++) {
	    int iPix = iChannel*RunHeader->NumPixInChannel + i;
	    for (UInt_t j=0; j<nSampPix; j++) {

	      Int_t sampcontent =  ChannelData[i*RunHeader->NumSamplesPerPixel+j];
	      // fill the histogram

	      hmean[iPix][j]->AddBinContent(iLevel,sampcontent);
	      hrms[iPix][j]->AddBinContent(iLevel,TMath::Power(sampcontent,2));
	      //	      printf("hist content mean %d   rms %d \n", hmean[index]->GetBinContent(iSample), hrms[index]->GetBinContent(iSample));
	    }
	  }
	}
 
    } // END event loop 
    fclose(fptr);
    iargc++;
  }//END loop on input files

  //////////////////////////////////////////////////////
  // build the calibration curves
  for (Int_t iPix=0; iPix<nPix; iPix++) {
    for (Int_t iCell=0; iCell<nSampPix; iCell++) {   
      hmean[iPix][iCell]->Scale(1./Neventslevel);
      for (Int_t iLevel=1;iLevel<=Nlevels;iLevel++){
	Double_t dummy = hrms[iPix][iCell]->GetBinContent(iLevel);
	float binErr = dummy/Neventslevel - TMath::Power(hmean[iPix][iCell]->GetBinContent(iLevel),2);
	if (binErr>0) binErr = TMath::Sqrt(binErr);
	else {
	  cout<<" ERROR in evaluation of binErr = "<<binErr<<endl;
	  return 1;
	}
	hrms[iPix][iCell]->SetBinContent(iLevel,binErr);
	hmean[iPix][iCell]->SetBinError(iLevel, 1);
	//      printf("sample %4d mean %.4f rms %.4f \n", iSample, hmean[offset+iPix-1]->GetBinContent(iSample), hrms[offset+iPix-1]->GetBinContent(iSample));
	
	// build DRS calibration curves
	gcurve[iPix][iCell]->SetPoint(iLevel-1, hmean[iPix][iCell]->GetBinContent(iLevel), voltsArray[iLevel-1]);
	gcurve[iPix][iCell]->SetPointError(iLevel-1, hrms[iPix][iCell]->GetBinContent(iLevel), 0.);
      }
    }
  }

  ///////////////////////////////
  // compute and save all the fit parameters in a txt file

  const char *txtfile = txtfilename;
  FILE *ftxt;
  ftxt = fopen(txtfile, "w");

  for (Int_t iPix=0; iPix<nPix; iPix++) {

    //Int_t pixid = PixId[iPix];
    printf("Writing coeff. of pixel %d\n", iPix+1);
    //fprintf(ftxt, "\nPixel %d\n",iPix+1);
    for (Int_t iCell = 0; iCell<nSampPix; iCell++) {
      Double_t ics1;
      Double_t ips1;
      Double_t ics2;
      Double_t ips2;

      fprintf(ftxt, "%d ", iCell+1); // capacitor number

      for (Int_t iLevel = 1; iLevel<Nlevels; iLevel++) {
	gcurve[iPix][iCell]->GetPoint(iLevel-1, ics1, ips1);
	gcurve[iPix][iCell]->GetPoint(iLevel-1+1, ics2, ips2);
	fprintf(ftxt, "%5.4e ", ics1); // x coordinate
	if (ics2-ics1 > 0) {
	  Float_t m = (ips2-ips1)/(ics2-ics1);
	  Float_t q = ips1-(ics1*m);
	  fprintf(ftxt, "%5.4e ", q); // b par
	  fprintf(ftxt, "%5.4e ", m); // a par
	}
	else {
	  Float_t m = 1.0;
	  Float_t q = 0.;
	  fprintf(ftxt, "%5.4e ", q); // b par
	  fprintf(ftxt, "%5.4e ", m); // a par
	}
      }

      fprintf(ftxt, "%5.4e ", ics2); // x coordinate
      fprintf(ftxt, "\n"); // newline
    }
  }

  // close the text file
  fclose(ftxt);

  ////////////////////////////////////////
  // stores the histograms in a root file

  char outname[40];
  sprintf(outname,"Calibrate_DRS_%06d.root",nRun);//Data: string->char
  printf("file name %s\n",outname);
  TFile* hfile = new TFile(outname, "recreate");

  // save the mean and rms histograms
  for (Int_t iPix = 0; iPix<nPix; iPix++) {
    for (Int_t iCell = 0; iCell<nSampPix; iCell++) {   
      gcurve[iPix][iCell]->Write();
      hmean[iPix][iCell]->Write();
      hrms[iPix][iCell]->Write();
    }
  }
  // then write the file
  hfile->Write();

  return 0;
}
