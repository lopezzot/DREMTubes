//$Id: checkfile.c,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#include <iostream>
#include <stdio.h>
#include "myFIFO-IOp.h"

using namespace std;

int main(){
  
  unsigned int size=0,internalsize=0;
  int pos[500000];
  int evsize[500000];
  unsigned int word,events=0;
  
  EventHeader head;
  

  FILE * datafile = fopen("datafile","r");
  
  while(fread(&word,sizeof(unsigned int),1,datafile)==1){
    size+=sizeof(unsigned int);
    if(word==0xCAFECAFE){
      if(fseek(datafile,-sizeof(unsigned int),SEEK_CUR)!=0){
	cout << "Wrong fseek" << endl;
	break;
      }
      size-=sizeof(unsigned int);
      
      pos[events]=ftell(datafile);
      
      if(fread(&head,sizeof(EventHeader),1,datafile)!=1){
	cout << "Wrong read" << endl;
	break;
      }
      
      size+=sizeof(EventHeader);
      //cout << "Event " << events << " Size " << head.evsiz << endl;
      internalsize+=head.evsiz;
      evsize[events]=head.evsiz;
      events++;
    }
  }

  pos[events]=ftell(datafile);

  for(unsigned int i=0;i<events;i++)
    if((pos[i+1]-pos[i])!=evsize[i]){
      cout << "ERROR Event " << i 
	   << " File size " << pos[i+1]-pos[i]
	   << " Int. size " << evsize[i] << endl;
    }

  cout << "Total size " << size << endl;
  cout << "Internally written total size " << internalsize << endl;
  cout << "Events " << events << endl;

  fclose(datafile);

  return 0;
}
