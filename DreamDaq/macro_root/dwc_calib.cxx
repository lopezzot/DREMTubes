

dwc_calib(TString run_lu, TString run_c, TString run_rd, TString hprefix="Debug/DWC1_"){

  TString runs[3]={run_lu, run_c, run_rd};
  Float_t ud[3];
  Float_t ud_e[3];
  Float_t rl[3];
  Float_t rl_e[3];
  Float_t x[3]={-30, 0, 30};
  Float_t mx[3]={30, 0, -30};
  Float_t x_e[3]={1, 1, 1};

  for (int i_run = 0; i_run<3; i_run++){
    TFile *f= TFile::Open(runs[i_run],"read");
    TH1F *hud;
    TH1F *hrl;
    f->GetObject(hprefix+"ud",hud);
    f->GetObject(hprefix+"rl",hrl);
    ud[i_run] = hud->GetMean();
    ud_e[i_run] = hud->GetMeanError();
    rl[i_run] = hrl->GetMean();
    rl_e[i_run] = hrl->GetMeanError();
    cout << i_run << ") ud: " << ud[i_run] << " rl: "<< rl[i_run]<< endl;
  }

  TGraphErrors *g_ud=new TGraphErrors(3,ud,x,ud_e,x_e);
  TGraphErrors *g_rl=new TGraphErrors(3,rl,x,rl_e,x_e);
  
  g_ud->SetNameTitle("gud","Up - Down;Up - Down (ms);x pos (mm)");
  g_rl->SetNameTitle("grl","Left - Right;Left - Right (ms);x pos (mm)");

  g_ud->SetMarkerStyle(20);
  g_rl->SetMarkerStyle(21);

  TCanvas *c = new TCanvas(hprefix, hprefix, 1200,800);

  c->Divide(1,2);
  c->cd(1)->SetGridx();
  c->cd(1)->SetGridy();
  g_ud->Draw("alp");
  c->cd(2)->SetGridx();
  c->cd(2)->SetGridy();
  g_rl->Draw("alp");
  
  g_ud->Fit("pol1","Q");
  g_rl->Fit("pol1","Q");

  TF1 *f_ud = g_ud->GetFunction("pol1");
  TF1 *f_rl = g_rl->GetFunction("pol1");

  Float_t a = 60/(ud[2]-ud[0]);
  Float_t b = -30 - ud[0] * a ;

  cout << "Up - Down " << endl
       << "vSlope (ignoring 0,0): " <<a << endl
       << "vOffset (ignoring 0,0): " << b << endl;

  ah = 60/(rl[2]-rl[0]);
  bh = -30 - rl[0] * ah ;

  cout << "Left - Right " << endl
       << "hSlope (ignoring 0,0): " << ah << endl
       << "hOffset (ignoring 0,0): " << bh << endl << endl;
  cout
       << "float hSlope = " << f_rl->GetParameter(1) << "; // fit" << endl
       << "float hOffset = " << f_rl->GetParameter(0) << "; // fit" << endl
       << "float vSlope = " << f_ud->GetParameter(1) << "; // fit" << endl
       << "float vOffset = " << f_ud->GetParameter(0) << "; // fit"  << endl;

}
dwc_calib(int run_lu, int run_c, int run_rd, TString hprefix="Debug/DWC1_", TString wd="/home/dreamtest/storage/hbook/", TString fprefix="datafile_histo_run"){

  dwc_calib(wd+fprefix+TString::Itoa(run_lu,10)+".root",  
	    wd+fprefix+TString::Itoa(run_c,10)+".root",  
	    wd+fprefix+TString::Itoa(run_rd,10)+".root",  
	    hprefix);


}

dwc_calib(){

  cout << "dwc1" << endl;
  dwc_calib(9942,9943,9944);
  cout << endl << "dwc2" << endl;
  dwc_calib(9942,9943,9944,"Debug/DWC2_");

}
