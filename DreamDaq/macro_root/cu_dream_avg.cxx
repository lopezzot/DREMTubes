void cu_dream_avg(TString runfile, float *res = 0){
  TFile *f = TFile::Open(runfile);
  
  float SAM = 2; //sigmas_around_mean
  TH1F * h;
  TF1 *g = new TF1("g","gaus");
  f->GetObject("Cu_Dream/Cu_dream_S_tot",h);

  new TCanvas("S","S",12,10,700,500);
  h->GetXaxis()->SetRange(1400,2100);
  h->Fit(g,"q","",1500,2000);
  float muS = g->GetParameter(1);
  float sigmaS = g->GetParameter(2);
  h->Fit(g,"q","",muS-SAM*sigmaS,muS+SAM*sigmaS);
  muS = g->GetParameter(1);
  sigmaS = g->GetParameter(2);
  h->Fit(g,"q","",muS-SAM*sigmaS,muS+SAM*sigmaS);
  muS = g->GetParameter(1);
  sigmaS = g->GetParameter(2);
  float pedS = 897;

  new TCanvas("C","C",714,10,700,500);
  f->GetObject("Cu_Dream/Cu_dream_C_tot",h);
  h->GetXaxis()->SetRange(1400,2100);
  h->Fit(g,"q","",1500,2000);
  float muC = g->GetParameter(1);
  float sigmaC = g->GetParameter(2);
  h->Fit(g,"q","",muC-SAM*sigmaC,muC+SAM*sigmaC);
  muC = g->GetParameter(1);
  sigmaC = g->GetParameter(2);
  h->Fit(g,"q","",muC-SAM*sigmaC,muC+SAM*sigmaC);
  muC = g->GetParameter(1);
  sigmaC = g->GetParameter(2);
  float pedC = 869;

  cout << std::setprecision(3)
       << "<Stot> = " << muS-pedS 
       << " +- " << sigmaS 
       << " ("<< sigmaS/(muS-pedS)*100 << "%)\n";

  cout << "<Ctot> = " << muC-pedC 
       << " +- " << sigmaC 
       << " (" << sigmaC/ (muC-pedC)*100 << "%)\n";
 
  if (res!=0) {
    res[0]= sigmaS/(muS-pedS)*100;
    res[1]= sigmaC/(muC-pedC)*100;
    res[2]= muS;
    res[3]= sigmaS;
    res[4]= muC;
    res[5]= sigmaC;
  }
}
void cu_dream_avg(int run, float *res = 0){
  cu_dream_avg(Form("~/storage/hbook/datafile_histo_run%d.root",run),res);
}

void cu_dream_res(int run, int n_theta, float *thetas, TString type){

  TMultiGraph *mg = new TMultiGraph("mg","#theta scan ("+type+"); #theta [deg]; #sigma / #mu [%]");

  TGraph* gS = new TGraph();
  gS->SetNameTitle("gS","S");
  gS->SetMarkerStyle(20);
  gS->SetMarkerColor(kBlue);
  gS->SetFillColor(0);
  TGraph* gC = new TGraph();
  gC->SetNameTitle("gC","C");
  gC->SetMarkerStyle(21);
  gC->SetMarkerColor(kRed);
  gC->SetFillColor(0);

  mg->Add(gS);
  mg->Add(gC);

  for(int i=0; i<n_theta; i++){
    cout << "run: " << run+i << " theta: " << thetas[i] << endl;
    float res[20];
    cu_dream_avg(run+i,res);
    gS->SetPoint(i,thetas[i],res[0]);
    gC->SetPoint(i,thetas[i],res[1]);
  }
  TCanvas* c=new TCanvas("thetascan"+type,"res vs theta");
  c->SetGridx();c->SetGridy();
  mg->Draw("alp");
  TLegend *lg = c->BuildLegend(0.6);
  lg->SetFillStyle(0);
  lg->Draw();

}

void cu_dream_mu(int run, int n_theta, float *thetas, TString type){

  TMultiGraph *mg = new TMultiGraph("mg","#theta scan ("+type+"); #theta [deg]; #mu [ADC counts]");

  TGraph* gS = new TGraph();
  gS->SetNameTitle("gS","S");
  gS->SetMarkerStyle(20);
  gS->SetMarkerColor(kBlue);
  gS->SetFillColor(0);
  TGraph* gC = new TGraph();
  gC->SetNameTitle("gC","C");
  gC->SetMarkerStyle(21);
  gC->SetMarkerColor(kRed);
  gC->SetFillColor(0);

  mg->Add(gS);
  mg->Add(gC);

  for(int i=0; i<n_theta; i++){
    cout << "run: " << run+i << " theta: " << thetas[i] << endl;
    float res[20];
    cu_dream_avg(run+i,res);
    gS->SetPoint(i,thetas[i],res[2]);
    gC->SetPoint(i,thetas[i],res[4]);
  }
  TCanvas* c=new TCanvas("thetascan_mu_"+type,"mu vs theta");
  c->SetGridx();c->SetGridy();
  mg->Draw("alp");
  TLegend *lg = c->BuildLegend(0.6);
  lg->SetFillStyle(0);
  lg->Draw();

}

void cu_dream_PS(){
  const int n_theta = 23;
  float thetas[n_theta]={5,4,3,2.5,2,1.5,1,0.8,0.6,0.4,0.2,0,-0.2,-0.4,-0.6,-.8,-1,-1.5,-2,-2.5,-3,-4,-5};
  int run = 10100;
  cu_dream_res(run, n_theta, thetas, "with PS");
  cu_dream_mu(run, n_theta, thetas, "with PS");
}

void cu_dream_no_PS(){
  const int n_theta = 23;
  float thetas[30]={-5,-4,-3,-2.5,-2,-1.5,-1,-0.8,-0.6,-0.4,-0.2,0,0.2,0.4,0.6,.8,1,1.5,2,2.5,3,4,5};
  int run = 10124;
  cu_dream_res(run, n_theta, thetas, "no PS");
  cu_dream_mu(run, n_theta, thetas, "no PS");
}
