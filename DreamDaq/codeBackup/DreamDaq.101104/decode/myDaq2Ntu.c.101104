/*************************************************************************

        myDaq2Ntu.c
        -----------

        myDaq --> Ntuple conversion program

        Version 0.1,      A.Cardini C. Deplano 29/11/2001

                          E. Meoni L. La Rotonda 25/07/2006

			  M. Cascella 2010
*********************************************************************/

#include "../myRawFile.h"
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <TTree.h>
#include <TFile.h>
#include <iostream>     
#include <TH1.h> 
#include <TMath.h>

#define TOTFADC 0
#define TOTADC  0
#define TOTSCA  1
#define TOTTDC  1
#define TOTKLOETDC 1
#define TOTTH03  1
#define TOTADCN 3

TTree *tree;
/* Global Variables for NTUPLE (not all used in the ntuple at the moment) */
bool phys;

int Nrunnumber;           // Number of the Run  
int BegTimeEvs;             // Time of first event in Seconds
int BegTimeEvu;             // Time of first event in MicroSeconds
int TimeEvs;                // Time of event in Seconds
int TimeEvu;                // Time of event in MicroSeconds
unsigned int Nevtda =0 ;             // Number of data event

///////////////////// SCALER
#define SCALER_ADDR 0x00200003

unsigned int NSCA;              // Total number of counts in Scaler 
unsigned int CHSCA[16];         // Channels in the scaler (the channel of the count) 
unsigned int  COUNTSCA[16];     //  Counts in the Scaler  (the value of the count)

///////////////////// OSC
#define OSC_ADDR 0x0000fafa
unsigned int NhitOSC;           // Total number of points in the Oscilloscope 
                                // (=2128 if there are 4 channels each with 282 points )
unsigned int CHOSC[2128];       // Channels in the oscilloscope
                                // CHOSC[i] is the channel of the i-th point )  
Char_t DATAOSC[2128];              // Values of the poit in the oscilloscope
// (DATAOSC[i] is the value of the i-th point)
unsigned int NUMOSC[2128];      // Number of the point in the channel (max value =282)
                                
int POSOSC[4];                  // Position for Oscilloscope (as in myOscDataFile.h) 
unsigned int TDOSC[4];          // Time Diff for Oscilloscope (as in myOscDataFile.h)
unsigned int SCALEOSC[4];       // Scale of the Oscilloscope (as in myOscDataFile.h)  
unsigned int CHFOSC[4];         // Oscilloscope Channel Flag (0= channel enabled, 1 =channel abled)  
unsigned int PTSOSC;            // Oscilloscope pts (as in myOscDataFile.h , at the moment =282)  
unsigned int SROSC;             // Sampling rate of the Oscilloscope (as in myOscDataFile.h)
float PED_MEAN_OSC[4];
float PED_RMS_OSC[4];

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
#define ADCN3_ADDR 0x07000005

unsigned int NhitADCN3;         // Hits in ADCN3 (the total number of couts in ADCNO, MODULE1  of V792AC)
unsigned int CHADCN3[32];       // Channel in ADCN3 (the channel of the count)
                                // (CHADCN3[i] is the channel of the i-th count )
unsigned int CHARGEADCN3[32] ;  // Charge in ADCN3  (the value of the count)
                                // (CHARGEADCN3[i] is the  value of the i-th cout )

UChar_t OVERADCN3[32] ;
UChar_t UNDERADCN3[32] ;

float   PED_MEAN_ADCN3[32];
float   PED_RMS_ADCN3[32];

///////////////////// TDC
#define TDC_ADDR 0x08000024
                                                                                                               
unsigned int NhitTDC;           // Hits in TDC  (the total number of couts in TDC, MODULE 1176 )
                                // Remember TDC have 16 channel multihit 
                                //(at the moment used only 8 channels)  
unsigned int CHTDC[64];         // Channel in TDC (the channel of the count)
                                // (CHTDC[i] is the channel of the i-th count)
