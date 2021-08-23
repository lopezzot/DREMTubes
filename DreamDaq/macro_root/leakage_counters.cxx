
leakage_counters(TString run, TString hprefix="Leakage/L"){
  TCanvas *c = new TCanvas("Leakage", "Leakage", 1200,800);
  c->Divide(5,4);
  TFile *f= TFile::Open(run,"read");
  for(int i=1; i<=20; i++){
    TH1F* h;	
    //cout << hprefix+TString::Itoa(i,10) << endl;
    f->GetObject(hprefix+TString::Itoa(i,10),h);
    c->cd(i);
    h->Draw();
  }
c->Update();
c->SaveAs("/tmp/leakage.gif");
}
leakage_counters(int run_num, TString hprefix="Leakage/L", TString wd="/home/dreamtest/storage/hbook/", TString fprefix="datafile_histo_run"){

  leakage_counters(wd+fprefix+TString::Itoa(run_num,10)+".root",  
	    hprefix);


}

leakage_counters(){

	cout << "usage: " << endl
		<< "leakage_counters( <run_number> )"
		<< endl;
}
