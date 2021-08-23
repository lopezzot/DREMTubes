#include <iostream>
#include <fstream>
#include <cmath>
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TF2.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"

# include <stdlib.h>
# include <string>
# include <cmath>
using namespace std;


void merge_drs_standalone(unsigned int runnumber,TString phys ,unsigned int evt_max){



// Constants
   float fr=2.;
  char ntdatafile_drs[256];
  int trg_check=10;
sprintf(ntdatafile_drs,"/home/dreamtest/working/root/run_000%d.root", runnumber);


if (phys=="data"){  
//sprintf(ntdatafile_drs,"/home/dreamtest/working/ntuple/datafile_ntup_run%d.root", runnumber);
trg_check=1;
}
if (phys=="pedestal") { 
//sprintf(ntdatafile_drs,"/home/dreamtest/working/ntuple/pedestal_ntup_run%d.root", runnumber);
trg_check=2;}

  TFile* f= new TFile(ntdatafile_drs);

//----------------------------------------------------------

//   Int_t entries = t_drs->GetEntries();

      TTree *t_drs= (TTree *)f->Get("DRS4");

     Int_t entries = t_drs->GetEntries();

    if (evt_max>=entries) {evt_max=entries;} 
    char cevt[20];
/*   TLatex l;
   l.SetTextSize(0.1);
   l.SetNDC(); //set absolute coordinates
*/
   cout << "Booking histograms..." << endl;

// Book histograms...
TH1F* hDRS[4][4];
TH1F* hM[4][4];
TH1F* hA[4][4];

 char histo[20],name[256];
   for (int i=0;i<4;i++) {
     for (int j=0;j<4;j++) {
      sprintf(histo,"drs%d_ch%d",i,j);
      sprintf(name,"RUN%d - DRS_%d CH%d",runnumber,i,j);
      hDRS[i][j] = new TH1F (histo, name, 1024,0.,1024.);
     sprintf(histo,"drs%d_ch%d_maximum",i,j);
      sprintf(name,"RUN%d - DRS_%d CH%d Maximum(mV) ",runnumber,i,j);
      hM[i][j] = new TH1F (histo, name, 200,0.,1000.);
      sprintf(histo,"drs%d_ch%d_integral",i,j);
      sprintf(name,"RUN%d - DRS_%d CH%d Integral (pC)",runnumber,i,j);
//      hA[i][j] = new TH1F (histo, name, 600,-10000.,50000.);
//        hA[i][j]= new TH1F (histo,name,1100,-100.,1000.); 
         hA[i][j]= new TH1F (histo,name,750,0.,1500.);
    }
   }

   float data[4][4][1024];
   float sum[4][4][1024];

   for(int i=0;i<4;i++) {
      for (int j=0;j<4;j++){
        for (int k=0;k<1024;k++){
        sum[i][j][k]=0.;
      }
     }
   }
//----------------------------------------------------------------   

   Short_t w_array0[4][1024];
   Short_t w_array1[4][1024];
   Short_t w_array2[4][1024];
   Short_t w_array3[4][1024];
   Float_t temp[4];
   int trg_mask;

    t_drs->SetBranchAddress("trg_mask",&trg_mask);
   t_drs->SetBranchAddress("w_array0",w_array0);
   t_drs->SetBranchAddress("w_array1",w_array1);
   t_drs->SetBranchAddress("w_array2",w_array2);
   t_drs->SetBranchAddress("w_array3",w_array3);
   t_drs->SetBranchAddress("temp",temp);
int maxbin[4][4];
float maxval[4][4];
int ientry_phys=0;
   for(int ientry=0; ientry <evt_max; ientry++){
    t_drs->GetEntry(ientry);
   if (trg_mask==trg_check) {
     TH1F* hDRS_temp[4][4];
    for (int i=0;i<4;i++) {
     for (int j=0;j<4;j++) {
      sprintf(histo,"drs%d_ch%d_temp",i,j);
      sprintf(name,"RUN%d - DRS_%d CH temp",runnumber,i,j);
      hDRS_temp[i][j] = new TH1F (histo, name, 1024,0.,1024.);
    }
    }


//      if (trg_mask!=1) cout<<" ERRORE evento "<<ientry<<" tgr "<<trg_mask<<endl; 
       for (int j=0;j<4;j++) { 
         for(int k=0; k<1024; k++){
            data[0][j][k]=(float) w_array0[j][k];
            data[1][j][k]=(float) w_array1[j][k];
            data[2][j][k]=(float) w_array2[j][k];
            data[3][j][k]=(float) w_array3[j][k];
          sum[0][j][k]+=data[0][j][k];
          sum[1][j][k]+=data[1][j][k];
          sum[2][j][k]+=data[2][j][k];
          sum[3][j][k]+=data[3][j][k];
       for (int i=0;i<4;i++) {
          hDRS_temp[i][j]->SetBinContent(k,data[i][j][k]);
          }  
          }
         }

     for (int i=0;i<4;i++) {
         for (int j=0;j<4;j++) {
// Histo of the maximumù
         maxbin[i][j]=0;
        maxbin[i][j]=hDRS_temp[i][j]->GetMaximumBin(); 
        maxval[i][j]=0;
       for(int i2=maxbin[i][j]-5; i2<maxbin[i][j]+5; i2++){
         maxval[i][j] += hDRS_temp[i][j]->GetBinContent(i2);
       }
       maxval[i][j] = maxval[i][j]/10.;
        //maxval[i][j] = hDRS_temp[i][j]->GetBinContent(maxbin[i][j]);
        hM[i][j]->Fill(maxval[i][j]);
// Histo of the integral
       if (maxbin[i][j]-100>0 && maxbin[i][j]<600) {
         float  basea = 0;
         int min = 1;
         int differ;
         for(int k=min; k<50+min; k++){
           basea += data[i][j][k];
         }
         basea = basea/50;
         float sig = 0;
         float binc=0;
         for(int k=0; k<1024; k++){
           binc = hDRS_temp[i][j]->GetBinContent(k);
             if((binc-basea) >0.1*(maxval[i][j]-basea)) {
              sig+= (data[i][j][k])-basea;}

         sum[i][j][k]-=basea;
         }
         sig = sig/(fr)/50. ;
          hA[i][j]->Fill(sig);
         } //i 
         }//j
        }

      for (int i=0;i<4;i++) {
        for (int j=0;j<4;j++) {
       delete  hDRS_temp[i][j];
        }
      }
      
      if (ientry_phys%1000==0) {printf(" Processed %d events\n",ientry_phys); }

      ientry_phys++; 
      }//trg_check
      }//event 
      printf(" Computing mean time structure...\n");
      for (int i=0;i<4;i++) {
       for (int j=0;j<4;j++) {
        for (int k=0;k<1024;k++) {
              sum[i][j][k]=1.*( sum[i][j][k])/(float)ientry_phys;
         hDRS[i][j]->SetBinContent(k,sum[i][j][k]);
         }
       } 
      }





char ntfilenamehisto[256];

if (phys=="data"){
  sprintf(ntfilenamehisto, "/home/dreamtest/working/hbook/datafile_histo_run%d_drs.root", runnumber);
  }else if (phys=="pedestal"){
  sprintf(ntfilenamehisto, "/home/dreamtest/working/hbook/pedestal_histo_run%d_drs.root",  runnumber);
  }

TFile hfile(ntfilenamehisto,"RECREATE");

for (int i=0;i<4;i++) {
for (int j=0;j<4;j++) {
hDRS[i][j]->Write();
hM[i][j]->Write();
hA[i][j]->Write();
}
}
hfile.Close();


}


int main(int argc, char * argv[]){


  if(argc < 2){
    cout << "Usage: " << argv[0] << " runnr " << endl;
    exit(1);
  }


  int runnr = atoi(argv[1]);

  int evt_max= atoi(argv[3]);

//  merge_drs_standalone(runnr, TString(argv[2]), evt_max );
    merge_drs_standalone(runnr, "pedestal", 50000 );
    merge_drs_standalone(runnr, "data"    , 50000 );
}
