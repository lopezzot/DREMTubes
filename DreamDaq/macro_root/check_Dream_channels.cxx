check_Dream_channels(int run = 2873){
  
  TString fname = Form("/home/dreamtest/working/hbook/datafile_histo_run%d.root",run);

  TFile *f = new TFile(fname, "read");

  TH1F *q[32], *adc0[32], *s[32], *adc1[32], *qp[32], *sp[32];

  for(int ch=0; ch<32; ch++){
    f->GetObject(Form("q_%d",ch),q[ch]);  
    f->GetObject(Form("q_ped_%d",ch),qp[ch]); 
    f->GetObject(Form("Debug/Dream_ADC0_ch%d",ch),adc0[ch]);
  
    f->GetObject(Form("s_%d",ch),s[ch]);  
    f->GetObject(Form("s_ped_%d",ch),sp[ch]); 
    f->GetObject(Form("Debug/Dream_ADC1_ch%d",ch),adc1[ch]);  

    Float_t q_=-1, qp_=-1, s_=-1, sp_=-1, a0_=-1, a1_=-1;
    if(q[ch] != 0 ) q_ = q[ch]->GetMean();
    if(qp[ch] != 0 ) qp_ = qp[ch]->GetMean();
    if(s[ch] != 0 ) s_ = s[ch]->GetMean();
    if(sp[ch] != 0 ) sp_ = sp[ch]->GetMean();
    if(adc0[ch] != 0 ) a0_ = adc0[ch]->GetMean();
    if(adc1[ch] != 0 ) a1_ = adc1[ch]->GetMean();

//     cout << "q: " << q_
//          << " qp: " << qp_
//          << " a0: " << a0_;
//     if(fabs(q_-a0_)>0.1) 
//       cout << " ### ERRORE";
//     cout  << endl;
      
    cout  << "s: " << s_
          << " sp: " << sp_
          << " a1: " << a1_;

    if(fabs(s_-a1_)>0.1) 
      cout << " ### ERRORE";
    cout  << endl;
  }


  for(int ch=0; ch<32; ch++){

  }

}
