/*************************************************************************

        myDaq2Ntu.c
        -----------

        DreamDaq --> Ntuple conversion program
        Version 2014      M. Cascella

*********************************************************************/

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdio>

#include <iostream>     

#include <TTree.h>
#include <TFile.h>
#include <TH1.h> 
#include <TMath.h>

#include "eventX742.h"
#include "eventX742_corrections.h"
#include "myRawFile.h"

//use the following on pcdreamdaq2 or pcdreamus
#include "X742CorrectionRoutines.h"
#define CORRECTION_FILE "/home/dreamtest/working/board_corrections.dat"

//use the following at TTU
/*
#define CORRECTION_FILE "/lustre/hep/osg/dream/Ntuple_gen/DreamDaq/board_corrections.dat"
extern "C" {
#include "../X742CorrectionRoutines.h"
}
*/

#include "../mapping.h" 


TTree *tree;
/* Global Variables for NTUPLE (not all used in the ntuple at the moment) */
bool phys;

int Nrunnumber;           // Number of the Run  
int BegTimeEvs;             // Time of first event in Seconds
int BegTimeEvu;             // Time of first event in MicroSeconds
int TimeEvs;                // Time of event in Seconds
int TimeEvu;                // Time of event in MicroSeconds
unsigned int Nevtda = 0 ;             // Number of data event

///////////////////// SCALER
#define SCALER_ADDR 0x00200003

unsigned int NSCA;              // Total number of counts in Scaler 
unsigned int CHSCA[16];         // Channels in the scaler (the channel of the count) 
unsigned int  COUNTSCA[16];     //  Counts in the Scaler  (the value of the count)

#define MAX_OSC_POINTS 1032
#define MAX_OSC_HITS (4*MAX_OSC_POINTS)     // 4128 max hits
///////////////////// OSC
#define OSC_ADDR 0x0000fafa
unsigned int NhitOSC;           // Total number of points in the Oscilloscope
                                // (=MAX_OSC_HITS if there are 4 channels each with 282 points )
unsigned int CHOSC[MAX_OSC_HITS];       // Channels in the oscilloscope
                                // CHOSC[i] is the channel of the i-th point )
Char_t DATAOSC[MAX_OSC_HITS];              // Values of the poit in the oscilloscope
Char_t DATAOSC_CH[4][MAX_OSC_POINTS];              // Values of the point in the oscilloscope

// (DATAOSC[i] is the value of the i-th point)
unsigned int NUMOSC[MAX_OSC_HITS];      // Number of the point in the channel (max value =282)

int POSOSC[4];                  // Position for Oscilloscope (as in myOscDataFile.h)
unsigned int TDOSC[4];          // Time Diff for Oscilloscope (as in myOscDataFile.h)
unsigned int SCALEOSC[4];       // Scale of the Oscilloscope (as in myOscDataFile.h)
unsigned int CHFOSC[4];         // Oscilloscope Channel Flag (0= channel enabled, 1 =channel abled)
unsigned int PTSOSC;            // Oscilloscope pts (as in myOscDataFile.h , at the moment =282)
unsigned int  SROSC;             // Sampling rate of the Oscilloscope (as in myOscDataFile.h)
float PED_MEAN_OSC[4];
float PED_RMS_OSC[4];

///////////////////// DRS
#define DRS_ADDR 0x00032026 

const int MAX_GROUP = MAX_X742_GROUP_SIZE;
const int MAX_CHANNEL = MAX_V1742_CHANNEL_SIZE;

/* const int MAX_GROUP = 2; */
/* const int MAX_CHANNEL = 4; */

UShort_t DATADRS[MAX_GROUP][MAX_CHANNEL][1024];
Float_t INTDRS[MAX_GROUP][MAX_CHANNEL]; 
float PED_MEAN_DRS[MAX_GROUP][MAX_CHANNEL];
float PED_RMS_DRS[MAX_GROUP][MAX_CHANNEL];

Int_t STARTING_CELL_DRS[MAX_GROUP];

CAEN_DGTZ_X742_EVENT_t *x742evt;
CAEN_DGTZ_V1742_EVENT_t *EventV1742;
static DataCorrection_t	CTable[MAX_X742_GROUP_SIZE][MAX_X742_FREQ_SETTINGS_N];

