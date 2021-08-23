#include <map>
#include <string>
#include <iostream>
#include <TString.h>

using namespace std;

typedef std::pair<std::string,std::string> name_title;

std::map<int,name_title> muon_ch_to_name;

string dream_adc_to_name[2];
std::map<int,name_title> dream_ch_to_name[2];

typedef std::pair<int, int> coords;

std::map<int,coords> dream_ch_to_place; 

void init_maps(){

  //muon
  muon_ch_to_name[0]=name_title("Muon_counter","Muon counter");

  //dream
  for(int i_dreamadc=0; i_dreamadc<2; i_dreamadc++) {
    char side;
    if(i_dreamadc==0) side = 's';
    else side = 'q';
    dream_adc_to_name[i_dreamadc] = side;
    name_title tmp;
    for(int i_ch=0; i_ch <= 22; i_ch++){
      if(i_ch>0 && i_ch<16) {
	tmp = name_title(Form("%c_%d",side ,i_ch),
			 Form("Dream ADC %c %d",side ,i_ch));
	dream_ch_to_name[i_dreamadc][i_ch] = tmp;
      }else if(i_ch>16 && i_ch<22) {
	tmp = name_title(Form("%c_%d",side ,i_ch-1),
			 Form("Dream ADC %c %d",side ,i_ch-1));
	dream_ch_to_name[i_dreamadc][i_ch] = tmp;
      }

/*       std::cout << "adc " << i_dreamadc << " ch " << i_ch  */
/* 		<< " dream " <<  (dream_ch_to_name[i_dreamadc][i_ch].first=="") */
/* 		<< std::endl; */
    }
  }
}
