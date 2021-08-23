#include <iostream>
#include <stdio.h>
#include "myFIFO-IOp.h"

#define SCALER 0x200003
#define ADC0 0x04000005
#define ADC1 0x06000005

using namespace std;

unsigned int scalerpar(FILE * file,unsigned * cle,unsigned int * ev){
  unsigned int init=ftell(file);
  SubEventHeader subhead;
  unsigned int scalerdata[10];

  fread(&subhead,sizeof(SubEventHeader),1,file);
  //cout << "Marker: " << hex << subhead.semk << dec <<endl;
  fread(scalerdata,1,
	  subhead.size-sizeof(SubEventHeader),file);
  //cout << "scaler 0: " << hex << scalerdata[0] << dec <<endl;
  *ev=(scalerdata[3]&0xFFFFFF);
  *cle=(scalerdata[4]&0xFFFFFF);

  fseek(file,init,SEEK_SET);
  return 0;
}

unsigned int findevent(unsigned int evnum,FILE * file, unsigned int deep){
  
  unsigned int init=ftell(file);
  EventHeader evhead;
  unsigned int evend;
  unsigned int ev=0;
  unsigned int pos=0;
  
  do{
    fread(&evhead,sizeof(EventHeader),1,file);
    //cout << "Marker: " << hex << subhead.semk << dec <<endl;
    evend=ftell(file)+evhead.evsiz-sizeof(EventHeader);
    if(evhead.evnum==evnum){
      pos=ftell(file)-sizeof(EventHeader);
      break;
    }else{
      fseek(file,evend,SEEK_SET);
    }
    ev++;
  }while(ev<deep);

  fseek(file,init,SEEK_SET);
  return pos;
}

FILE * nextevent(FILE * datafile, FILE * pedfile,
		 unsigned int *off ,bool * isPhys){
  
  unsigned int init;
  EventHeader evhead;
  unsigned int pos=0;
  unsigned int dataevnum;
  unsigned int invalid=10000000;

  init=ftell(datafile);
  if(fread(&evhead,sizeof(EventHeader),1,datafile)!=0){
    fseek(datafile,init,SEEK_SET);
    dataevnum=evhead.evnum;
    pos=ftell(datafile);
  }else
    dataevnum=invalid;
  
  init=ftell(pedfile);
  if(fread(&evhead,sizeof(EventHeader),1,pedfile)!=0){
    fseek(pedfile,init,SEEK_SET);
  }else{
    evhead.evnum=invalid;
  }

  if(dataevnum==invalid && evhead.evnum==invalid){
    return NULL;
  }
  
  if(dataevnum<evhead.evnum){
    *isPhys=true;
    *off=pos;
    return datafile;
  }else{
    *isPhys=false;
    *off=ftell(pedfile);
    return pedfile;
  }
}

unsigned int findsubevent(unsigned int id,FILE * file,
			  unsigned int maxoffset, unsigned int * size){
  SubEventHeader subhead;
  unsigned int subend;
  unsigned int init=ftell(file);
  unsigned int pos=0;
  do{
    if(fread(&subhead,sizeof(SubEventHeader),1,file)==0)
      break;
    //cout << "Marker: " << hex << subhead.semk << dec <<endl;
    subend=ftell(file)+subhead.size-sizeof(SubEventHeader);
    if(subhead.id==id){
      *size=subhead.size;
      pos=ftell(file)-sizeof(SubEventHeader);
      break;
    }else{
      fseek(file,subend,SEEK_SET);
    }
  }while(subend<maxoffset);
  
  fseek(file,init,SEEK_SET);
  //cout << endl ;
  return pos;
}


