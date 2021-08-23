#include <stdlib.h>
#include <cstdlib>
#include <iostream.h>
#include "TH1.h"
#include <TH2.h>
#include <TStyle.h>
#include "TCanvas.h"


//----------------------------------------------------------------------

void read_drs_mean(unsigned int runnumber, TString phys, unsigned int evt_max)
{
// Constants
  char ntdatafile_drs[256];
if (phys=="data"){  sprintf(ntdatafile_drs,"/home/dreamtest/working/ntuple/datafile_ntup_run%d.root", runnumber);
}
if (phys=="pedestal") { sprintf(ntdatafile_drs,"/home/dreamtest/working/ntuple/pedestal_ntup_run%d.root", runnumber);}

  TFile* f= new TFile(ntdatafile_drs);

//----------------------------------------------------------

   Int_t entries = DREAM->GetEntries();

    if (evt_max>=entries) {evt_max=entries;} 
    char cevt[20];
   TLatex l;
   l.SetTextSize(0.1);
   l.SetNDC(); //set absolute coordinates

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
      hM[i][j] = new TH1F (histo, name, 2300,-300.,2000.);
      sprintf(histo,"drs%d_ch%d_integral",i,j);
      sprintf(name,"RUN%d - DRS_%d CH%d Integral (pC)",runnumber,i,j);
//      hA[i][j] = new TH1F (histo, name, 600,-10000.,50000.);
        hA[i][j]= new TH1F (histo,name,1100,-100.,1000.); 

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

    DREAM->SetBranchAddress("trg_mask",&trg_mask);
   DREAM->SetBranchAddress("w_array0",w_array0);
   DREAM->SetBranchAddress("w_array1",w_array1);
   DREAM->SetBranchAddress("w_array2",w_array2);
   DREAM->SetBranchAddress("w_array3",w_array3);
   DREAM->SetBranchAddress("temp",temp);
float maxbin[4][4],maxval[4][4];
   for(int ientry=0; ientry <evt_max; ientry++){

     TH1F* hDRS_temp[4][4];
    for (int i=0;i<4;i++) {
     for (int j=0;j<4;j++) {
      sprintf(histo,"drs%d_ch%d_temp",i,j);
      sprintf(name,"RUN%d - DRS_%d CH temp",runnumber,i,j);
      hDRS_temp[i][j] = new TH1F (histo, name, 1024,0.,1024.);
    }
    }


  DREAM->GetEntry(ientry);
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
// Histo of the maximum
        maxbin[i][j]=hDRS_temp[i][j]->GetMaximumBin(); 
        maxval[i][j] = hDRS_temp[i][j]->GetBinContent(maxbin[i][j]);
        hM[i][j]->Fill(maxval[i][j]);
// Histo of the integral
       if (maxbin[i][j]-100>0 && maxbin[i][j]<400) {
         float  basea = 0;
         int min = 1;
         int differ;
         for(int k=1; k<51; k++){
           basea += data[i][j][k];
         }
         basea = basea/50;
         float sig = 0;
         float binc=0;
         for(int k=0; k<1024; k++){
           binc = hDRS_temp[i][j]->GetBinContent(k);
//           if((binc-basea) > 0.1*(maxval[i][j]-basea)) sig += binc-basea;
             if((binc-basea) >0.1*(maxval[i][j]-basea)) {sig+= (data[i][j][k])-basea;}

//            sig += binc-basea;
         sum[i][j][k]-=basea;
         }
         sig = sig/(5*50.) ;
          hA[i][j]->Fill(sig);
         } //i 
         }//j
        }

      for (int i=0;i<4;i++) {
        for (int j=0;j<4;j++) {
       delete  hDRS_temp[i][j];
        }
      }
      if (ientry%1000==0) {printf(" Processed %d events\n",ientry); }
      }//event 
      printf(" Computing mean time structure...\n");
      for (int i=0;i<4;i++) {
       for (int j=0;j<4;j++) {
        for (int k=0;k<1024;k++) {
              sum[i][j][k]=1.*( sum[i][j][k])/(float)evt_max;
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
