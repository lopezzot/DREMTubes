
#include "dreammon.hxx"
#include "myRawFile.h"
#include "TString.h"
//int dream_adc_to[32]=     {-100,  -1,  -2,  -3,  -4,  -5,  -6,  -7,  -8,  -9,  -10,  -11,  -12,  -13,  -14,  -15,  -16,  -17,  -18,  -19,  -20,  -21,  -22,  -23,  -24,  -25,  -26,  -27,  -28,  -29,  -30,  -31};
int dream_adc_to_ch[32]=    {-100,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,   11,   12,   13,   14,   15,  -16,   16,   17,   18,   19,  -20,  -21,  -23,  -24,  -25,  -26,  -27,  -28,  -29,  -30,  -31};
float dream_adc_to_x[32]=   {-100,   0, -.5,  -1, -.5,  .5,   1,  .5,   0,  -1, -1.5,   -2, -1.5,   -1,    0,    1, -100,  1.5,    2,  1.5,    1, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100};
float dream_adc_to_y[32]=   {-100,   0,  -1,   0,   1,   1,   0,  -1,  -2,  -2,   -1,    0,    1,    2,    2,    2, -100,    1,    0,   -1,   -2, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100};

int newdream_adc_to_ch[32]= {-100,   1,   2,   3,   4,  -5,  -6,  -7,  -8,  -9,  -10,  -11,  -12,  -13,  -14,  -15,  -16,  -17,  -18,  -19,  -20,  -21,  -22,  -23,  -24,  -25,  -26,  -27,  -28,  -29,  -30,  -31};
float newdream_adc_to_x[32]={-100,  -1,   1,   1,  -1,-100,-100,-100,-100,-100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100};
float newdream_adc_to_y[32]={-100,   1,   1,   -1, -1,-100,-100,-100,-100,-100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100};

using namespace std;

