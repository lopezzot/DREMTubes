
#include "run_offset.h"

void print_usage(char* name){
  std::cout << std::endl
  << "Usage: " << name << " <action> <run_number> [offset]" << std::endl;
  std::cout << "action = read | write"<< std::endl;
  std::cout << "Use the following environment variables to change the program behaviour"<< std::endl;
  std::cout << "Input/output file is searched in:"<< std::endl;
  std::cout << "\t $DATADIR/merged/"<< std::endl;
}

int main(int argc, char* argv[])
{

  if(argc<3){
    print_usage(argv[0]);
    exit(1);
  }
  int run_n = atoi(argv[2]);
  bool write=false;
  TString action = argv[1];
  if(action.Contains("write")) write=true;
  int offset=0;
  if(argc>=4){
    offset = atoi(argv[3]);
  }else if(write) {
    print_usage(argv[0]);
    exit(1);
  }

  run_offset(run_n,write,offset);
}
