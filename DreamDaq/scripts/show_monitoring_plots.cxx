
TFile *file = 0;

void show_c_s_plots(int run){
  
  TH1F* h_s , *h_c;
  TH1F* h_si , *h_ci;

  if(file->IsZombie()) return;

  file->GetObject("time_str_osc1_mean_v",h_c);
  file->GetObject("time_str_osc2_mean_v",h_s);

  file->GetObject("int_osc1_mean_v",h_ci);
  file->GetObject("int_osc2_mean_v",h_si);

  TCanvas *c = new TCanvas("c_s_c","Cherenkov and Scintillator plots",1280,960);
  c->Divide(2,2);
  
  c->cd(1);
  if(h_c!=0) h_c->Draw();
  c->cd(2)->SetLogy();
  if(h_c!=0) h_ci->Draw();
  c->cd(3);
  if(h_s!=0) h_s->Draw();
  c->cd(4)->SetLogy();
  if(h_s!=0) h_si->Draw();

}

void show_osc_plots(int run){
  
  TH2F* h[4];
  TH1F *hi[4];
  if(file->IsZombie()) return;

  TCanvas *c = new TCanvas("c_osc","Oscilloscope plots",1280,960);
  TCanvas *ci = new TCanvas("c_osc_i","Oscilloscope plotsintegrals",1280,960);
  c->Divide(2,2);
  ci->Divide(2,2);

  for(int i=0; i<4;i++){
    h[i]=(TH2F*)file->Get(Form("Oscilloscope/OSC_ch%d_map",i));
    hi[i]=(TH1F*)file->Get(Form("Oscilloscope/OSC_ch%d_integral",i));
    
    c->cd(i+1);
    if(h[i]!=0)h[i]->Draw();
    ci->cd(i+1);
    if(hi[i]!=0)hi[i]->Draw();
  }
}

void show_q715_plots(int run){
  
  TH1F* h_1 , *h_5, *h_7;

  if(file->IsZombie()) return;

  file->GetObject("q_1",h_1);
  file->GetObject("q_5",h_5);
  file->GetObject("q_7",h_7);

  TCanvas *c = new TCanvas("c_s_c","",1600,600);
  c->Divide(3,1);
  
  c->cd(1);
  if(h_7!=0) h_7->Draw();
  c->cd(2);
  if(h_1!=0) h_1->Draw();
  c->cd(3);
  if(h_5!=0) h_5->Draw();

}

void show_dwc_plots(int run){
  
  TH1F* h , *h_cut;

  if(file->IsZombie()) return;



  h = (TH1F*)file->Get("DWC/DWC_xy");
  h_cut = (TH1F*)file->Get("DWC/DWC_xy_cut");

  TCanvas *c = new TCanvas("c_dwc","",1600,600);
  c->Divide(2,1);
  
  c->cd(1);
  if(h!=0) h->Draw();
  c->cd(2);
  if(h_cut!=0) h_cut->Draw();
}


void show_q316_plots(int run){
  
  TH1F* h_1 , *h_3, *h_6;

  if(file->IsZombie()) return;

  file->GetObject("q_1",h_1);
  file->GetObject("q_3",h_3);
  file->GetObject("q_6",h_6);

  TCanvas *c = new TCanvas("c_s_c","",1600,600);
  c->Divide(3,1);
  
  c->cd(1);
  if(h_3!=0) h_3->Draw();
  c->cd(2);
  if(h_1!=0) h_1->Draw();
  c->cd(3);
  if(h_6!=0) h_6->Draw();

}

void show_q_plots(int run){
  
  TH1F* h[19];

  if(file->IsZombie()) return;

  TCanvas *c = new TCanvas("c_all_q","All Q channels",800,600);
  c->Divide(5,4);

  int i_max =-1;
  float max = -1;
  for(int i =0; i < 19; i++){
    file->GetObject(Form("q_%d",i+1),h[i]);
    c->cd(i+1);
    if(h[i]!=0) {
      h[i]->Draw();
      if(h[i]->GetMean()>max) {i_max = i; max=h[i]->GetMean();}
    }
  }
  TCanvas *c_max = new TCanvas("c_max_q","");
  if(h[i_max]!=0) h[i_max]->Draw();
}

void show_s_plots(int run){
  
  TH1F* h[19];

  if(file->IsZombie()) return;

  TCanvas *c = new TCanvas("c_all_s","All S channels",800,600);
  c->Divide(5,4);

  int i_max =-1;
  float max = -1;
  for(int i =0; i < 19; i++){
    file->GetObject(Form("s_%d",i+1),h[i]);
    c->cd(i+1);
    if(h[i]!=0) {
      h[i]->Draw();
      if(h[i]->GetMean()>max) {i_max = i; max=h[i]->GetMean();}
    }
  }
  TCanvas *c_max = new TCanvas("c_max_s","");
  if(h[i_max]!=0) h[i_max]->Draw();
}

void show_monitoring_plots_tmp(int run, TString args){

  file = TFile::Open(Form("/home/dreamtest/working/hbook/datafile_histo_run%d.root",run));

  args.ToLower();
  if(args.Contains("help")){
    cout << "usage: show_monitoring_plot <run_number> <options>" << endl<< endl
	 << "available options: " << endl
	 << "help all_s all_q xtals osc dwc browser" << endl;
    gSystem->Exit(1);
  }
  if(args.Contains(Form("%d",run)) ){
    cout << "usage: show_monitoring_plot <run_number> <options>" << endl<< endl
	 << "available options: " << endl
	 << "help all_s all_q xtals osc dwc browser" << endl;
    new TBrowser();
  }
  //show_q316_plots(run);
  //show_q715_plots(run);
  //show_c_s_plots(run);
  if(args.Contains("all_s"))show_s_plots(run);
  if(args.Contains("all_q"))show_q_plots(run);
  if(args.Contains("xtals"))show_c_s_plots(run);
  if(args.Contains("osc"))show_osc_plots(run);
  if(args.Contains("dwc"))show_dwc_plots(run);
  if(args.Contains("browser")) new TBrowser();
}

void show_monitoring_plots(int run, TString args){
  show_monitoring_plots_tmp(run, args);
}
