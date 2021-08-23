{
  //TFile *_file0 = TFile::Open("./datafile_ntup_run6836.root");
  TFile *_file0 = TFile::Open("./test.root");
  //DREAM->Scan("SROSC:X[0]:Nrunnumber:CHADCN0[0]:COUNTTDC[0]:Nevtda:Entry$","Nevtda==0||(Nevtda>=3525 && Nevtda<=4385)");
DREAM->Scan("SROSC:X[0]:Nrunnumber:CHADCN0[0]:COUNTTDC[0]:Nevtda:Entry$","Nevtda==0");


}
