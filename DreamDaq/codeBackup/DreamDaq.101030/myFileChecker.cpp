//$Id: myFileChecker.cpp,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "myDecode.h"
#include "myFIFO-IOp.h"


#define BLOCKSIZE 1000000 //in words
unsigned int buffer[BLOCKSIZE];

using namespace std;

int main(int argc, char * argv[]){

  FILE * datafile=NULL;

  struct stat filestat;
  unsigned int totalwords=0;
  unsigned int readwords=0;
  int verb=0;
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
  readwords=readwords*sizeof(RunHeader)/sizeof(unsigned int);
  if(myRH.magic!=0xAABBCCDD ||
     (myRH.ruhsiz/sizeof(unsigned int))!=readwords){
    cout << "Wrong Run Header" << endl;
    fclose(datafile);
    return 1;
  }
  
  
  if(verb){
    cout << "@@@@@ Run " << myRH.runnumber << " @@@@@" << endl;
    cout << "Run Header size " << myRH.ruhsiz << endl;
    cout << "Events " << myRH.evtsinrun << endl;
    cout << "Start Time " << myRH.begtim << endl;
    cout << "End Time " << myRH.endtim << endl;
    cout << "     @@@@@@@@@@" << endl;
  }

  totalwords+=readwords;
  
  readwords=1;
  unsigned int error=0;
  unsigned int events=0;
  while(readwords!=0 && !error){
    unsigned int i;
    unsigned int endpointer;
    EventHeader *head;
    
    readwords=fread(buffer,sizeof(unsigned int),BLOCKSIZE,datafile);
    if(ferror(datafile)){
      cout << "Wrong read" << endl;
      fclose(datafile);
      return 1;
    }

    totalwords+=readwords;

    i=0;
    while(i!=readwords){
      unsigned int evsizeword;

      head=(EventHeader *) &buffer[i];
      
      if(head->evmark!=0xCAFECAFE){
	cout << "Cannot find the event marker. Something is wrong." << endl;
	fclose(datafile);
	return 1;
      }

      evsizeword=head->evsiz/sizeof(unsigned int);

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

    if(fseek(datafile,(i-readwords)*sizeof(unsigned int),SEEK_CUR)!=0){
      cout << "Wrong fseek" << endl;
      fclose(datafile);
      return 1;
    }
    totalwords+=i-readwords;
  }

  cout << endl; 
  cout<< "Byte read: " << totalwords*sizeof(unsigned int) << endl;
  cout<< "File size: " << filestat.st_size << endl;


  fclose(datafile);
  
  cout << endl; 
  if(events==myRH.evtsinrun)
    cout << "The file is OK!!" << endl; 
  else{
    cout << "Wrong event count" << endl;
    cout << "Run header says: " << myRH.evtsinrun << endl;
    cout << "I found: " << events << endl;
  }

  return 0;
}
