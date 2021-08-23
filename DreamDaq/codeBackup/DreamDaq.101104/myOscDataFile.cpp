
#include <sstream>
#include "myOscDataFile.h"

using namespace std;

string filename(int run, int spill, int channel, bool local){

  ostringstream oss;

  if(local){
    oss << "/home/dreamtest/oscilloscope/";
  }else{
    oss << "b:\\";
  }
  
  oss << "RUN" << run << "_Spill" << spill << "_Ch" << channel << ".wmf";

  return oss.str();
}

void swapbytes(void * abuf, unsigned int size){

  char * buf = (char *) abuf;
  char tmp;
  unsigned int i;
  
  for(i=0;i<size/2;i++){
    tmp=buf[i];
    buf[i]=buf[size-1-i];
    buf[size-1-i]=tmp;
  }
}


unsigned int chdatasizebyte(unsigned int pts, unsigned int nch){
  return nch*(ADDITIONALPTS+pts)*2;
}
