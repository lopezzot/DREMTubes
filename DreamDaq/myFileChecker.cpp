#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <cstdio>
#include <iostream>

#include "myDecode.h"
#include "myFIFO-IOp.h"

#define BLOCKSIZE 1000000 //in words
uint32_t buffer[BLOCKSIZE];

using namespace std;

int32_t main(int32_t argc, char* argv[]){

  FILE* datafile=NULL;

  struct stat filestat;
  uint32_t totalwords=0;
  uint32_t readwords=0;
  int32_t verb=0;
  RunHeader myRH;

  if(argc>3 || argc<2){
    cout << "Usage: "<< argv[0] << " [-v] datafile" << endl;
    return 1;
  }else if(argc==3){
    if(strcmp(argv[1],"-v")==0)
      verb=1;
    else{
      cout << "Usage: "<< argv[0] << " [-v]" << endl;
      return 1;
    }
    datafile=fopen(argv[2],"r");
    if(datafile==NULL){
      cout << "Cannot open file " << argv[2] << endl;
      return 1;
    }
    stat(argv[2],&filestat);
  }else if(argc==2){
    datafile=fopen(argv[1],"r");
    if(datafile==NULL){
      cout << "Cannot open file " << argv[1] << endl;
      return 1;
    }
    stat(argv[1],&filestat);
  }

  if(fseek(datafile,0,SEEK_SET)!=0){
    cout << "Wrong fseek" << endl;
    fclose(datafile);
    return 1;
  }
  readwords=fread(&myRH,sizeof(RunHeader),1,datafile);
  readwords=readwords*sizeof(RunHeader)/sizeof(uint32_t);
  if(myRH.magic!=0xAABBCCDD ||
     (myRH.ruhsiz/sizeof(uint32_t))!=readwords){
    cout << "Wrong Run Header" << endl;
    fclose(datafile);
    return 1;
  }
  
  
  if(verb){
    cout << "@@@@@ Run " << myRH.runnumber << " @@@@@";
    cout << "\nRun Header size " << myRH.ruhsiz;
    cout << "\nEvents " << myRH.evtsinrun;
    cout << "\nStart Time " << myRH.begtim;
    cout << "\nEnd Time " << myRH.endtim;
    cout << "\n     @@@@@@@@@@" << endl;
  }

  totalwords+=readwords;
  
  readwords=1;
  uint32_t error=0;
  uint32_t events=0;
  while(readwords!=0 && !error){
    uint32_t i;
    uint32_t endpointer;
    EventHeader *head;
    
    readwords=fread(buffer,sizeof(uint32_t),BLOCKSIZE,datafile);
    if(ferror(datafile)){
      cout << "Wrong read" << endl;
      fclose(datafile);
      return 1;
    }

    totalwords+=readwords;

    i=0;
    while(i!=readwords){
      uint32_t evsizeword;

      head=(EventHeader *) &buffer[i];
      
      if(head->evmark!=0xCAFECAFE){
	cout << "Cannot find the event marker. Something is wrong." << endl;
	fclose(datafile);
	return 1;
      }

      evsizeword=head->evsiz/sizeof(uint32_t);

      if(evsizeword>BLOCKSIZE){
	cout << "Event is bigger than the block size" << endl;
	fclose(datafile);
	return 1;
      }

      endpointer=i+evsizeword;
      if(endpointer>readwords){
	break;
      }

      if(myEvent(&buffer[i],verb)!=0){
	cout << "Something is wrong in event "<< 0 << " stopping here" << endl;
	fclose(datafile);
	return 1;
      }
      events++;
      i=endpointer;
    }

    if(fseek(datafile,(i-readwords)*sizeof(uint32_t),SEEK_CUR)!=0){
      cout << "Wrong fseek" << endl;
      fclose(datafile);
      return 1;
    }
    totalwords+=i-readwords;
  }

  cout << "\nByte read: " << totalwords*sizeof(uint32_t);
  cout << "\nFile size: " << filestat.st_size << endl;


  fclose(datafile);
  
  cout << endl; 
  if(events==myRH.evtsinrun)
    cout << "The file is OK!!" << endl;
  else{
    cout << "Wrong event count";
    cout << "\nRun header says: " << myRH.evtsinrun;
    cout << "\nI found: " << events << endl;
  }

  return 0;
}
