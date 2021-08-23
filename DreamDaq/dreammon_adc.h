#ifndef _DREAMMON_ADC_H_
#define _DREAMMON_ADC_H_

#include "myhbook.h"
#include "mapping.h"

void adc_init(){

  char histo[20];
  char name[50];
  ////////////////////////////// Debug Histograms 

  for(int32_t i_adc=0; i_adc<N_ADC; i_adc++){
    for(int32_t ch=0;ch<32;ch++){
      sprintf(histo,"ADC%d_ch%02d", i_adc,ch);
      sprintf(name,"Debug (ADC%d ch%02d);ADC counts;events", i_adc,ch);
      hadc_dbg[i_adc][ch] = new TH1F (histo,name, BIN_ADC,MIN_ADC, MAX_ADC);
    }
    sprintf(histo,"Debug_ADC%d_map",i_adc);
    sprintf(name,"Debug (ADC%d);channel;average ADC counts", i_adc);
    hadc_dbg_map[i_adc] = 
      new TH2F(histo,histo,32,0,32,BIN_ADC,MIN_ADC, MAX_ADC);
  }
  ////////////////////////////// New Histograms 

  /* for(int32_t i_adc=N_ADC; i_adc<N_ADC; i_adc++){ */
  /*   for(int32_t ch=0;ch<8;ch++){ */
  /*     sprintf(histo,"ADC%d_ch%02d", i_adc,ch); */
  /*     sprintf(name,"Debug (ADC%d ch%02d);ADC counts;events", i_adc,ch); */
  /*     hadc_dbg[i_adc][ch] = new TH1F (histo,name, BIN_ADC,MIN_ADC, MAX_ADC); */
  /*   } */
  /*   sprintf(histo,"Debug_ADC%d_map",i_adc); */
  /*   sprintf(name,"Debug (ADC%d);channel;average ADC counts", i_adc); */
  /*   hadc_dbg_map[i_adc] =  */
  /*     new TH2F(histo,histo,8,0,8,BIN_ADC,MIN_ADC, MAX_ADC); */
  /* } */
  ////////////////////////////// New Histograms 

  //Histograms for newDREAM
  for(int32_t i_amp=0; i_amp<N_GAINS; i_amp++){
    for(int32_t i_fiber=0; i_fiber<N_FIBERS; i_fiber++){
      char c_fiber = (i_fiber==0)? 'S' : 'C';
      //char c_amp = (i_amp==0)? 'L' : 'H';
      sprintf(histo,"newdream_%c_tot",c_fiber);
      hadc_newdream_tot[i_fiber][i_amp] = new TH1F(histo,histo, BIN_ADC,MIN_ADC,MAX_ADC);
      hadc_newdream_tot[i_fiber][i_amp]->SetBit(TH1::kCanRebin);
      sprintf(histo,"newdream_%c_emap",c_fiber);
      hadc_newdream_emap[i_fiber][i_amp] = new TH2F(histo,histo,100,0,7,100,0,7);
      sprintf(histo,"Cu_dream_%c_tot",c_fiber);
      hadc_Cu_dream_tot[i_fiber][i_amp] = new TH1F(histo,histo, BIN_ADC,MIN_ADC,MAX_ADC);
      hadc_Cu_dream_tot[i_fiber][i_amp]->SetBit(TH1::kCanRebin);
      for(int32_t ring=1; ring<N_RINGS; ring++){
	sprintf(histo,"Digital Ring%d-%c_tot",ring,c_fiber);
	hadc_newdream_ring_tot[ring][i_fiber][i_amp] = new TH1F(histo,histo, BIN_ADC,MIN_ADC,MAX_ADC);
	hadc_newdream_ring_tot[ring][i_fiber][i_amp]->SetBit(TH1::kCanRebin);
      }
    }
  }
 

  // Total Leakage 
  sprintf(histo,"Leakage_tot");
  hadc_leakage_tot = new TH1F(histo,histo, 8196,-100.5,8095.5);
  hadc_leakage_tot->SetBit(TH1::kCanRebin);
}

