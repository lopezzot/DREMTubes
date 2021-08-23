#include <strstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TF2.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TMath.h"

# include <string>
# include <cmath>
using namespace std;
#include "merge_drs.cxx"
#include "read_drs_mean.cxx"

void merge_and_histos_drs(unsigned int runnumber,TString phys,unsigned int evt_max) {
cout<<" Starting the update of the roople with DRS info "<<endl;
merge_drs(runnumber,phys);
cout<<" Ending update of the roople "<<endl; 
cout<<" Starting the analysis of DRS data"<<endl;
read_drs_mean(runnumber,phys,evt_max);
}
