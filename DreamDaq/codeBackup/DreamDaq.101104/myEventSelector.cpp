//$Id: myEventSelector.cpp,v 1.2 2006/10/31 16:04:07 cvsdream Exp $
#include <iostream>
#include <stdio.h>
#include "myFIFO-IOp.h"

using namespace std;

int main(int argc, char * argv[]){
  
  unsigned int size=0;
  unsigned int buf[10000];
  unsigned int word,events=0;
  
  if(argc<3){
    cout << "Usage: " << argv[0] << " n_events input_file output_file" << endl;
    exit(1);
}
  
  unsigned int max=atoi(argv[1]);

  EventHeader head;
  RunHeader run;

  FILE * datafile = fopen(argv[2],"r");
  FILE * outfile = fopen(argv[3],"w");

  //cout << "Datafile opened" << endl;

  fread(&word,sizeof(unsigned int),1,datafile);
  size+=sizeof(unsigned int);

  //cout << "One word read" << endl;

  if(word!=0xaabbccdd){
    exit(1);
  }
  
  if(fseek(datafile,-sizeof(unsigned int),SEEK_CUR)!=0){
    cout << "Wrong fseek" << endl;
    exit(1);
  }
  size-=sizeof(unsigned int);

  if(fread(&run,sizeof(RunHeader),1,datafile)!=1){
    cout << "Wrong read" << endl;
    exit(1);
  }
  
  if(fwrite(&run,sizeof(RunHeader),1,outfile)!=1){
    cout << "Wrong write" << endl;
    exit(1);
  }
  //cout << "Starting loop" << endl;

  while(fread(&word,sizeof(unsigned int),1,datafile)==1){
    size+=sizeof(unsigned int);
    if(word!=0xCAFECAFE){
      cout << "Event header not found" << endl;
      exit(1);
    }
     
    if(fseek(datafile,-sizeof(unsigned int),SEEK_CUR)!=0){
      cout << "Wrong fseek" << endl;
      break;
    }
    size-=sizeof(unsigned int);
        
    if(fread(&head,sizeof(EventHeader),1,datafile)!=1){
      cout << "Wrong read" << endl;
      break;
    }
   
    if(fwrite(&head,sizeof(EventHeader),1,outfile)!=1){
      cout << "Wrong write" << endl;
      exit(1);
    }
 
    size+=sizeof(EventHeader);
    //cout << "Event " << events << " Size " << head.evsiz << endl;
    
    if(fread(&buf,1,head.evsiz-head.evhsiz,datafile)!=(head.evsiz-head.evhsiz)){
      cout << "Wrong read" << endl;
      break;
    }
    
    if(fwrite(&buf,1,head.evsiz-head.evhsiz,outfile)!=(head.evsiz-head.evhsiz)){      cout << "Wrong write" << endl;
      exit(1);
    }

    size+=(head.evsiz-head.evhsiz)/sizeof(unsigned int);
    events++;
    
    if(events%1000==0)
      cout << "Event:" << events << endl;

    if(events>=max)
      break;
  }

  fclose(datafile);
  fclose(outfile);

  cout << "Done" << endl;

  return 0;
}