void adc_event(uint32_t* buf, uint32_t i, uint32_t sz){
  myADCN adcnData;

  ////////////////////////////// Fill Debug Histograms 

 float tot_leakage=0;
 float Cu_dream_tot[N_FIBERS][N_GAINS];
 float dream_tot[N_FIBERS][N_GAINS];
 float dream_x[N_FIBERS][N_GAINS];
 float dream_y[N_FIBERS][N_GAINS];
 float dream_ring_tot[N_RINGS][N_FIBERS][N_GAINS];
 
 for(int32_t i_fiber=0; i_fiber<N_FIBERS; i_fiber++){
   for(int32_t i_amp=0; i_amp<N_GAINS; i_amp++){
     dream_tot[i_fiber][i_amp] = 0;
     dream_x[i_fiber][i_amp] = 0;
     dream_y[i_fiber][i_amp] = 0;
     Cu_dream_tot[i_fiber][i_amp] = 0;
     for(int32_t ring=1; ring<N_RINGS; ring++){
       dream_ring_tot[ring][i_fiber][i_amp] = 0;
     }
   }
 }

 for(int32_t i_adc=0; i_adc<N_ADC; i_adc++){
   uint32_t adc_id = adc_num_to_vme_addr[i_adc];
   uint32_t* addr =  SubEventSeek2(adc_id, &buf[i],sz);

    //should use DecodeV862 for ADC3 but the two functions are actually the same
    DecodeV792AC(addr, &adcnData);

    for (uint32_t j=0; j<adcnData.index; j++) {
      int32_t ch = adcnData.channel[j];
      int32_t data = adcnData.data[j];

      hadc_dbg[i_adc][ch]->Fill(data);
      hadc_dbg_map[i_adc]->Fill(ch,data);
      if(adc_addr_to_tower[i_adc][ch]>0){ //tower==0 is reserved for the adders
	int32_t fiber = adc_addr_to_fiber[i_adc][ch];
	int32_t amp = adc_addr_to_amp[i_adc][ch];
	int32_t tower = adc_addr_to_tower[i_adc][ch];
	int32_t ring = tower_to_ring[tower];
	dream_tot[fiber][amp] += data;
	dream_x[fiber][amp] += data*get_x(i_adc,ch);
	dream_y[fiber][amp] += data*get_y(i_adc,ch);
	dream_ring_tot[ring][fiber][amp] += data;
      }
      if(adc_addr_to_leakage[i_adc][ch]>=0)tot_leakage +=data;

      if(adc_addr_to_Cu_dream[i_adc][ch]>=0){
      int32_t fiber = adc_addr_to_Cu_fiber[i_adc][ch];
      int32_t amp = adc_addr_to_Cu_amp[i_adc][ch];
	Cu_dream_tot[fiber][amp]+=data;
      }
    }
  }

 hadc_leakage_tot->Fill(tot_leakage);
 for(int32_t i_amp=0; i_amp<N_GAINS; i_amp++){
   for(int32_t i_fiber=0; i_fiber<N_FIBERS; i_fiber++){
     hadc_newdream_tot[i_fiber][i_amp]->Fill(dream_tot[i_fiber][i_amp]);
     hadc_newdream_emap[i_fiber][i_amp]->Fill(dream_x[i_fiber][i_amp]/dream_tot[i_fiber][i_amp],
					      dream_y[i_fiber][i_amp]/dream_tot[i_fiber][i_amp]);
     hadc_Cu_dream_tot[i_fiber][i_amp]->Fill(Cu_dream_tot[i_fiber][i_amp]);
     for(int32_t ring=1; ring<N_RINGS; ring++){
       hadc_newdream_ring_tot[ring][i_fiber][i_amp]->Fill(dream_ring_tot[ring][i_fiber][i_amp]);
     }
   }
 }

}

