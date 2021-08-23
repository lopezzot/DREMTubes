//$Id: myTekOsc.h,v 1.10 2009/07/21 09:14:45 dreamdaq Exp $

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <strings.h>
#include <ni488.h>
  //#include <gpib/ib.h>
#include <string.h>
}

// C++ header files
#include <iostream>
#include <string>
#include <sstream>

#include "myOscDataFile.h"
#define BUFL 1000

using namespace std;

class tekOsc{

 public:

  // Constructor
  tekOsc(unsigned int aid,int boardID, int pad, int sad);

  // Generic Commands
  int readEvent(unsigned int * evbuff);         // Read event from hardware and stores it in user-provided buffer
  
  // Crap
  void print();                                 // Simple dump function

  inline unsigned int id() {return m_id;};
  
  int write(int run, int spill);

  //string filename(int run, int spill, int channel, bool local);

  int start();

  int stop();
  
  int getNSample();
  
  int getAcqState();

  unsigned int getChMask(){return chmask;};
  unsigned int getNCh(){return nch;};
  unsigned int getMaxSample(){return maxsample;};
  unsigned int getSampleLen(){return pts;};
  unsigned int getSamRate(){return samrate;};
  unsigned int getChScale(unsigned int ch){
    if(ch>1 && ch <5)
      return scale[ch-1];
    else
      cout << "Unavailable ch " << ch <<endl;
    return 0;
  };
  unsigned int getChPos(unsigned int ch){
    if(ch>1 && ch <5)
      return position[ch-1];
    else
      cout << "Unavailable ch " << ch <<endl;
    return 0;
  };
  
  
 protected:

 private:
  int sendcommand(char * comm);
  int readdata();
  char buf[BUFL];
  int len;
  unsigned int m_id;
  int devid;
  unsigned int chmask;
  unsigned int nch;
  unsigned int maxsample;
  unsigned int pts;
  unsigned int samrate;
  unsigned int position[4];
  unsigned int scale[4];
  
};

int tekOsc::readEvent(unsigned int * evbuff)
{
  //we may want to fill the buffer with zeros
  //then return the number of words to be reserved
  //bzero(evbuff,OSCDATASIZEB);
  unsigned int i;
  unsigned int size=0;
  struct oscheader * head = (struct oscheader *) evbuff;
  head->chn=nch;
  head->chmask=chmask;
  head->pts=pts+ADDITIONALPTS;
  head->samrate=samrate;
  for(i=0;i<4;i++){
    head->scale[i]=scale[i];
    head->position[i]=position[i];
  }
  
  size+=sizeof(struct oscheader)/sizeof(unsigned int);
  size+=chdatasizebyte(pts,nch)/sizeof(unsigned int);

  return size;
}

int tekOsc::sendcommand(char *comm){

  usleep(10000);
  long len = strlen(comm);
  ibwrt(devid,comm,len);

  // 1 OK; 0 BAD
  return ibsta & ERR;
}

int tekOsc::readdata(){
  
  usleep(10000);
  len=BUFL;
  ibrd(devid,buf,len);
  len=ibcntl;

  // 1 OK, 0 BAD
  if(!ibsta & ERR)
    return ibcntl;
  else
    return 0;
}

int tekOsc::start(){
  sendcommand(":ACQ:STOPA SEQ");

  sendcommand(":ACQ:STATE RUN");

  return 0;
}


int tekOsc::stop(){

  sendcommand(":ACQ:STATE STOP");

  return 0;
}

int tekOsc::getAcqState(){
  sendcommand(":ACQ:STATE?");

  readdata();

  char tmp[len+1];

  strncpy(tmp, buf, len);

  return atoi(tmp);

}

int tekOsc::getNSample(){

  sendcommand(":ACQ:NUMAC?");

  readdata();

  char tmp[len+1];

  strncpy(tmp, buf, len);

  return atoi(tmp);
}
  
int tekOsc::write(int run, int spill){
 
  char comm[200];
  unsigned int i;
  unsigned int last=0;
  
  sendcommand(":DAT:STAR 1");
  sendcommand(":DAT:STOP 10000");
  sendcommand(":DAT:FRAMESTAR 1");
  sendcommand(":DAT:FRAMESTOP 10000");
  sendcommand(":SAVE:WAVE:FILEF INTERN");

  
  for(i=0;i<4;i++){
    if((chmask>>i)&1){
      sprintf(comm,":SAVE:WAVE CH%d,\"",i+1);
      string cmd(comm);
      cmd=cmd+filename(run,spill,i+1,false)+"\"";
      strcpy(comm,cmd.c_str());
      sendcommand(comm);
      last=i+1;
    }
  }
  
  struct stat st;
  do{
    usleep(10000);
    int res = stat(filename(run,spill,last,true).c_str(),&st);
    if(res!=0 && errno==ENOENT)
      continue;
  }while(st.st_mtime>=time(NULL));
  
  return 0;
}


tekOsc::tekOsc(unsigned int aid,int boardID, int pad, int sad):m_id(aid){

  unsigned int i;

  usleep(10000);
  devid = ibdev(boardID,pad,sad,T300s,1,0);

  usleep(10000);
  ibclr(devid);

  //Max samples
  sendcommand(":HOR:FAST:COUN?");
  
  readdata();
  
  {
    char tmp[len+1];
    
    strncpy(tmp, buf, len);

    maxsample=atoi(tmp);
  }

  //Enabled channels
  sendcommand(":SEL?");

  readdata();

  chmask=0;
  nch=0;

  for(i=0;i<4;i++){
    chmask |= (buf[i*2]=='1') ? 1 << i : 0;
    nch+= chmask >> i;
  }
  
  //Points per sample
  sendcommand(":HOR:FAST:LEN?");
  
  readdata();
  {
    char tmp[len+1];
    
    strncpy(tmp, buf, len);
    
    pts=atoi(tmp);
  }

  //Sample rate
  sendcommand(":HOR:MAI:SAMPLER?");
  
  readdata();
  {
    char tmp[len+1];
    
    strncpy(tmp, buf, len);
    
    samrate=(unsigned int)atof(tmp);
  }

  
  char comm[100];
  for(i=0;i<4;i++){
    if(((chmask >> i)& 1)){
      
      sprintf(comm,":CH%d:SCAL?",i+1);
      sendcommand(comm);
      
      readdata();
      {
	char tmp[len+1];
	
	strncpy(tmp, buf, len);
      
	scale[i]=(unsigned int)(atof(tmp)*1000);
      }

      sprintf(comm,":CH%d:POS?",i+1);
      sendcommand(comm);
  
      readdata();
      {
	char tmp[len+1];
	
	strncpy(tmp, buf, len);
	position[i]=(unsigned int)((atof(tmp)+5)*1000);
      }
      
    }
  }


  cout << "Osc. conf.: Nch " << nch << " - Max samples " 
       << maxsample << " - Pts/sample " << pts 
       << " - Sam. rate " << samrate << endl;
 
  for(i=0;i<4;i++){
    if(((chmask >> i)& 1)==1){
      cout << "Ch " << i+1
	   << " Scale " << scale[i] << " mV/div - "
	   << "Position " << position[i] << " div" << endl;
    }
  }

}    


/*****************************************/
void tekOsc::print() 
/*****************************************/
{
  cout << "*************************************" << endl;  
  cout << "Tektronics Oscilloscope. Id "<< m_id << endl;
  cout << "*************************************" << endl;
  return;
}
