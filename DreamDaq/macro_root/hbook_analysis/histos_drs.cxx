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
#include "read_drs_mean_standalone.cxx"

void histos_drs() {
cout<<" Run 406 started"<<endl;
read_drs_mean_standalone(406,"data",30000);
cout<<" Run 406 finished"<<endl; 
read_drs_mean_standalone(416,"data",30000);
cout<<" Run 416 finished"<<endl;
read_drs_mean_standalone(421,"data",30000);
cout<<" Run 421 finished"<<endl;
read_drs_mean_standalone(422,"data",30000); 
cout<<" Run 422 finished"<<endl;
read_drs_mean_standalone(423,"data",30000); 
cout<<" Run 423 finished"<<endl;
read_drs_mean_standalone(424,"data",30000); 
cout<<" Run 424 finished"<<endl;
read_drs_mean_standalone(430,"data",30000); 
cout<<" Run 430 finished"<<endl;
read_drs_mean_standalone(431,"data",30000); 
cout<<" Run 431 finished"<<endl;
read_drs_mean_standalone(434,"data",30000); 
cout<<" Run 434 finished"<<endl;
read_drs_mean_standalone(437,"data",30000); 
cout<<" Run 437 finished"<<endl;
read_drs_mean_standalone(439,"data",30000); 
cout<<" Run 439 finished"<<endl;
read_drs_mean_standalone(441,"data",30000); 
cout<<" Run 441 finished"<<endl;
read_drs_mean_standalone(443,"data",30000); 
cout<<" Run 443 finished"<<endl;
read_drs_mean_standalone(444,"data",30000); 
cout<<" Run 444 finished"<<endl;
read_drs_mean_standalone(446,"data",30000); 
cout<<" Run 446 finished"<<endl;
read_drs_mean_standalone(447,"data",30000); 
cout<<" Run 447 finished"<<endl;
read_drs_mean_standalone(448,"data",30000); 
cout<<" Run 448 finished"<<endl;
read_drs_mean_standalone(453,"data",30000); 
cout<<" Run 453 finished"<<endl;
read_drs_mean_standalone(454,"data",30000); 
cout<<" Run 454 finished"<<endl;
read_drs_mean_standalone(455,"data",30000); 
cout<<" Run 455 finished"<<endl;
read_drs_mean_standalone(461,"data",30000); 
cout<<" Run 461 finished"<<endl;
read_drs_mean_standalone(468,"data",30000); 
cout<<" Run 468 finished"<<endl;
read_drs_mean_standalone(471,"data",30000); 
cout<<" Run 471 finished"<<endl;
read_drs_mean_standalone(472,"data",30000); 
cout<<" Run 472 finished"<<endl;
read_drs_mean_standalone(476,"data",30000); 
cout<<" Run 476 finished"<<endl;
read_drs_mean_standalone(479,"data",30000);
cout<<" Run 479 finished"<<endl;
}
