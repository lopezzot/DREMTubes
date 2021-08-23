void ana_drs_matrix(int run){
 char ntdatafile_drs1[256],ntdatafile_drs2[256];
sprintf(ntdatafile_drs1,"datafile_histo_run%d_drs.root", run);
TFile* fd= new TFile(ntdatafile_drs1);
//sprintf(ntdatafile_drs2,"./pedestal_file_ori/pedestal_histo_run%d_drs.root", run);
sprintf(ntdatafile_drs2,"./pedestal_histo_run%d_drs.root", run);
TFile* fp= new TFile(ntdatafile_drs2);

TCanvas *c1_1 = new TCanvas("c1_1","DRS1 CH1",0,0,500,500);
fd->cd();
c1_1->SetLogy();
drs1_ch1_integral->Draw();
fp->cd();
drs1_ch1_integral->SetLineColor(4);
drs1_ch1_integral->Draw("SAME");
fd->cd();
drs1_ch1_integral->Draw("SAME");

TCanvas *c0_1 = new TCanvas("c0_1","DRS0 CH1",500,0,500,500);
fd->cd();
c0_1->SetLogy();
drs0_ch1_integral->Draw();
fp->cd();
drs0_ch1_integral->SetLineColor(4);
drs0_ch1_integral->Draw("SAME");
fd->cd();
drs0_ch1_integral->Draw("SAME");

TCanvas *c3_1 = new TCanvas("c3_1","DRS3 CH1",100,100,500,500);
fd->cd();
c3_1->SetLogy();
drs3_ch1_integral->Draw();
fp->cd();
drs3_ch1_integral->SetLineColor(4);
drs3_ch1_integral->Draw("SAME");
fd->cd();
drs3_ch1_integral->Draw("SAME");


}