void dreammon::dreammon(unsigned int run_nr, bool phys_h)
  Nrunnumber(run_nr), 1ev_counter(0){

  string myWorkDir;
  if (getenv("WORKDIR") == NULL)
    myWorkDir = "/home/dreamtest/working/";
  else
    myWorkDir = getenv("WORKDIR");
  
  if (getenv("HISTODIR") == NULL)
    ntdir = Form("%s/%s", myWorkDir, "hbook");
  else
    ntdir = getenv("HISTODIR");

  if (phys_h){
    ntfilename = Form("%s/datafile_histo_run%d.root", ntdir, Nrunnumber);
  }else {
    ntfilename = Form("%s/datafile_histo_pedestal_run%d.root", ntdir, Nrunnumber);
  }

  if (getenv("PEDDIR") == NULL)
    peddir = Form("%s/%s", myWorkDir, "ped");
  else
    peddir = getenv("PEDDIR");

  if(phys_h){
    ped_file = TFile::Open(Form("%s/datafile_histo_pedestal_run%d.root",
				ntdir, Nrunnumber));
    if(ped_file==0){
      ped_file = TFile::Open(Form("%s/datafile_histo_pedestal_run0.root",
				  ntdir));
    } 
    printf ("using ped_file %s\n", ped_file->GetName());   
  } else {
    ped_file=0;
  }

  char histo[50],name[50];

  //Histograms for the 2 Dream ADC toghether
  for(int i_dreamadc=0; i_dreamadc<2; i_dreamadc++){
    char adcname[10];
    if(i_dreamadc==0)sprintf(adcname,"s");
    elses printf(adcname,"q");
    for(int ch=0;ch<32;ch++){
      sprintf(histo,"Dream_ADC%d_ch%d", i_dreamadc,ch);
      sprintf(name,"Dream (ADC%d ch%d)", i_dreamadc,ch);
      hadc_dream_debug[i_dreamadc][ch] = new TH1F (histo,name, 4196,-100.5,4095.5);

      hadc_dream[i_dreamadc][ch]=0;
      if(dream_adc_to_ch[ch]>=0){
	sprintf(histo,"%s_%d",adcname,dream_adc_to_ch[ch]);
        hadc_dream[i_dreamadc][ch] = new TH1F (histo,histo, 4196,-100.5,4095.5);        
      }
//       if(ch>0 && ch<16) {
//         if(dream_adc_to_ch[ch]>=0)
// 	sprintf(histo,"%s_%d",adcname,ch);
// 	hadc_dream[i_dreamadc][ch] = new TH1F (histo,histo, 4196,-100.5,4095.5);
//       }else if (ch>16 && ch<22) { 
// 	sprintf(histo,"%s_%d",adcname,ch-1);
// 	hadc_dream[i_dreamadc][ch] = new TH1F (histo,histo, 4196,-100.5,4095.5);
//       } 

      hadc_newdream[i_dreamadc][ch]=0;
      if(newdream_adc_to_ch[ch]>=0){
      	sprintf(histo,"%s_newdream_%d",adcname,newdream_adc_to_ch[ch]);
	hadc_newdream[i_dreamadc][ch] = new TH1F (histo,histo, 4196,-100.5,4095.5);
      }
//       if(ch>0 && ch<5) {
// 	sprintf(histo,"%s_newdream_%d",adcname,ch);
// 	hadc_newdream[i_dreamadc][ch] = new TH1F (histo,histo, 4196,-100.5,4095.5);
//       }

//       hadc_dream[i_dreamadc][ch] = 0;//new TH1F (histo,name, 4196,-100.5,4095.5);
//       string name = dream_ch_to_name[i_dreamadc][ch].first;
//       string title = dream_ch_to_name[i_dreamadc][ch].second;
//       if(name!="")
// 	hadc_dream[i_dreamadc][ch]=new TH1F (name.c_str(),title.c_str(), 4196,-100.5,4095.5);
    }
    sprintf(histo,"%s_tot",adcname);
    TH1F* hadc_dream_tot[i_dreamadc] = new TH1F(histo,histo, 8196,-100.5,8095.5);

    sprintf(histo,"%s_newdream_tot",adcname);
    TH1F* hadc_newdream_tot[i_dreamadc] = new TH1F(histo,histo, 8196,-100.5,8095.5);
    
    sprintf(histo,"Dream_ADC%d_map",i_dreamadc);
    hadc_dream_map[i_dreamadc] = 
      new TH2F(histo,histo,32,0,32,4196,-100.5,4095.5);
    sprintf(histo,"Dream_%s_baricenter_x",adcname);
    hadc_dream_baricenter_x[i_dreamadc] = 
      new TH1F(histo,histo,100,-2,2);
    sprintf(histo,"Dream_%s_baricenter_y",adcname);
    hadc_dream_baricenter_y[i_dreamadc] = 
      new TH1F(histo,histo,100,-2,2);
    sprintf(histo,"Dream_%s_baricenter",adcname);
    hadc_dream_baricenter[i_dreamadc] = 
      new TH2F(histo,histo,100,-2,2,100,-2,2);

    sprintf(histo,"Newdream_ADC%d_map",i_dreamadc);
    hadc_newdream_map[i_dreamadc] = 
      new TH2F(histo,histo,32,0,32,4196,-100.5,4095.5);
    sprintf(histo,"Newdream_%s_baricenter_x",adcname);
    hadc_newdream_baricenter_x[i_dreamadc] = 
      new TH1F(histo,histo,100,-2,2);
    sprintf(histo,"Newdream_%s_baricenter_y",adcname);
    hadc_newdream_baricenter_y[i_dreamadc] = 
      new TH1F(histo,histo,100,-2,2);
    sprintf(histo,"Newdream_%s_baricenter",adcname);
    hadc_newdream_baricenter[i_dreamadc] = 
      new TH2F(histo,histo,100,-2,2,100,-2,2);

  }

//   //Muon ADC histograms
//   for(int ch=0;ch<32;ch++){
//     sprintf(histo,"Muon_ADC0_ch%d",ch);
//     sprintf(name,"Muon (ADC0 ch%d)",ch);
//     hadc_muons_debug[ch] = new TH1F (histo,name, 4196,-100.5,4095.5);
//     hadc_muons[ch]=0;
//     if(ch == 0) 
//       hadc_muons[ch] = new TH1F ("Muon_veto","Muon veto", 4196,-100.5,4095.5);
//   }
  
//   for (int i_tdc_ch=0;i_tdc_ch<16;i_tdc_ch++) {
//     sprintf(histo,"TDC_ch%d",i_tdc_ch);  
//     sprintf(name,"TDC ch %d",i_tdc_ch);  
//     htdc_debug[i_tdc_ch]=new TH1F (histo,name,4096,-0.5,4095.5);
//   }
//   for ( int i_dwc_ch=0;i_dwc_ch<4;i_dwc_ch++) {
//     sprintf(histo,"DWC_ch%d",i_dwc_ch+1);  
//     sprintf(name,"DWC ch %d",i_dwc_ch+1);  
//     hdwc[i_dwc_ch] = new TH1F (histo,name,4096,-0.5,4095.5);
//   }
  
//   h_dwc_x = new TH1F ("DWC_x","DWC x",500,-250.5,249.5);
//   h_dwc_y = new TH1F ("DWC_y","DWC y",500,-250.5,249.5);
//   h_dwc_xy = new TH2F ("DWC_xy","DWC xy",500,-250.5,249.5,500,-250.5,249.5);
  
  
//   for(int i_osc=0;i_osc < N_CH_OSC; i_osc++){
//     //osc stuff
    
//   }

}

