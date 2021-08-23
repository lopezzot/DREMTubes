void  plot_leakage(int run)
{

ostringstream filename;
filename << "/home/dreamtest/working/hbook/datafile_histo_run"<<run<<".root";
cout << filename.str() << endl;

TFile f(filename.str().c_str());

 TCanvas h;
 h.Divide(2,3);
 h.cd(1);
 TH1F* Leakage4= (TH1F*)f.Get("Leakage4");
 Leakage4->Draw();
/* h.cd(2);        
 Leakage5.Draw();
 h.cd(3);        
 Leakage6.Draw();
 h.cd(4);        
 Leakage7.Draw();
 h.cd(5);        
 Leakage8.Draw();
 h.cd(6);        
 Leakage9.Draw();
*/
return;
}
