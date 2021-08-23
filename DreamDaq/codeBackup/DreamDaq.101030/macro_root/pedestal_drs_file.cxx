//Only an example to make pedestal_drs_runx.dat 
// produced using the off-line ntuples
//Read pedestal file
// Pedestal  file have AT THE MOMENT 8x10x1024 lines:
// 10x1024 lines for each CIP 
//and 4 columns: Chip_number (1-...-8) Channel(0-...-9) ADC(0...1023) Mean Rms

# include <string>
# include <cmath>
#include <iostream>
#include <stdlib.h>
#include "TTree.h"
#include "TH1F.h"
#include "TFile.h"
#include "TCut.h" 
using namespace std;
                                                                                
void pedestal_drs_file(unsigned int runnumber)
{
  FILE* file_ped;
  char ntdatafile[256];
  sprintf(ntdatafile,"pedestal_ntup_run%d.root", runnumber);
  TFile fnew(ntdatafile);
  sprintf(ntdatafile,"pedestal_drs_run%d.cal", runnumber);
  file_ped= fopen(ntdatafile,"w+");
  TCut opt; 
  cout << "File opened" << endl;
  char charge_drs[256],good_evt_drs[256];
  float mean, rms;
  TH1F* ht=new TH1F("ht","DRS ADC",4096,-0.5,4095.5);
  TTree *t=(TTree *)gDirectory->Get("DREAM");
 if(t!=NULL && t->GetEntries()>0){

  for (int k=1;k<9;k++){
        for (int j=0;j<10;j++){
          for (int i=0;i<1024;i++) {
	    ht->Reset();
	    //cout<<" K= "<<k<<" J "<<j<<" i "<<i<<endl;
           sprintf(charge_drs,"drs%da[%d][%d]",k,j,i);
            sprintf(good_evt_drs,"drs_flag==1");
            if(t->Project("ht",charge_drs,good_evt_drs)){
              mean= ht->GetMean();
              rms = ht->GetRMS();
            }else{
              mean=0;
              rms=0;
            }
           fprintf(file_ped,"%d %d %d %f %f\n",k,j,i,mean,rms);
          }
        }
        }


  }//if t  

  fclose(file_ped);
}


int main(int argc, char * argv[] ){

  if(argc<2){
    cout << "Usage: " << argv[0] << " runnumber" << endl;
  }
 
  pedestal_drs_file(::atoi(argv[1]));
  
}
