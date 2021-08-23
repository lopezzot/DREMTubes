#ifndef RUN_OFFSET_H
#define RUN_OFFSET_H

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <TString.h>


int run_offset(int run_number=12230, bool write=false, int offset=-9999){
  std::map<int, int> offset_map;

  int tmp_run;
  int tmp_offset;

  TString datadir = "/home/dreamtest/storage/";
  if (getenv("DATADIR") != NULL){
    datadir = Form("%s", getenv("DATADIR"));
  }
  TString offset_file = datadir + "merged/";
  offset_file = offset_file + "offset.txt";

  std::cout << "Offset file: " << offset_file << std::endl;

  std::ifstream infile(offset_file.Data());

  while (infile >> tmp_run >> tmp_offset){
    offset_map[tmp_run] = tmp_offset;
    //std::cout << tmp_run << " " << tmp_offset << std::endl;
  }
  infile.close();

  if(write){
    offset_map[run_number]=offset;
    std::cout << "writing: " << run_number << " " << offset_map[run_number] << std::endl;

    std::ofstream outfile;
    outfile.open(offset_file.Data(), std::ofstream::out);

    for(std::map<int,int>::iterator it=offset_map.begin(); it!=offset_map.end(); ++it) {
      outfile << it->first<< " " << it->second << std::endl;
    }
  }else{
    std::cout << "read: " << run_number << " " << offset_map[run_number] << std::endl;
  }
  return offset_map[run_number];
}
#endif //RUN_OFFSET_H
