#include <stdio.h>
#include <stdlib.h>
#include <TTree.h>
#include <TBranch.h>
#include <TFile.h>  
#include <TH1.h> 
#include <TMath.h>

TTree *tree;
/* Global Variables for NTUPLE (not all used in the ntuple at the moment) */

int Nrunnumber;           // Number of the Run  
unsigned int Nevtda = 0 ;             // Number of data event

///////////////////// SCALER
#define SCALER_ADDR 0x00200003

unsigned int NSCA;              // Total number of counts in Scaler 
unsigned int CHSCA[16];         // Channels in the scaler (the channel of the count) 
unsigned int  COUNTSCA[16];     //  Counts in the Scaler  (the value of the count)

///////////////////// OSC

#define MAX_OSC_SAMPLES 532
#define MAX_OSC_CH 4

#define OSC_ADDR 0x0000fafa
unsigned int NhitOSC;           // Total number of points in the Oscilloscope
                                // (=2128 if there are 4 channels each with 282 points )
unsigned int CHOSC[MAX_OSC_SAMPLES*MAX_OSC_CH];       // Channels in the oscilloscope
                                // CHOSC[i] is the channel of the i-th point )
Char_t DATAOSC[MAX_OSC_SAMPLES*MAX_OSC_CH];              // Values of the poit in the oscilloscope
Char_t DATAOSC_CH[MAX_OSC_CH][MAX_OSC_SAMPLES];              // Values of the point in the oscilloscope

// (DATAOSC[i] is the value of the i-th point)
unsigned int NUMOSC[MAX_OSC_SAMPLES*MAX_OSC_CH];      // Number of the point in the channel (max value =282)

int POSOSC[MAX_OSC_CH];                  // Position for Oscilloscope (as in myOscDataFile.h)
unsigned int TDOSC[MAX_OSC_CH];          // Time Diff for Oscilloscope (as in myOscDataFile.h)
unsigned int SCALEOSC[MAX_OSC_CH];       // Scale of the Oscilloscope (as in myOscDataFile.h)
unsigned int CHFOSC[MAX_OSC_CH];         // Oscilloscope Channel Flag (0= channel enabled, 1 =channel abled)
unsigned int PTSOSC;            // Oscilloscope pts (as in myOscDataFile.h , at the moment =282)
unsigned int SROSC;             // Sampling rate of the Oscilloscope (as in myOscDataFile.h)
float PED_MEAN_OSC[MAX_OSC_CH];
float PED_RMS_OSC[MAX_OSC_CH];

///////////////////// ADC0
#define ADCN0_ADDR 0x04000005

unsigned int NhitADCN0;         // Hits in ADCN0 (the total number of couts in ADCNO, MODULE 0 of V792AC)  
unsigned int CHADCN0[32];       // Channel in ADCN0 (the channel of cout)
                                // (CHADCN0[i] is the channel of the i-th count )
unsigned int CHARGEADCN0[32] ;  // Charge in ADCN0  (the value of the count)
                                // (CHARGEADCN0[i] is the  value of the i-th cout )
UChar_t OVERADCN0[32] ; 
UChar_t UNDERADCN0[32] ; 

float   PED_MEAN_ADCN0[32];
float   PED_RMS_ADCN0[32];

///////////////////// ADC1
#define ADCN1_ADDR 0x05000005

unsigned int NhitADCN1;         // Hits in ADCN1 (the total number of couts in ADCNO, MODULE1  of V792AC)
unsigned int CHADCN1[32];       // Channel in ADCN1 (the channel of the count)
                                // (CHADCN1[i] is the channel of the i-th count )
unsigned int CHARGEADCN1[32] ;  // Charge in ADCN1  (the value of the count)
                                // (CHARGEADCN1[i] is the  value of the i-th cout )

UChar_t OVERADCN1[32] ; 
UChar_t UNDERADCN1[32] ; 
 
float   PED_MEAN_ADCN1[32];
float   PED_RMS_ADCN1[32];

///////////////////// ADC2
#define ADCN2_ADDR 0x06000005

unsigned int NhitADCN2;         // Hits in ADCN2 (the total number of couts in ADCNO, MODULE1  of V792AC)
unsigned int CHADCN2[32];       // Channel in ADCN2 (the channel of the count)
                                // (CHADCN2[i] is the channel of the i-th count )
unsigned int CHARGEADCN2[32] ;  // Charge in ADCN2  (the value of the count)
                                // (CHARGEADCN2[i] is the  value of the i-th cout )

UChar_t OVERADCN2[32] ;
UChar_t UNDERADCN2[32] ;

float   PED_MEAN_ADCN2[32];
float   PED_RMS_ADCN2[32];

///////////////////// ADC3
#define ADCN3_ADDR 0x07000025

unsigned int NhitADCN3;         // Hits in ADCN3 (the total number of couts in ADCNO, MODULE1  of V792AC)
unsigned int CHADCN3[32];       // Channel in ADCN3 (the channel of the count)
                                // (CHADCN3[i] is the channel of the i-th count )