///////////////////// ADCs

unsigned int NhitADCN[N_ADC];         // Hits in ADCN[] (the total number of couts in ADCNO, MODULE 0 of V792AC)  
unsigned int CHADCN[N_ADC][N_CH_ADC];       // Channel in ADCN[] (the channel of cout)
                                // (CHADCN[][i] is the channel of the i-th count )
unsigned int CHARGEADCN[N_ADC][N_CH_ADC] ;  // Charge in ADCN[]  (the value of the count)
                                // (CHARGEADCN[][i] is the  value of the i-th cout )
UChar_t OVERADCN[N_ADC][N_CH_ADC] ; 
UChar_t UNDERADCN[N_ADC][N_CH_ADC] ; 

float   PED_MEAN_ADCN[N_ADC][N_CH_ADC];
float   PED_RMS_ADCN[N_ADC][N_CH_ADC];

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
void ntbook(bool hasOSC,bool hasDRS)
/************************************************************************/
{                                                                                                                             
  //  TTree::SetMaxTreeSize(1000*Long64_t(2000000000));
  // Declare ntuple identifier and title
  tree = new TTree("DREAM","Dream data");

  // Describe data to be stored in the Ntuple (see comments before) 
 
  tree->Branch("Nrunnumber",&Nrunnumber,"Nrunnumber/i");
  tree->Branch("TimeEvs",&TimeEvs,"TimeEvs/i");
  tree->Branch("TimeEvu",&TimeEvu,"TimeEvu/i");
  tree->Branch("Nevtda",&Nevtda,"Nevtda/i");

  if(hasOSC){
    tree->Branch("DATAOSC",DATAOSC,"DATAOSC[4128]/B");
    tree->Branch("DATAOSC_CH",DATAOSC_CH,"DATAOSC[4][1032]/B");
    tree->Branch("CHFOSC",CHFOSC,"CHFOSC[4]/i");
    tree->Branch("POSOSC",POSOSC,"POSOSC[4]/I");
    tree->Branch("SCALEOSC",SCALEOSC,"SCALEOSC[4]/i");
    tree->Branch("TDOSC",TDOSC,"TDOSC[4]/i");
    tree->Branch("PTSOSC",&PTSOSC,"PTSOSC/i");
    tree->Branch("SROSC",&SROSC,"SROSC/i");
    tree->Branch("PED_MEAN_OSC",PED_MEAN_OSC,"PED_MEAN_OSC[4]/F");
    tree->Branch("PED_RMS_OSC",PED_RMS_OSC,"PED_RMS_OSC[4]/F");
  }
  if(hasDRS){
    tree->Branch("DATADRS",DATADRS,Form("DATADRS[%d][%d][1024]/s",MAX_GROUP,MAX_CHANNEL));
    tree->Branch("INTDRS",INTDRS,Form("INTDRS[%d][%d]/F",MAX_GROUP,MAX_CHANNEL));
    tree->Branch("PED_MEAN_DRS",PED_MEAN_DRS,Form("PED_MEAN_DRS[%d][%d]/F",MAX_GROUP,MAX_CHANNEL));
    tree->Branch("PED_RMS_DRS",PED_RMS_DRS,Form("PED_RMS_DRS[%d][%d]/F",MAX_GROUP,MAX_CHANNEL));
    tree->Branch("STARTING_CELL_DRS",STARTING_CELL_DRS,Form("STARTING_CELL_DRS[%d]/I",MAX_GROUP));
  }

  for(int i_adc=0; i_adc<N_ADC; i_adc++){
    tree->Branch(Form("CHADCN%d",i_adc),CHADCN[i_adc],Form("CHADCN%d[%d]/i",i_adc,N_CH_ADC));
    tree->Branch(Form("CHARGEADCN%d",i_adc),CHARGEADCN[i_adc],Form("CHARGEADCN%d[%d]/i",i_adc,N_CH_ADC));
    tree->Branch(Form("OVERADCN%d",i_adc),OVERADCN[i_adc],Form("OVERADCN%d/b",i_adc));
    tree->Branch(Form("UNDERADCN%d",i_adc),UNDERADCN[i_adc],Form("UNDERADCN%d/b",i_adc));

    tree->Branch(Form("PED_MEAN_ADCN%d",i_adc),PED_MEAN_ADCN[i_adc],Form("PED_MEAN_ADCN%d[%d]/F",i_adc,N_CH_ADC));
    tree->Branch(Form("PED_RMS_ADCN%d",i_adc),PED_RMS_ADCN[i_adc],Form("PED_RMS_ADCN%d[%d]/F",i_adc,N_CH_ADC));
  }

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
}