unsigned int VALIDTDC[64];      //  0=good 1=corrupt
unsigned int EDGETDC[64];       // Edge in TDC (0=falling 1=rising)
unsigned int COUNTTDC[64];      // Count in TDC (the value of the count)
                                //(COUNTTDC[i] is the  value of the i-th cout) 
float X_UP[16];
float Y_UP[16];                                                                        
float X_DW[16];
float Y_DW[16];

unsigned int N_X_UP;
unsigned int N_Y_UP;
unsigned int N_X_DW;
unsigned int N_Y_DW;



/************************************************************************/
void ntbook()
/************************************************************************/
{                                                                                                                             
  //  TTree::SetMaxTreeSize(1000*Long64_t(2000000000));
  // Declare ntuple identifier and title
  tree = new TTree("DREAM","Title Dream 2008");

                                                                                                 
  // Describe data to be stored in the Ntuple (see comments before) 
 
  tree->Branch("Nrunnumber",&Nrunnumber,"Nrunnumber/i");
  tree->Branch("TimeEvs",&TimeEvs,"TimeEvs/i");
  tree->Branch("TimeEvu",&TimeEvu,"TimeEvu/i");
  tree->Branch("Nevtda",&Nevtda,"Nevtda/i");

  tree->Branch("DATAOSC",DATAOSC,"DATAOSC[2128]/B");
  tree->Branch("CHFOSC",CHFOSC,"CHFOSC[4]/i");
  tree->Branch("POSOSC",POSOSC,"POSOSC[4]/I");
  tree->Branch("SCALEOSC",SCALEOSC,"SCALEOSC[4]/i");
  tree->Branch("TDOSC",TDOSC,"TDOSC[4]/i");
  tree->Branch("PTSOSC",&PTSOSC,"PTSOSC/i");
  tree->Branch("SROSC",&SROSC,"SROSC/i");

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

  tree->Branch("NhitTDC",&NhitTDC,"NhitTDC/i"); 
  tree->Branch("CHTDC",CHTDC,"CHTDC[64]/i");
  tree->Branch("COUNTTDC",COUNTTDC,"COUNTTDC[64]/i");
  tree->Branch("VALIDTDC",VALIDTDC,"VALIDTDC[64]/i");
  tree->Branch("EDGETDC",EDGETDC,"EDGETDC[64]/i");
  
  tree->Branch("X",X_DW,"X[16]/F");
  tree->Branch("N_X",&N_X_DW,"N_X/i");
  tree->Branch("Y",Y_DW,"Y[16]/F");
  tree->Branch("N_Y",&N_Y_DW,"N_Y/i");

  tree->Branch("CHSCA",CHSCA,"CHSCA[16]/i");
  tree->Branch("COUNTSCA",COUNTSCA,"COUNTSCA[16]/i");

  if (phys) {
    tree->Branch("PED_MEAN_ADCN0",PED_MEAN_ADCN0,"PED_MEAN_ADCN0[32]/F");
    tree->Branch("PED_RMS_ADCN0",PED_RMS_ADCN0,"PED_RMS_ADCN0[32]/F");
    
    tree->Branch("PED_MEAN_ADCN1",PED_MEAN_ADCN1,"PED_MEAN_ADCN1[32]/F");
    tree->Branch("PED_RMS_ADCN1",PED_RMS_ADCN1,"PED_RMS_ADCN1[32]/F");
    
    tree->Branch("PED_MEAN_ADCN2",PED_MEAN_ADCN2,"PED_MEAN_ADCN2[32]/F");
    tree->Branch("PED_RMS_ADCN2",PED_RMS_ADCN2,"PED_RMS_ADCN2[32]/F");

    tree->Branch("PED_MEAN_ADCN3",PED_MEAN_ADCN3,"PED_MEAN_ADCN3[32]/F");
    tree->Branch("PED_RMS_ADCN3",PED_RMS_ADCN3,"PED_RMS_ADCN3[32]/F");
    
    tree->Branch("PED_MEAN_OSC",PED_MEAN_OSC,"PED_MEAN_OSC[4]/F");
    tree->Branch("PED_RMS_OSC",PED_RMS_OSC,"PED_RMS_OSC[4]/F");
  } 
}