int dreammon::event(unsigned int evt, unsigned int * buf){
  int rc = 0; //   

  EventHeader * head=(EventHeader *) buf;
  
  if(head->evmark!=0xCAFECAFE){
    printf( "Cannot find the event marker. Something is wrong.\n" );
    return 0;
  }

  int i=head->evhsiz/sizeof(unsigned int); 
  //FIXME i is the point where the data starts, change the name!

  bool fill_1ev_histos = (evt%100==0) & (1ev_counter<N_1EV_H);

  myADCN adcnData;
//   myTEKOSC tekoscData;
//   myTDC tdcData;
  
  //debug histogram with all channels
  for(int i_dreamadc=0; i_dreamadc<2; i_dreamadc++){
    int adc_id = DREAM_ADC_BASE_ADDR + 
      0x01000000 * i_dreamadc;
    addr =  SubEventSeek2(adc_id, &buf[i],sz);
    rc += DecodeV792AC(addr, &adcnData);
    float dream_tot = 0, newdream_tot=0;
    float dream_weight_x = 0, newdream_weight_x=0, 
      dream_weight_y = 0, newdream_weight_y=0;
    for (j=0; j<adcnData.index; j++) {
      int ch = adcnData.channel[j];
      int data = adcnData.data[j];
      int data_ped = data - 
        get_pedestal(hadc_dream_debug[i_dreamadc][ch]);
      hadc_dream_debug[i_dreamadc][ch]->Fill(data);
      if(hadc_dream[i_dreamadc][ch]!=0){
        hadc_dream[i_dreamadc][ch]->Fill(data);
        dream_tot += data_ped;
        dream_weight_x += data_ped*dream_adc_to_x[ch];
        dream_weight_y += data_ped*dream_adc_to_y[ch];
      }
      if(hadc_newdream[i_dreamadc][ch]!=0){
        hadc_newdream[i_dreamadc][ch]->Fill(data);
        newdream_tot += data_ped;
        newdream_weight_x += data_ped*newdream_adc_to_x[ch];
        newdream_weight_y += data_ped*newdream_adc_to_y[ch];
      }
      hadc_dream_map[i_dreamadc]->Fill(ch,data);
    }
    hadc_dream_tot[i_dreamadc]->Fill(dream_tot);
    hadc_newdream_tot[i_dreamadc]->Fill(newdream_tot);
    float dream_x = dream_weight_x/dream_tot;
    float dream_y = dream_weight_y/dream_tot;
    hadc_dream_baricenter_x[i_dreamadc]->Fill(dream_x);
    hadc_dream_baricenter_y[i_dreamadc]->Fill(dream_y);
    hadc_dream_baricenter[i_dreamadc]->Fill(dream_x,dream_y);

    float newdream_x = newdream_weight_x/newdream_tot;
    float newdream_y = newdream_weight_y/newdream_tot;
    hadc_newdream_baricenter_x[i_newdreamadc]->Fill(newdream_x);
    hadc_newdream_baricenter_y[i_newdreamadc]->Fill(newdream_y);
    hadc_newdream_baricenter[i_newdreamadc]->Fill(newdream_x,newdream_y);
  }

  //FIXME other stuff


  if(fill_1ev_histos) 1ev_counter++; //must be at the end
  return rc;
}