void fill_pedestals(char* pedntfilename,bool hasOSC,bool hasDRS){

  // Read pedestal information for the different modules from Pedestal Ntuple
  char channel[50],charge_modul[50];

  TFile *fileped = new TFile(pedntfilename,"READ");
  if (!(fileped->IsOpen())){
    printf("myDaq2Ntu: cannot open pedestal ntuple file %s\n",pedntfilename);
  }else {
    TTree *t=(TTree *)gDirectory->Get("DREAM");
      
    if(t!=NULL && t->GetEntries()>0){
	
      TH1F *ht=new TH1F("ht","ADC",4096,-0.5,4095.5);

      for (unsigned int j=0;j<N_ADC;j++){
	for (unsigned int i=0;i<N_CH_ADC;i++) {
	  sprintf(charge_modul,"CHARGEADCN%d",j);
	  sprintf(channel,"CHADCN%d==%d",j,i);
	  if(t->Project("ht",charge_modul,channel))  {
	    PED_MEAN_ADCN[j][i]= ht->GetMean();
	    PED_RMS_ADCN[j][i] = ht->GetRMS();
	  }else{
	    PED_MEAN_ADCN[j][i]=0;
	    PED_RMS_ADCN[j][i]=0;
	  }
	  ht->Reset();
	}
      }


      delete ht;

      if(hasOSC | hasDRS){
	UInt_t ptsosc,chfosc[4];
	Char_t dataosc[MAX_OSC_HITS];
	Int_t datadrs[MAX_GROUP][MAX_CHANNEL][1024];
	  
	TBranch *b_DATADRS=0, *b_PTSOSC=0, *b_CHFOSC=0, *b_dataosc=0;

	if(hasOSC){
          t->SetBranchAddress("DATAOSC",dataosc,&b_dataosc);
          t->SetBranchAddress("PTSOSC",&ptsosc,&b_PTSOSC);
          t->SetBranchAddress("CHFOSC",chfosc,&b_CHFOSC);
        }
	if(hasDRS)t->SetBranchAddress("DATADRS",datadrs,&b_DATADRS);
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
	TH1F *htdrs[MAX_GROUP][MAX_CHANNEL];
	for(unsigned int j=0;j<4;j++) {
	  if (chfosc[j]==1){
	    sprintf(histo,"ht%d",j);
	    sprintf(name,"CHFOSC%d",j);
	    htosc[j]=new TH1F(histo,name,256,-128.5,127.5);
	  }
	} 
	for(int i_gr=0;i_gr<MAX_GROUP;i_gr++) {
	  for(int ch=0;ch<MAX_CHANNEL;ch++) {
	    sprintf(histo,"ht%d,%d",i_gr,ch);
	    sprintf(name,"CHFDRS%d_%d",i_gr,ch);
	    htdrs[i_gr][ch]=new TH1F(histo,name,4096,0,4096);
	  }
	}

	for (unsigned int j=0;j<t->GetEntriesFast();j++){
	  t->GetEntry(j);
	  for (int i=0;i<4;i++) {
	    if (chfosc[i]==1) {
	      for (unsigned l=0;l<ptsosc;l++) {
		htosc[i]->Fill(dataosc[(k[i]*ptsosc)+l]);
	      } 
	    } 
	  }	
	  for(int i_gr=0;i_gr<MAX_GROUP;i_gr++) {
	    for(int ch=0;ch<MAX_CHANNEL;ch++) {
	      for (unsigned sample=0;sample<1024;sample++) {
		htdrs[i_gr][ch]->Fill(datadrs[i_gr][ch][sample]);
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
	    PED_MEAN_OSC[i]=0;
	    PED_RMS_OSC[i]=0;
	  }
	}
	for(int i_gr=0;i_gr<MAX_GROUP;i_gr++) {
	  for(int ch=0;ch<MAX_CHANNEL;ch++) {
	    PED_MEAN_DRS[i_gr][ch]=htdrs[i_gr][ch]->GetMean();
	    PED_RMS_DRS[i_gr][ch]=htdrs[i_gr][ch]->GetRMS();
	    delete htdrs[i_gr][ch];
	  }
	}
      }
    }//entries>0
    fileped->Close(); 
  }//file open
}


/************************************************************************/
int main(int argc, char **argv)
/************************************************************************/
{
  int rc;
  unsigned int j;
  int hits;
  unsigned int buf[30000];
  unsigned int *addr;
  //mySCA scaData;
  myADCN adcnData;
  myTDC tdcData;

  char datadir[256];
  char datafileprefix[256];
  char datafilename[256];
  char ntdir[256]; 
  char ntfilename[256];
  int isFirstEvent = 1;
  //  int istat;
  int carry;

  //create official CAEN structure
  x742evt = newEventX742();
  EventV1742 = newEventV1742(); 

  char fname[1000];
  //board_corrections.dat file should be added and the absolute path is needed to be set.
  sprintf(fname,CORRECTION_FILE);
  FILE *fp=std::fopen(fname,"r");

  //added by sehwook
  if (fp == NULL) {fputs("File error",stderr);exit(1);}

  fread(&CTable, sizeof(CTable), 1, fp);

  //added by sehwook
  //size_t result;
  //result=fread(&CTable, sizeof(CTable), 1, fp);
  //if (result != sizeof(CTable)) {fputs ("Reading error",stderr);exit(2);} 

  fclose(fp);
  //loadCorrectionFiles(); // load cell by cell corrections from /etc/wavedump/ 

  //test sehwook
/*   int fr=CAEN_DGTZ_DRS4_5GHz; */
/*   for(int gr=0; gr<MAX_X742_GROUP_SIZE; gr++) */
/*   { */
/*      for(int sample=0; sample<1024; sample++) */
/*      { */
/*         for(int ch=0; ch<MAX_X742_CHANNELS+1; ch++) */
/*         { */
/*            std::cout << "CTable["<<gr<<"]["<<fr<<"]"  */
/*                      <<".cell["<< ch<<"]["<< sample <<"] = "  */
/*                      << CTable[gr][fr].cell[ch][sample]  */
/*                      << std::endl; */
/*            std::cout << "CTable["<<gr<<"]["<<fr<<"]"  */
/*                      <<".nsample["<< ch<<"]["<< sample <<"] = "  */
/*                      << (int) CTable[gr][fr].nsample[ch][sample]  */
/*                      << std::endl; */
/*         } */
/*         std::cout << "CTable["<<gr<<"]["<<fr<<"]"  */
/*                   <<".time["<< sample <<"] = "  */
/*                   << CTable[gr][fr].time[sample]  */
/*                   << std::endl; */
/*      } */
/*   } */

  if(argc<3){
    std::cout << std::endl 
	<< "Usage: " << argv[0] << " <run_number> [pedestal|data]" << std::endl 
	      << std::endl;
    std::cout << "Use the following environment variables to change the program behaviour"<< std::endl;
    std::cout << "Input binary files are searched in:"<< std::endl;
    std::cout << "\t $DATADIR/data"<< std::endl;
    std::cout << "\t $DATADIR/pedestal"<< std::endl;
    std::cout << "Input root files for pedestal math are searched in:"<< std::endl;
    std::cout << "\t $NTUPLEDIR/"<< std::endl;
    std::cout << "Output files will be placed in:"<< std::endl;
    std::cout << "\t $NTUPLEDIR/"<< std::endl;
    exit(1);
  }
  
  if(!strcmp(argv[2],"pedestal") | !strcmp(argv[2],"ped")){
    phys=false;
  }else if(!strcmp(argv[2],"data")){
    phys=true;
  }else{
    std::cout << "Unknown data type: " << argv[2] << std::endl;
    exit(1);
  }

  // Create Input/Output filenames
  if (getenv("DATADIR") == NULL){
      sprintf(datadir, "/home/dreamtest/storage/");
  }else
    sprintf(datadir, "%s", getenv("DATADIR"));
  
  if (getenv("NTUPLEDIR") == NULL)
    sprintf(ntdir, "/home/dreamtest/storage/ntuple");
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

  //defaults to true for debugging purposes
  bool hasOSC = false,hasDRS = false;

   // Open Data file
  rc = RawFileOpen(datafilename);
  if (rc) {
    printf("myDaq2Ntu: cannot open data file %s\n", argv[1]);
    return -1;
  }

  for(int i=1; i<10;i++){
    rc = RawFileReadEventData(buf);
    if ((rc == RAWDATAEOF) | (rc == RAWDATAUNEXPECTEDEOF)) {
      break;
    }
    addr =  SubEventSeek(OSC_ADDR, buf);
    if(addr!=0) hasOSC = true;
    addr =  SubEventSeek(DRS_ADDR, buf);
    if(addr!=0) hasDRS = true;
  }  
  RawFileClose(); 
  rc = RawFileOpen(datafilename);

  // Open file
  if(phys){
    char pedntfilename[256];
    sprintf(pedntfilename, "%s/pedestal_ntup_run%s.root", ntdir, argv[1]);
    fill_pedestals(pedntfilename, hasOSC, hasDRS);
  }
  TFile *file = new TFile(ntfilename,"RECREATE");

  if (!file->IsOpen()) {
    printf("myDaq2Ntu: cannot open ntuple file %s\n", ntfilename);
    return -1;
  }
  printf("output file: %s\n",file->GetName());
  file->SetCompressionLevel(1);

  // Book Ntuple
  ntbook(hasOSC, hasDRS);
  tree->SetMaxTreeSize(1073741824); //Max size = 1Gb

  int max_evt = -1;
  if (getenv("MAXEVT") != NULL){
    max_evt = atoi(getenv("MAXEVT"));
  }

  // Main Loop on all events
  for (;;) {

    // Read Event and write it in buffer 'buf'
    rc = RawFileReadEventData(buf);
    if (rc == RAWDATAEOF) { 
      printf("Found EOF at event %d\n", GetEventNumber()); 
    } else if (rc == RAWDATAUNEXPECTEDEOF) { 
      printf("Unexpected EOF at event %d\n", GetEventNumber()); 
    }
    if ((rc == RAWDATAEOF) | (rc == RAWDATAUNEXPECTEDEOF)) {
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

    if(max_evt>0 && Nevtda>(unsigned int)max_evt){
      printf("reached max number of events\n");
      break;
    }

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

    // FILL ADC 

  for(int i_adc=0; i_adc<N_ADC; i_adc++){
    for (int i=0;i<N_CH_ADC;i++){     
      CHADCN[i_adc][i] = 100;
    }
    hits = 0;
    addr =  SubEventSeek(adc_num_to_vme_addr[i_adc], buf);

    if (addr != NULL) {
      rc += DecodeV792AC(addr, &adcnData);
    
      for (j=0; j<adcnData.index; j++) {
        int ch = adcnData.channel[j];
        CHADCN[i_adc][ch]  = adcnData.channel[j];
        CHARGEADCN[i_adc][ch] = adcnData.data[j];
        OVERADCN[i_adc][ch] = adcnData.ov[j];
        UNDERADCN[i_adc][ch] = adcnData.un[j];
/* 	std::cout << "adc"<< i_adc << "_"<< ch */
/* 		  << "  CHADCN: " << CHADCN[i_adc][ch] */
/* 		  << "  CHARGEADCN: " << CHARGEADCN[i_adc][ch] */
/* 		  << "  OVERADCN: " << OVERADCN[i_adc][ch] */
/* 		  << "  UNDERADCN: " << UNDERADCN[i_adc][ch] */
/* 		  << std::endl; */
        hits++;
      }
    }
    NhitADCN[i_adc] = hits;
  }

	    //////////////////// DRS

	  // Look for address of subevent containing DRS data
	  addr =  SubEventSeek(DRS_ADDR, buf);
	  if(addr!=0){
	    // DRS "DGTZ" buffers
	    
	    CAEN_DGTZ_EventInfo_t eventInfo;
	    GetEventInfo((char*)(addr+4), &eventInfo );
	    // //cascella: print debug info
	    // std::cout <<  std::hex << "EventSize: " << eventInfo.EventSize << " "
	    // 	      << "BoardId: " << eventInfo.BoardId << " "
	    // 	      << "Pattern: " << eventInfo.Pattern << " "
	    // 	      << "ChannelMask: "<<  eventInfo.ChannelMask << " "
	    // 	      << "EventCounter: " << eventInfo.EventCounter << " "
	    // 	      << "TriggerTimeTag: " << eventInfo.TriggerTimeTag << " " 
	    // 	      <<  std::dec<<  std::endl;

	    for(int gr = 0; gr<MAX_X742_GROUP_SIZE; gr++){
	      //if the group is not present in the mask skip it
	      if(! ((eventInfo.ChannelMask >> gr) & 1) )continue;
	      V1742UnpackEventGroup((uint32_t *)addr+(8+3458*gr), &(EventV1742->Group[gr])); 
	    }

	    // //cascella: print out some of the numbers before the correction
	    // for(int i_gr=0; i_gr<MAX_X742_GROUP_SIZE; i_gr++){
	    //   if(! ((eventInfo.ChannelMask >> i_gr) & 1) )continue;
	    //   for(int i_ch=0; i_ch<MAX_V1742_CHANNEL_SIZE; i_ch++ ) {
	    // 	for(int sample=0; sample< NUM_SAMPLES; sample++) {
	    // 	  float data = EventV1742->Group[i_gr].Data[i_ch][sample];
	    // 	  //std::cout << Nevtda<< ") before correction data["<<i_gr<<"]["<<i_ch<<"]["<<sample<<"] = " << data << std::endl;
	    // 	}
	    //   }
	    // }


	    CAEN_DGTZ_DRS4Frequency_t freq = CAEN_DGTZ_DRS4_2_5GHz; //can be read from data

	    for(int i_gr=0; i_gr<MAX_GROUP; i_gr++){
	      //if the group is not present in the mask, skip it
	      if(! ((eventInfo.ChannelMask >> i_gr) & 1) )continue;
	      x742evt->DataGroup[i_gr].TriggerTimeTag = EventV1742->Group[i_gr].TriggerTimeTag;
	      x742evt->DataGroup[i_gr].StartIndexCell = EventV1742->Group[i_gr].StartIndexCell;

	      for(int i_ch=0; i_ch<MAX_X742_CHANNEL_SIZE; i_ch++){
		for(int sample=0; sample<1024; sample++){
		  x742evt->DataGroup[i_gr].DataChannel[i_ch][sample]= EventV1742->Group[i_gr].Data[i_ch][sample];
		}
	      }
	    }
	   
	    for(int i_gr=0; i_gr<MAX_X742_GROUP_SIZE; i_gr++){
	      //if the group is not present in the mask, skip it
	      if(! ((eventInfo.ChannelMask >> i_gr) & 1) )continue;
		ApplyDataCorrection(&CTable[i_gr][freq], 
				    freq,
				    7, // it's a bitmask 7=0b111
				    &(x742evt->DataGroup[i_gr]));
	    }

	    // set int counter to zero
	    for(int i_gr=0;i_gr<MAX_GROUP;i_gr++){
	      for(int i_ch=0; i_ch<MAX_CHANNEL; i_ch++){
		INTDRS[i_gr][i_ch] = 0;
		for(int sample=0; sample< NUM_SAMPLES; sample++) {
		  DATADRS[i_gr][i_ch][sample] = 0;
		}
	      }
	    }
	    for(int i_gr=0; i_gr<MAX_GROUP; i_gr++){
	      //if the group is not present in the mask skip it
	      if(! ((eventInfo.ChannelMask >> i_gr) & 1) )continue;
	      STARTING_CELL_DRS[i_gr] = EventV1742->Group[i_gr].StartIndexCell;
	      for(int i_ch=0; i_ch<MAX_CHANNEL; i_ch++ ) {
		for(int sample=0; sample< NUM_SAMPLES; sample++) {
		  float data = x742evt->DataGroup[i_gr].DataChannel[i_ch][sample];
		  INTDRS[i_gr][i_ch] += data;
		  Float_t rescaled= round((16.*data));
		  if(rescaled>4096*16) rescaled=4096*16;
		  DATADRS[i_gr][i_ch][sample] = (UShort_t)round(rescaled);
		  // //cascella: print out the same info after the correction
		  // std::cout << Nevtda<< ") after correction ["<<i_gr<<"]["<<i_ch<<"]["<<sample<<"] = " << data << std::endl;
		}
	      }
	    }

	    // End of DRS analsis - free DRS "DGTZ" buffers
	  }
	    //////////////////// New DWC
	    
	    hits = 0;
	    addr = SubEventSeek(TDC_ADDR, buf);
	    rc += DecodeV775(addr, &tdcData);

	    int dwc1_l=9990, dwc1_r=9990, dwc1_u=9990, dwc1_d=9990;
	    int dwc2_l=9990, dwc2_r=9990, dwc2_u=9990, dwc2_d=9990;
	  

	    for (j=0; j<tdcData.index; j++) { 
	      unsigned int ch = tdcData.channel[j];
	      unsigned int data = tdcData.data[j];
	      unsigned int valid = tdcData.valid[j];

	      //std::cout << "ch: " << ch << " data: " << data << std::endl;

	      CHTDC[ch]=ch;
	      COUNTTDC[ch] = data;
	      VALIDTDC[ch]  = valid;
	      hits++;
	      switch(ch){
	       case 0: {dwc1_l = data; break;}
	       case 1: {dwc1_r = data; break;}
	       case 2: {dwc1_u = data; break;}
	       case 3: {dwc1_d = data; break;}
	       case 4: {dwc2_l = data; break;}
	       case 5: {dwc2_r = data; break;}
	       case 6: {dwc2_u = data; break;}
	       case 7: {dwc2_d = data; break;}
	      }
	    }
	    NhitTDC=hits;
	    if(tdcData.index!=0){
	      
	      //X-position = (timeRight - timeLeft) * horizontalSlope + horizontalOffset
	      //Y-position = (timeUp - timeDown) * verticalSlope + verticalOffset
	      
	/*       float vSlope1 = 0.175; */
	/*       float vOffset1 = 0.26; */
	/*       float hSlope1 = 0.174; */
	/*       float hOffset1 = 0.26; */
	      
	/*       float vSlope2 = 0.182; */
	/*       float vOffset2 = 0.; */
	/*       float hSlope2 = 0.182; */
	/*       float hOffset2 = 0.27; */

            /*
	    //  Jul2012 calibration 
	    float vSlope1 = -0.189392;
	    float vOffset1 = 0.353532;
	    float hSlope1 = -0.190354;
	    float hOffset1 = -0.101521;
	    
	    float vSlope2 = -0.185638;
	    float vOffset2 = 0.374063;
	    float hSlope2 = -0.186101;
	    float hOffset2 = -0.14888;
            */

            // Dec2014 calibration
            float hSlope1 = -0.178388; // fit
            float hOffset1 = 0.646796; // fit
            float vSlope1 = 0.174544; // fit
            float vOffset1 = 1.0665; // fit

            float hSlope2 = -0.172184; // fit
            float hOffset2 = -0.00774708; // fit
            float vSlope2 = 0.175517; // fit
            float vOffset2 = 0.61149; // fit

	      float x1=9999;
	      float x2=9999;
	      float y1=9999;
	      float y2 =9999;
              float couts_to_ns = 0.1391; //was 0.2472

	      x1 = (dwc1_r - dwc1_l) * hSlope1*couts_to_ns + hOffset1;
	      x2 = (dwc2_r - dwc2_l) * hSlope2*couts_to_ns + hOffset2;
	      y1 = (dwc1_u - dwc1_d) * vSlope1*couts_to_ns + vOffset1;
	      y2 = (dwc2_u - dwc2_d) * vSlope2*couts_to_ns + vOffset2;
		   
	      N_X_DW=2;
	      N_Y_DW=2;

	      X_DW[0]=x1;
	      Y_DW[0]=y1;
	      X_DW[1]=x2;
	      Y_DW[1]=y2;
	  }


	   ///////////////////////////////////////////////////

	    tree->Fill();
	  } // End of loop forever

	  // Close Data file
	  // Write and close Ntuple
	  //  tree->Write();
	  file = tree->GetCurrentFile(); //to get the pointer to the current file
	  //tree->Write("",TObject::kWriteDelete);
	  file->Write();
	  file->Close();

	  if (EventV1742) destroyEventV1742(EventV1742); 
	  return 0;

}
