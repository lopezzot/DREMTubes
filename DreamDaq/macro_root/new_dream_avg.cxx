void new_dream_avg(TString runfile, int ch, float *res = 0){
  TFile *f = TFile::Open(runfile);
  
  float SAM = 2; //sigmas_around_mean
  TH1F * h;
  TF1 *g = new TF1("g","gaus");
  TString histo= Form("NewDream/S%d",ch);
    cout<<"Histo S"<<histo<<endl;
  f->GetObject(histo,h);

  new TCanvas("S","S",12,10,700,500);
//  h->GetXaxis()->SetRange(1400,2100);
  h->GetXaxis()->SetRange(0,4000);

  h->Fit(g,"q","",1500,3000);
  float muS = g->GetParameter(1);
  float sigmaS = g->GetParameter(2);
  //float muS = h->GetMean();
  //float sigmaS = h->GetRMS();
  //float sigmaS = 300.;
  //cout<<" S: Mean "<<muS <<" RMS "<<sigmaS<<" Range"<<muS-SAM*sigmaS << " - "<<muS+SAM*sigmaS <<endl; 
  h->Fit(g,"q","",(muS-SAM*sigmaS),(muS+SAM*sigmaS));
  muS = g->GetParameter(1);
  sigmaS = g->GetParameter(2);
  h->Fit(g,"q","",muS-SAM*sigmaS,muS+SAM*sigmaS);
  cout<<" S: Mean "<<muS <<" RMS "<<sigmaS<<" Range"<<muS-SAM*sigmaS << " - "<<muS+SAM*sigmaS <<endl;
  muS = g->GetParameter(1);
  sigmaS = g->GetParameter(2);
//  float pedS = 897;
 TFile *fp=TFile::Open("~/storage/hbook/datafile_histo_pedestal_run10539.root");
 TH1F * hp;
 fp->GetObject(histo,hp); 
 float pedS = hp->GetMean(); 

  f->cd();
   TH1F * hC;
  new TCanvas("C","C",714,10,700,500);
//   TString histoC="NewDream/C"+ch;
   TString histoC= Form("NewDream/C%d",ch);
  f->GetObject(histoC,hC);
 // h->GetXaxis()->SetRange(1400,2100);
  hC->GetXaxis()->SetRange(0,4000);
  hC->Draw();
  hC->Fit(g,"q","",500,1000);
  float muC = g->GetParameter(1);
  float sigmaC = g->GetParameter(2);
//   float muC = h->GetMean();
//  float sigmaC = h->GetRMS();
  hC->Fit(g,"q","",muC-SAM*sigmaC,muC+SAM*sigmaC);
  muC = g->GetParameter(1);
  sigmaC = g->GetParameter(2);
  hC->Fit(g,"q","",muC-SAM*sigmaC,muC+SAM*sigmaC);
  muC = g->GetParameter(1);
  sigmaC = g->GetParameter(2);
 // float pedC = 869;
   TFile *fp=TFile::Open("~/storage/hbook/datafile_histo_pedestal_run10539.root");
 TH1F * hp;
 fp->GetObject(histoC,hp);

 float pedC = hp->GetMean();
  cout<< " RunS "<< runfile <<" Channel "<<ch<<endl;
  cout << std::setprecision(4)
       << "S = " << muS-pedS 
       << " +- " << sigmaS 
       << " ("<< sigmaS/(muS-pedS)*100 << "%)\n";
  cout<<"PedS "<<pedS<<endl;

  cout << "C = " << muC-pedC 
       << " +- " << sigmaC 
       << " (" << sigmaC/ (muC-pedC)*100 << "%)\n";
  cout<<"PedC "<<pedC<<endl;
 
  if (res!=0) {
    res[0]= sigmaS/(muS-pedS)*100;
    res[1]= sigmaC/(muC-pedC)*100;
    res[2]= muS;
    res[3]= sigmaS;
    res[4]= muC;
    res[5]= sigmaC;
  }
}
void new_dream_avg(int run, int ch, float *res = 0){
  new_dream_avg(Form("~/storage/hbook/datafile_histo_run%d.root",run),ch,res);
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