void adc_sync(TFile *file, TDirectory *dbg_dir){

  //Save Histo for NewDream  ADC(s)
  file->mkdir("NewDream");
  file->cd("NewDream");

  for(int32_t i_adc=0; i_adc<N_ADC; i_adc++){
    for(int32_t ch=0;ch<32;ch++){
      if(adc_addr_to_tower[i_adc][ch]>=0){
	TH1F* h = new TH1F(*hadc_dbg[i_adc][ch]);
	h->SetNameTitle(adc_addr_to_name[i_adc][ch],adc_addr_to_name[i_adc][ch]);
	h->Write();
	delete h;
      }
    }
  }
  for(int32_t i_amp=0; i_amp<N_GAINS; i_amp++){
    for(int32_t i_fiber=0; i_fiber<N_FIBERS; i_fiber++){
      hadc_newdream_tot[i_fiber][i_amp]->Write(); 
      hadc_newdream_emap[i_fiber][i_amp]->Write(); 
      for(int32_t ring=1; ring<N_RINGS; ring++){
	hadc_newdream_ring_tot[ring][i_fiber][i_amp]->Write();
      }
    }
  }

  //Save Histo for Crystals  ADC
/*   file->mkdir("Crystals"); */
/*   file->cd("Crystals"); */
/*   for(int32_t i_adc=0; i_adc<N_ADC; i_adc++){ */
/*     for(int32_t ch=0;ch<32;ch++){ */
/*       if(adc_addr_to_xtal[i_adc][ch]>=0){ */
/* 	TH1F* h = new TH1F(*hadc_dbg[i_adc][ch]); */
/* 	h->SetNameTitle(adc_addr_to_name[i_adc][ch],adc_addr_to_name[i_adc][ch]); */
/* 	h->Write(); */
/* 	delete h; */
/*       } */
/*     } */
/*   } */

  //Save Histo for Ancillary  ADC
  file->mkdir("Ancillary");
  file->cd("Ancillary");
  for(int32_t i_adc=0; i_adc<N_ADC; i_adc++){
    for(int32_t ch=0;ch<32;ch++){
      if(adc_addr_to_ancillary[i_adc][ch]>=0){
	TH1F* h = new TH1F(*hadc_dbg[i_adc][ch]);
	h->SetNameTitle(adc_addr_to_name[i_adc][ch],adc_addr_to_name[i_adc][ch]);
	h->Write();
	delete h;
      }
    }
  }
  //Save Histo for Cu_Dream 
  file->mkdir("Cu_Dream");
  file->cd("Cu_Dream");
  for(int32_t i_adc=0; i_adc<N_ADC; i_adc++){
    for(int32_t ch=0;ch<32;ch++){
      if(adc_addr_to_Cu_dream[i_adc][ch]>=0){
	TH1F* h = new TH1F(*hadc_dbg[i_adc][ch]);
	h->SetNameTitle(adc_addr_to_name[i_adc][ch],adc_addr_to_name[i_adc][ch]);
	h->Write();
	delete h;
      }
    }
  }
  for(int32_t i_amp=0; i_amp<N_GAINS; i_amp++){
    for(int32_t i_fiber=0; i_fiber<N_FIBERS; i_fiber++){
      hadc_Cu_dream_tot[i_fiber][i_amp]->Write(); 
    }
  }

  //Save Histo for  Leakage 
  file->mkdir("Leakage");
  file->cd("Leakage");
  for(int32_t i_adc=0; i_adc<N_ADC; i_adc++){
    for(int32_t ch=0;ch<32;ch++){
      if(adc_addr_to_leakage[i_adc][ch]>=0){
	TH1F* h = new TH1F(*hadc_dbg[i_adc][ch]);
	h->SetNameTitle(adc_addr_to_name[i_adc][ch],adc_addr_to_name[i_adc][ch]);
	h->Write();
	delete h;
      }
    }
  }
  hadc_leakage_tot->Write();

  //debug plots in the Debug directory

  dbg_dir->cd();

  // Other Debug Histograms 
  for(int32_t i_adc=0; i_adc<N_ADC; i_adc++){
    dbg_dir->mkdir(adc_num_to_name[i_adc]);
    dbg_dir->cd(adc_num_to_name[i_adc]);
    
    TProfile *hsummary = hadc_dbg_map[i_adc]->ProfileX("Mean_summary");
    TH1F *hsummary_rms = new TH1F("RMS_summary","RMS_summary", 32, 0, 32);

    for(int32_t ch=0;ch<32;ch++){
      hadc_dbg[i_adc][ch]->Write();

      //hsummary->GetXaxis()->SetBinLabel(ch+1,adc_addr_to_name[i_adc][ch]);
      //hsummary_rms->GetXaxis()->SetBinLabel(ch+1,adc_addr_to_name[i_adc][ch]);
      hsummary_rms->SetBinContent(ch+1,hadc_dbg[i_adc][ch]->GetRMS());
      hsummary_rms->SetBinError(ch+1,hadc_dbg[i_adc][ch]->GetRMSError());
    }
    hsummary->GetXaxis()->SetLabelSize(0.03);
    hsummary_rms->GetXaxis()->SetLabelSize(0.03);
    hadc_dbg_map[i_adc]->Write();
    hsummary->Write();
    hsummary_rms->Write();
    delete hsummary_rms;
  }
}

#endif // _DREAMMON_ADC_H_
