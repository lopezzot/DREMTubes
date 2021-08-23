#include <iostream>
#include <stdio.h>
#include "myOscDataFile.h"
#include "myFIFO-IOp.h"

using namespace std;

int main(int argc, char **argv){

  struct oscheader head;
  unsigned int word,i,j;
  unsigned int size;
  short buf[1128];
  FILE * fd = fopen(argv[1],"r");

  do{
    fread(&word,sizeof(unsigned int),1,fd);
  }while(word!=0xFAFA);
  fread(&size,sizeof(unsigned int),1,fd);

  size=(size-sizeof(struct oscheader)-sizeof(SubEventHeader))/sizeof(short);

  fread(&head,sizeof(struct oscheader),1,fd);
  
  cout << "Nch " << head.chn << " Size (shorts) " << size <<endl;

  fread(buf,sizeof(short),size,fd);

  fread(&word,sizeof(unsigned int),1,fd);
  
  if(word==0xACABACAB){
    for(i=0;i<(size/head.chn);i++){
      for(j=0;j<head.chn;j++)
	cout << buf[i+j*size/head.chn] << "\t";
      cout << endl;
    }
  }else{
    cout << "Wrong alignment" << endl;
  }

  fclose(fd);

  return 0;
}