unsigned int CHARGEADCN3[32] ;  // Charge in ADCN3  (the value of the count)
                                // (CHARGEADCN3[i] is the  value of the i-th cout )

UChar_t OVERADCN3[32] ;
UChar_t UNDERADCN3[32] ;

float   PED_MEAN_ADCN3[32];
float   PED_RMS_ADCN3[32];


/************************************************************************/
void ntbook(bool hasOSC,bool hasDRS)
/************************************************************************/
{
  //  TTree::SetMaxTreeSize(1000*Long64_t(2000000000));
  // Declare ntuple identifier and title
  tree = new TTree("DREAM","Dream TB Data");

                                                                                                 
  // Describe data to be stored in the Ntuple (see comments before)
 
  tree->Branch("Nrunnumber",&Nrunnumber,"Nrunnumber/i");
  tree->Branch("Nevtda",&Nevtda,"Nevtda/i");

  if(hasOSC){
    tree->Branch("DATAOSC",DATAOSC,(TString)Form("DATAOSC[%d]/B",MAX_OSC_SAMPLES*MAX_OSC_CH));
    tree->Branch("DATAOSC_CH",DATAOSC_CH,Form("DATAOSC[%d][%d]/B",MAX_OSC_CH,MAX_OSC_SAMPLES));
    tree->Branch("CHFOSC",CHFOSC,Form("CHFOSC[%d]/i",MAX_OSC_CH));
    tree->Branch("POSOSC",POSOSC,Form("POSOSC[%d]/I",MAX_OSC_CH));
    tree->Branch("SCALEOSC",SCALEOSC,Form("SCALEOSC[%d]/i",MAX_OSC_CH));
    tree->Branch("TDOSC",TDOSC,Form("TDOSC[%d]/i",MAX_OSC_CH));
    tree->Branch("PTSOSC",&PTSOSC,"PTSOSC/i");
    tree->Branch("SROSC",&SROSC,"SROSC/i");
    tree->Branch("PED_MEAN_OSC",PED_MEAN_OSC,Form("PED_MEAN_OSC[%d]/F",MAX_OSC_CH));
    tree->Branch("PED_RMS_OSC",PED_RMS_OSC,Form("PED_RMS_OSC[%d]/F",MAX_OSC_CH));
  }

  tree->Branch("CHADCN0",CHADCN0,"CHADCN0[32]/i");
  tree->Branch("CHARGEADCN0",CHARGEADCN0,"CHARGEADCN0[32]/i");
  tree->Branch("OVERADCN0",OVERADCN0,"OVERADCN0/b");
  tree->Branch("UNDERADCN0",UNDERADCN0,"UNDERADCN0/b");
  
  tree->Branch("CHADCN1",CHADCN1,"CHADCN1[32]/i");
  tree->Branch("CHARGEADCN1",CHARGEADCN1,"CHARGEADCN1[32]/i");
  tree->Branch("OVERADCN1",OVERADCN1,"OVERADCN1/b");
  tree->Branch("UNDERADCN1",UNDERADCN1,"UNDERADCN1/b");

  tree->Branch("CHADCN2",CHADCN2,"CHADCN2[32]/i");
  tree->Branch("CHARGEADCN2",CHARGEADCN2,"CHARGEADCN2[32]/i");
  tree->Branch("OVERADCN2",OVERADCN2,"OVERADCN2/b");
  tree->Branch("UNDERADCN2",UNDERADCN2,"UNDERADCN2/b");

  tree->Branch("CHADCN3",CHADCN3,"CHADCN3[32]/i");
  tree->Branch("CHARGEADCN3",CHARGEADCN3,"CHARGEADCN3[32]/i");
  tree->Branch("OVERADCN3",OVERADCN3,"OVERADCN3/b");
  tree->Branch("UNDERADCN3",UNDERADCN3,"UNDERADCN3/b");
}

void fill_pedestals(char* pedntfilename,bool hasOSC,bool hasDRS){}



/************************************************************************/
int main(int argc, char **argv)
/************************************************************************/
{
  bool hasOSC = false,hasDRS = false;

hasOSC = true;

  // Book Ntuple
  ntbook(hasOSC, hasDRS);
  TFile *file = new TFile("test.root","RECREATE");
  if (!file->IsOpen()) {
    printf("myDaq2Ntu: cannot open ntuple file %s\n", file->GetName());
    return -1;
  }
  //file->SetCompressionLevel(1);


  // Main Loop on all events
  for (int count =0;count < 30000;count++) {
    Nrunnumber=9999;
    Nevtda   = count+1;   
    tree->Fill();
  } // End of loop 

  file->cd();
  tree->Write("",TObject::kWriteDelete);
  printf("output file: %s\n",file->GetName());
  file->Close();

  return 0;

}