/************************************************************************/
int main(int argc, char **argv)
/************************************************************************/
{
  int rc;
  unsigned int j;
  int hits;
  unsigned int buf[10000];
  unsigned int *addr;
  mySCA scaData;
  myADCN adcnData;
  myTEKOSC tekoscData;
  myTDC tdcData;


  char datadir[256];
  char datafileprefix[256];
  char datafilename[256];
  char ntdir[256];
  char ntfilename[256];
  int isFirstEvent = 1;
  //  int istat;
  int carry;

  if(argc<3){
    std::cout << "Usage: " << argv[0] << " runnumber datatype" 
	      << " (pedestal or data)" << std::endl;
    std::cout << "PLEASE set correctly the following environmental variables"<< std::endl;
   
    std::cout << "Input binary files are searched in:"<< std::endl;
    std::cout << "\t $DATADIR/data"<< std::endl;
    std::cout << "\t $DATADIR/pedestal"<< std::endl;
    std::cout << "Input root files for pedestal math are searched in:"<< std::endl;
    std::cout << "\t $NTUPLEDIR/"<< std::endl;
    std::cout << "Output files will be placed in:"<< std::endl;
    std::cout << "\t $NTUPLEDIR/"<< std::endl;
    exit(1);
  }
  
  if(!strcmp(argv[2],"pedestal")){
    phys=false;
  }else if(!strcmp(argv[2],"data")){
    phys=true;
  }else{
    std::cout << "Unknown data type: " << argv[2] << std::endl;
    exit(1);
  }

  // Create Input/Output filenames
  if (getenv("DATADIR") == NULL){
      sprintf(datadir, "/home/dreamtest/working");
  }else
    sprintf(datadir, "%s", getenv("DATADIR"));
  
  if (getenv("NTUPLEDIR") == NULL)
    sprintf(ntdir, "/home/dreamtest/working/ntuple");
  else
    sprintf(ntdir, "%s", getenv("NTUPLEDIR"));
   
  
 
  if(phys){
    sprintf(datafileprefix, "%s/data/datafile_run%s", datadir, argv[1]);
    sprintf(datafilename, "%s/data/datafile_run%s_0.dat", datadir, argv[1]);
    sprintf(ntfilename, "%s/datafile_ntup_run%s.root", ntdir, argv[1]);
  }else{
    sprintf(datafileprefix, "%s/pedestal/pedestal_run%s", datadir, argv[1]);
    sprintf(datafilename, "%s/pedestal/pedestal_run%s_0.dat", datadir, argv[1]);
    sprintf(ntfilename, "%s/pedestal_ntup_run%s.root", ntdir, argv[1]);
  }
  int filecount = 0;

  //

  if(phys){
    const int n_adc = 4;

    // Read pedestal information for the different modules from Pedestal Ntuple
    char pedntfilename[256],channel[50],charge_modul[50];
    float mean_adcn[n_adc][32],rms_adcn[n_adc][32],mean_adc[8],rms_adc[8],mean_osc[4],rms_osc[4];

    for (unsigned int i=0;i<8;i++){
      mean_adc[i]=0;
      rms_adc[i]=0;
    }
    for (unsigned int i=0;i<4;i++) {
      mean_osc[i]=0;
      rms_osc[i]=0;
    } 
    for (unsigned int i=0;i<n_adc;i++) {
      for (unsigned int j=0;j<32;j++){
	mean_adcn[i][j]=0;
	rms_adcn[i][j]=0;
      }
    }
    // DRS 2008    if (!drs) {  
    sprintf(pedntfilename, "%s/pedestal_ntup_run%s.root", ntdir, argv[1]);
     
    TFile *fileped = new TFile(pedntfilename,"READ");
    if (!(fileped->IsOpen())){
      printf("myDaq2Ntu: cannot open pedestal ntuple file %s\n",pedntfilename);
    }//file open
    if(fileped->IsOpen()) {
      //      if (!drs) {
      TTree *t=(TTree *)gDirectory->Get("DREAM");
      
      if(t!=NULL && t->GetEntries()>0){
	
	TH1F *ht=new TH1F("ht","ADC",4096,-0.5,4095.5);

	for (int j=0;j<n_adc;j++){
	  for (int i=0;i<32;i++) {
	    sprintf(charge_modul,"CHARGEADCN%d",j);
	    sprintf(channel,"CHADCN%d==%d",j,i);
	    if(t->Project("ht",charge_modul,channel))  {
	      mean_adcn[j][i]= ht->GetMean();
	      rms_adcn[j][i] = ht->GetRMS();
	    }else{
	      mean_adcn[j][i]=0;
	      rms_adcn[j][i]=0;
	    }
	    ht->Reset();
	  }
	}
	
	for (int i=0;i<32;i++) {
	  PED_MEAN_ADCN0[i]=mean_adcn[0][i];
	  PED_RMS_ADCN0[i]=rms_adcn[0][i];
	  PED_MEAN_ADCN1[i]=mean_adcn[1][i];
	  PED_RMS_ADCN1[i]=rms_adcn[1][i];
	  PED_MEAN_ADCN2[i]=mean_adcn[2][i];
	  PED_RMS_ADCN2[i]=rms_adcn[2][i];
	  PED_MEAN_ADCN3[i]=mean_adcn[3][i];
	  PED_RMS_ADCN3[i]=rms_adcn[3][i];

	}
	delete ht;

	UInt_t ptsosc,chfosc[4];
	Char_t dataosc[2128];
	t->SetBranchAddress("PTSOSC",&ptsosc);
	t->SetBranchAddress("CHFOSC",chfosc);
	t->SetBranchAddress("DATAOSC",dataosc);
	t->GetEntry(0);
	
	unsigned int k[4];
	for (int i=0;i<4;i++) {
	  if (chfosc[i]==1)
	    k[i]=i;
	  else
	    k[i]=0;
	}
	
	char histo[20],name[20];
	TH1F *htosc[4];
	for(unsigned int j=0;j<4;j++) {
	  if (chfosc[j]==1){
	    sprintf(histo,"ht%d",j);
	    sprintf(name,"CHFOSC%d",j);
	    htosc[j]=new TH1F(histo,name,256,-128.5,127.5);
	  }
	} 
	
	for (unsigned int j=0;j<t->GetEntriesFast();j++){
	  t->GetEntry(j);
	  for (int i=0;i<4;i++) {
	    /*               std::cout << "entry " << j << " channel " << i */
	    /*                         << " chfosc " <<  chfosc[i]  */
	    /*                         << " k[i] " << k[i]  */
	    /*                         << " ptsosc " << ptsosc */
	    /*                         << " dataosc[(k[i]*ptsosc)+0] " << dataosc[(k[i]*ptsosc)+0]  */
	    /*                         << "\n"; */
	    if (chfosc[i]==1) {
	      for (unsigned l=0;l<ptsosc;l++) {
		htosc[i]->Fill(dataosc[(k[i]*ptsosc)+l]);
	      } 
	    } 
	  }
	}
	
	for (int i=0;i<4;i++){
	  if (chfosc[i]==1){
	    PED_MEAN_OSC[i]=htosc[i]->GetMean();
	    PED_RMS_OSC[i]=htosc[i]->GetRMS();
	    delete htosc[i];
	  }else{
	    mean_osc[i]=0;
	    rms_osc[i]=0;
	    PED_MEAN_OSC[i]=0;
	    PED_RMS_OSC[i]=0;

	  }
	}
      }//entries>0
      fileped->Close(); 
    }//file open
  }//if phys
 
   // Open Data file
  rc = RawFileOpen(datafilename);
  if (rc) {
    printf("myDaq2Ntu: cannot open data file %s\n", argv[1]);
    return -1;
  }
 
  TFile *file = new TFile(ntfilename,"RECREATE");

  printf("output file: %s\n",file->GetName());

  if (!file->IsOpen()) {
    printf("myDaq2Ntu: cannot open ntuple file %s\n", ntfilename);
    return -1;
  }
  file->SetCompressionLevel(4);
  
  //
  // Book Ntuple
  ntbook();
  // Main Loop on all events
  for (;;) {

    // Read Event and write it in buffer 'buf'
    rc = RawFileReadEventData(buf);
    if (rc == RAWDATAEOF) { 
      printf("Found EOF at event %d\n", GetEventNumber()); 
    } else if (rc == RAWDATAUNEXPECTEDEOF) { 
      printf("Unexpected EOF at event %d\n", GetEventNumber()); 
    }
    if (rc == RAWDATAEOF | rc == RAWDATAUNEXPECTEDEOF ) {
      RawFileClose(); 
      filecount++;
      sprintf(datafilename, "%s_%d.dat", datafileprefix,filecount);
      
      rc = RawFileOpen(datafilename);
      if(rc==-1){
	break;
      }
    }
    rc = 0;  // 
    Nrunnumber=GetRunNumber();
    //    int Nevttot=GetTotEvts();
    // Write in the Ntuple  the Event Number and Time (seconds and uSec) w.r.t first event.
    if (isFirstEvent) {
      BegTimeEvs = GetEventTimes();
      BegTimeEvu = GetEventTimeu();
      isFirstEvent =0;
    }
    Nevtda   = GetEventNumber();
    //    std::cout<<" Nevt "<<Nevtda<<std::endl;
    TimeEvu  = GetEventTimeu() - BegTimeEvu;
    if (TimeEvu<0.) {
      double TimeEvu_t= 1e6 + GetEventTimeu() - BegTimeEvu; 
      TimeEvu = (int) TimeEvu_t; 
      carry = 1; 
    }
    else 
      carry = 0;
    
    TimeEvs  = GetEventTimes() - BegTimeEvs - carry;


    //    printf("while fillng time info: Event %d TimeEvs %d TimeEvu %d\n", Nevtda, TimeEvs, TimeEvu);


    //  Decode module and compute the variables of the NTUPLE
    //

    // FILL EVENTNUMER



    // FILL SCALER 260
    for (int i=0;i<16;i++) {
      CHSCA[i] = 100;
    }

    hits = 0;
    addr = SubEventSeek(SCALER_ADDR, buf);
    rc += DecodeV260(addr, &scaData);
    for (j=0; j<scaData.index; j++) {
      int ch = scaData.channel[j];
      CHSCA[ch] = scaData.channel[j];
      COUNTSCA[ch] = scaData.counts[j];
      hits++;
    }
    NSCA = hits;

    // FILL ADC NEW 0
    for (int i=0;i<32;i++){     
      CHADCN0[i] = 100;
      CHADCN1[i] = 100;
      CHADCN2[i] = 100; 
      CHADCN3[i] = 100; 
    }

    // FILL ADC NEW 0
    hits = 0;
    addr =  SubEventSeek(ADCN0_ADDR, buf);
    rc += DecodeV792AC(addr, &adcnData);
    
    for (j=0; j<adcnData.index; j++) {
      int ch = adcnData.channel[j];
      CHADCN0[ch]  = adcnData.channel[j];
      CHARGEADCN0[ch] = adcnData.data[j];
      OVERADCN0[ch] = adcnData.ov[j];
      UNDERADCN0[ch] = adcnData.un[j];
      hits++;
    }
    NhitADCN0 = hits;
    
    // FILL ADC NEW 1
    hits = 0;
    addr =  SubEventSeek(ADCN1_ADDR, buf);
    rc += DecodeV792AC(addr, &adcnData);
    
    for (j=0; j<adcnData.index; j++) {
      int ch = adcnData.channel[j];
      CHADCN1[ch]  = adcnData.channel[j];
      CHARGEADCN1[ch] = adcnData.data[j];
      OVERADCN1[ch] = adcnData.ov[j];
      UNDERADCN1[ch] = adcnData.un[j];
      hits++;
    }
    NhitADCN1 = hits;                            
              
    // FILL ADC NEW 2                                                                        
    hits = 0;
    addr =  SubEventSeek(ADCN2_ADDR, buf);
    rc += DecodeV792AC(addr, &adcnData);

    for (j=0; j<adcnData.index; j++) {
      int ch = adcnData.channel[j];
      CHADCN2[ch]  = adcnData.channel[j];
      CHARGEADCN2[ch] = adcnData.data[j];
      OVERADCN2[ch] = adcnData.ov[j];
      UNDERADCN2[ch] = adcnData.un[j];
      hits++;
    }
    NhitADCN2 = hits;

    
    // FILL ADC NEW 3
    hits = 0;
    addr =  SubEventSeek(ADCN3_ADDR, buf);
    rc += DecodeV792AC(addr, &adcnData);
    
    for (j=0; j<adcnData.index; j++) {
      int ch = adcnData.channel[j];
      CHADCN3[ch]  = adcnData.channel[j];
      CHARGEADCN3[ch] = adcnData.data[j];
      OVERADCN3[ch] = adcnData.ov[j];
      UNDERADCN3[ch] = adcnData.un[j];
      hits++;
    }
    NhitADCN3 = hits; 

    // FILL TEKOSC
    hits = 0;
    addr =  SubEventSeek(OSC_ADDR, buf);
    rc += DecodeTEKOSC(addr, &tekoscData);
    unsigned int kp;
    for (j=0; j<tekoscData.index; j++) {
      int ch = tekoscData.channel[j];
      CHOSC[ch]  = tekoscData.channel[j];
      DATAOSC[ch] = (Char_t)(tekoscData.data[j]/256);
      NUMOSC[ch] =tekoscData.num[j];
      
      hits++;
    }
    NhitOSC = hits;
    
    for (kp=0;kp<4;kp++) {
      SCALEOSC[kp]=tekoscData.scale[kp];
      POSOSC[kp]=tekoscData.position[kp];
      TDOSC[kp]=tekoscData.tdiff[kp];
      CHFOSC[kp]=tekoscData.chfla[kp];
    }
    PTSOSC=tekoscData.pts;
    SROSC=tekoscData.samplerate;
    if (PTSOSC>532){
      printf("ERROR in scope sampling\n = %d, correct sampling=%d\n",tekoscData.pts,PTSOSC);
      return 1;
    }
        
    //////////////////// New DWC
    
    hits = 0;
    addr = SubEventSeek(TDC_ADDR, buf);
    rc += DecodeV775(addr, &tdcData);
    float dwc_l=0, dwc_r=0, dwc_u=0, dwc_d=0;
    for (j=0; j<tdcData.index; j++) { 
      int ch = tdcData.channel[j];
      int data = tdcData.data[j];
      int valid = tdcData.valid[j];
      CHTDC[ch]=ch;
      COUNTTDC[ch] = data;
      VALIDTDC[ch]  = tdcData.valid[j];
      hits++;
      if(ch>=1 && ch<=4){
	switch(ch){
	case 1: {dwc_l = data; break;}
	case 2: {dwc_r = data; break;}
	case 3: {dwc_u = data; break;}
	case 4: {dwc_d = data; break;}
	}
      }
    }
    NhitTDC=hits;
    if(tdcData.index!=0){
      
      //X-position = (timeRight - timeLeft) * horizontalSlope + horizontalOffset
      //Y-position = (timeUp - timeDown) * verticalSlope + verticalOffset
      
      float vSlope = 0.178042;
      float vOffset = 3.47181;
      float hSlope = 0.179104;
      float hOffset = 1.34328;
      
      float x = (dwc_r - dwc_l) * hSlope + hOffset;
      float y = (dwc_u - dwc_d) * vSlope + vOffset;

      N_X_DW=1;
      N_Y_DW=1;

      X_DW[0]=x;
      Y_DW[0]=y;
  }


   ///////////////////////////////////////////////////

    tree->Fill();
  } // End of loop forever


  // Close Data file
  // Write and close Ntuple
  //  tree->Write();
  tree->Write("",TObject::kWriteDelete);
  file->Close();

  return 0;

}