int main(int argc, char **argv){

  unsigned int buffer[10000];
  unsigned int scalerdata[10];
  unsigned int totevphys;
  unsigned int totevped;
  unsigned int ev=0;
  unsigned int ped=0;
  unsigned int offset=0;
  char name[1000];

  if(argc<5){
    cout << "Usage: " << argv[0] << " datafile pedfile runnumber outdir" << endl;
    return 1;
  }

  FILE * olddata = fopen(argv[1],"r");
  if(olddata==NULL){
    cout << "Cannot open: " << argv[1] << endl;
    return 1;
  }
  FILE * oldped = fopen(argv[2],"r");
  if(oldped==NULL){
    cout << "Cannot open: " << argv[2] << endl;
    return 1;
  }
  snprintf(name,1000,"%s/datafile_run%s_corrected.dat",argv[4],argv[3]);
  FILE * newdata = fopen(name,"w+");
  if(newdata==NULL){
    cout << "Cannot open: " << name << endl;
    return 1;
  }
  snprintf(name,1000,"%s/pedestal_run%s_corrected.dat",argv[4],argv[3]);
  FILE * newped = fopen(name,"w+");
  if(newped==NULL){
    cout << "Cannot open: " << name << endl;
    return 1;
  }

  fread(buffer,sizeof(RunHeader),1,olddata);
  fwrite(buffer,sizeof(RunHeader),1,newdata);
  totevphys=((RunHeader *)buffer)->evtsinrun;
    
  fread(buffer,sizeof(RunHeader),1,oldped);
  fwrite(buffer,sizeof(RunHeader),1,newped);
  totevped=((RunHeader *)buffer)->evtsinrun;
  
  unsigned int tot, oldtot;
  unsigned int cle, oldcle;
  unsigned int evnum, oldevnum;
  bool isPhys;
  do{
    unsigned int evpos;
    FILE * thisev;
    thisev=nextevent(olddata,oldped,
		     &evpos ,&isPhys);
    fseek(thisev,evpos,SEEK_SET);
    fread(buffer,sizeof(EventHeader),1,thisev);
    //cout << "Marker: " << hex << buffer[0] << " " << isPhys <<dec <<endl;
    
    unsigned int evend=ftell(thisev)+((EventHeader*)buffer)->evsiz-sizeof(EventHeader);
    unsigned int evsiz=((EventHeader*)buffer)->evsiz;
    oldevnum=evnum;
    evnum=((EventHeader*)buffer)->evnum;
    //cout << "Managing event: " << evnum << endl;
    
    unsigned int size;
    unsigned int adc0offset=findsubevent(ADC0,thisev,evend,&size);
    unsigned int adc1offset=findsubevent(ADC1,thisev,evend,&size);
    unsigned int scaleroff=findsubevent(SCALER,thisev,evend,&size);

    fseek(thisev,scaleroff,SEEK_SET);

    oldcle=cle;
    oldtot=tot;
    scalerpar(thisev,&cle,&tot);
    //if(cle==0)
    //cout << "cle 0" << endl;

    if((tot-1)!=evnum){
      cout << "Opss.  Tot: " << tot 
	   << " Cle: "<< cle 
	   << " Evnum: " << evnum 
	   << " == oldTot: " << oldtot 
	   << " oldCle: "<< oldcle 
	   << " oldEvnum: " << oldevnum 
	   << endl;
      break;
    }
    
    fseek(thisev,evpos,SEEK_SET);
    fread(buffer,1,evsiz,thisev);
    if(isPhys)
      fwrite(buffer,1,evsiz,newdata);
    else
      fwrite(buffer,1,evsiz,newped);

    //cout << "Event written" << endl;
    if(cle){
      //look for the correct event
      //cout << "Event: " << evnum <<" Cle: " << cle << endl;
      if((evnum+cle)>(totevphys+totevped))
	break;

      fseek(thisev,evend,SEEK_SET);
      unsigned int olddataoff=ftell(olddata);
      unsigned int oldpedoff=ftell(oldped);
      unsigned copyfrom;
      bool unused;
      FILE * fromev;
      /*      if(cle>1)
	cout << "olddata off: "<< ftell(olddata)
	<< " oldped off: " << ftell(oldped) << endl;*/

      for(unsigned int i=0;i<cle;i++){
	fromev=nextevent(olddata,oldped,
			 &copyfrom,&unused);
	fseek(fromev,copyfrom,SEEK_SET);
	fread(buffer,sizeof(EventHeader),1,fromev);
	fseek(fromev,((EventHeader*)buffer)->evsiz-sizeof(EventHeader),SEEK_CUR);
	/*if(cle>1)
	  cout << "fromev: " << fromev 
	       << " copyfrom " << copyfrom 
	       << " olddata " << olddata 
	       << " oldped " << oldped 
	       << " olddata off: "<< ftell(olddata)
	       << " oldped off: " << ftell(oldped) 
	       << " fromev: " << ftell(fromev)<< endl;*/
      }
      /*      if(cle>1)
	      cout << endl;*/
      
      fseek(fromev,copyfrom,SEEK_SET);

      fread(buffer,sizeof(EventHeader),1,fromev);
      unsigned int fromend=ftell(fromev)+((EventHeader*)buffer)->evsiz-sizeof(EventHeader);
      
      FILE * tofile;
      if(isPhys)
	tofile=newdata;
      else
	tofile=newped;
      //cout << "Event: " << evnum << endl;
      unsigned int newadc0offset=findsubevent(ADC0,fromev,fromend,&size);
      fseek(tofile,adc0offset,SEEK_SET);
      fseek(fromev,newadc0offset,SEEK_SET);
      fread(buffer,1,size,fromev);
      //cout << "Marker1: " << hex <<buffer[0] << dec << endl;
      fwrite(buffer,1,size,tofile);
      
      unsigned int newadc1offset=findsubevent(ADC1,fromev,fromend,&size);
      fseek(tofile,adc1offset,SEEK_SET);
      fseek(fromev,newadc1offset,SEEK_SET);
      fread(buffer,1,size,fromev);
      //cout << "Marker2: " << hex << buffer[0] << dec << endl;
      fwrite(buffer,1,size,tofile);
      //scrivere

      fseek(tofile,evend,SEEK_SET);
      fseek(olddata,olddataoff,SEEK_SET);
      fseek(oldped,oldpedoff,SEEK_SET);
      //cout << "Event updated" << endl;
    }
    
    if(isPhys)
      ev++;
    else
      ped++;
    
  }while(ev<totevphys || ped <totevped);
  
  cout << "Closing files" << endl;
  
  fseek(newdata,0,SEEK_SET);
  fread(buffer,sizeof(RunHeader),1,newdata);
  ((RunHeader *)buffer)->evtsinrun=ev;
  fseek(newdata,0,SEEK_SET);
  fwrite(buffer,sizeof(RunHeader),1,newdata);

  fseek(newped,0,SEEK_SET);
  fread(buffer,sizeof(RunHeader),1,newped);
  ((RunHeader *)buffer)->evtsinrun=ev;
  fseek(newped,0,SEEK_SET);
  fwrite(buffer,sizeof(RunHeader),1,newped);
  
  fclose(olddata);
  fclose(oldped);
  fclose(newdata);
  fclose(newped);
  return 0;
}


