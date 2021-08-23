//Only an example to make pedestal_runx.dat 
// produced using the off-line ntuples
// and necessary for the on-line monitoring (to have ADC spettra with pedestal
// subtratted
// THIS MACRO MUST BE MODIFIED 
//Read pedestal file
// Pedestal  file have AT THE MOMENT 66 lines:
// 32 lines for ADCN0 (32 channels)
// 32 lines for ADCN1 (32 channels)
// 2 lines for ADC0 (channels 0 1)
//and 4 columns: Module_number (0 or 1) Channel Mean Rms

# include <string>
# include <cmath>
using namespace std;
                                                                                
void pedestal_file(unsigned int runnumber)
{
  FILE* file_ped;
  char ntdatafile[256];
  sprintf(ntdatafile,"pedestal_ntup_run%d.root", runnumber);
  TFile fnew(ntdatafile);
  sprintf(ntdatafile,"pedestal_run%d.cal", runnumber);
  file_ped= fopen(ntdatafile,"w+");
  TCut opt; 
  cout << "File opened" << endl;
  char channel[50],charge_modul[50];
  float mean,rms;
  TTree *t=(TTree *)gDirectory->Get("DREAM");
  for (int j=0;j<2;j++){
    for (int i=0;i<32;i++) {
      sprintf(charge_modul,"CHARGEADCN%d",j);
      sprintf(channel,"CHADCN%d==%d",j,i);
      //opt=channel;
      if(t->Draw(charge_modul,channel)){
	mean= htemp->GetMean();
	rms = htemp->GetRMS();
      }else{
	mean=0;
	rms=0;
      }
      fprintf(file_ped,"%d %d %f %f\n",j,i,mean,rms); 
    }
  }
  
  int j=0; 
  for (int i=0;i<2;i++) {
    sprintf(charge_modul,"CHARGEADC%d",j);
    sprintf(channel,"CHADC%d==%d",j,i);
    opt=channel;
    if(t->Draw(charge_modul,opt)){
      mean= htemp->GetMean();
      rms = htemp->GetRMS();
    }else{
      mean=0;
      rms=0;
    }
    fprintf(file_ped,"%d %d %f %f\n",j,i,mean,rms); 
  }

  fclose(file_ped);
 }