int dreammon::exit(){
  TFile hfile(ntfilename,"RECREATE");
  if (hfile.IsZombie()) {
    fprintf(stderr, "Cannot open  file %s\n", ntfilename);
    return -1;
  }
  printf("%s\n",ntfilename);
  
  hfile.mkdir("Dream_Debug");
  hfile.cd("Dream_Debug");
  //Save debug histos for the 2 Dream ADCs
  for(int i_dreamadc=0; i_dreamadc<2; i_dreamadc++){
    for(int ch=0;ch<32;ch++){
      hadc_dream_debug[i_dreamadc][ch]->Write();
    }
  }

  hfile.cd("");
  hfile.mkdir("Dream");
  hfile.cd("Dream");
    //Histograms for the 2 Dream ADC toghether
  for(int i_dreamadc=0; i_dreamadc<2; i_dreamadc++){
    for(int ch=0;ch<32;ch++){
      if(hadc_dream[i_dreamadc][ch]!=0)
        hadc_dream[i_dreamadc][ch]->Write();
      if(hadc_newdream[i_dreamadc][ch]!=0)
        hadc_newdream[i_dreamadc][ch]->Write();
    }
    hadc_dream_tot[i_dreamadc]->Write();
    hadc_newdream_tot[i_dreamadc]->Write();
    
    hadc_dream_map[i_dreamadc]->Write(); 
    hadc_dream_baricenter_x[i_dreamadc]->Write(); 
    hadc_dream_baricenter_y[i_dreamadc]->Write(); 
    hadc_dream_baricenter[i_dreamadc]->Write(); 

    hadc_newdream_map[i_dreamadc]->Write(); 
    hadc_newdream_baricenter_x[i_dreamadc]->Write(); 
    hadc_newdream_baricenter_y[i_dreamadc]->Write(); 
    hadc_newdream_baricenter[i_dreamadc]->Write(); 
  }
  hfile.Close(); 
  return 0;

}

float dreammon::get_pedestal(TH1F* h){
  if(ped_file==0 || ped_file->IsZombie() || h==0)
    return 0.;
  //printf("ped file exists\n");
 
  static std::map<std::string, float> cache;

  std::string name = Form("%s",h->GetName());
  //printf("h_ped: %s\n",name.c_str());
  float ped = ped_cache[name];

  if(ped == 0.){ //not in cache
    //printf("h_ped: %s\n",name.c_str());
    //printf("not in cache\n");
    //printf("ped_file: %s\n",ped_file->GetName());
    TH1F* h_ped = 0;
    ped_file->GetObject(name.c_str(),h_ped);
    if (h_ped==0 ){
      name = Form("Debug/%s",h->GetName());
      ped_file->GetObject(name.c_str(),h_ped);
    }
    if (h_ped==0 ) {
	//printf("not found\n");
	ped = 0;
	cache[name] = -10000.;
//     } else if(h_ped->GetEntries()<10){ 
//       printf("not enough stat\n"); 
//       ped = 0; 
//       cache[name] = -10000.; 
    } else {
      //printf("found!\n");
      ped = h_ped->GetMean();
      cache[name] = ped;
    } 
  } else if(ped == - 10000.){ //we already know it's not there
    //printf("we already know it's missing!\n");
    ped = 0;
  }
  //printf("ped: %f\n",ped);
  return ped;
}
