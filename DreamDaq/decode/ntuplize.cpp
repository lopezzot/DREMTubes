#include <iostream>

#include <TTree.h>
#include <TString.h>

int ntuplize(TString fname = "RUN12240.txt"){

  TString datadir = "/home/dreamtest/storage/";
  if (getenv("DATADIR") != NULL)
    datadir = Form("%s", getenv("DATADIR"));

  TString output_base_string = datadir + "MADA_ntuple/";

   TTree *t = new TTree("DREAM_SiPM","DREAM_SiPM");

   TString dir_txt = datadir + "MADA_data/";
   TString d_fname = dir_txt +fname;
   TString o_fname = output_base_string + fname;

   int lines = t->ReadFile(d_fname,"event/l:time/l:ch0/D:ch1:ch2:ch3:ch4:ch5:ch6:ch7:ch8:ch9:ch10:ch11:ch12:ch13:ch14:ch15:ch16:ch17:ch18:ch19:ch20:ch21:ch22:ch23:ch24:ch25:ch26:ch27:ch28:ch29:ch30:ch31:ch32:ch33:ch34:ch35:ch36:ch37:ch38:ch39:ch40:ch41:ch42:ch43:ch44:ch45:ch46:ch47:ch48:ch49:ch50:ch51:ch52:ch53:ch54:ch55:ch56:ch57:ch58:ch59:ch60:ch61:ch62:ch63",';');
   if (!lines) std::cout << "Cannot read input file " << d_fname << std::endl;
   std::cout << "test d_fname : " << d_fname << std::endl;
   std::cout << "before changed fname : " << fname << std::endl;
   fname.ReplaceAll(".txt",".root");

   // from txt to root //
   d_fname.ReplaceAll(".txt",".root");
   o_fname.ReplaceAll(".txt",".root");
//   std::cout << "after changed fname : " << fname << std::endl;
//   std::cout << "after  d_fname : " << d_fname << std::endl;

   t->SaveAs(o_fname);

   return lines;
}

int ntuplize(int run_num){

TString fname = Form("RUN%d.txt",run_num);

return ntuplize(fname);
}

int main(int argc, char* argv[])
{

  if(argc<2){
    std::cout << std::endl
	<< "Usage: " << argv[0] << " <run_number>" << std::endl
	      << std::endl;
    std::cout << "Use the following environment variables to change the program behaviour"<< std::endl;
    std::cout << "Input files are searched in:"<< std::endl;
    std::cout << "\t $DATADIR/MADA_data"<< std::endl;
    std::cout << "Output binary files are placed in:"<< std::endl;
    std::cout << "\t $DATADIR/MADA_ntuple"<< std::endl;
    exit(1);
  }
  int run_number=atoi(argv[1]);

    ntuplize(run_number);
    return 0;
}
