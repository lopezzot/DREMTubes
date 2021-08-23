#include <ni488.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
//#include "myVme.h"
#include <sys/time.h>
//#include "myV262.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

//v262  io(0x400000,"/dev/vmedrv24d16");
bool wait;


int sendcommand(int id, char *comm){

  long len = strlen(comm);
  
  ibwrt(id,comm,len);

  // 0 OK; 1 BAD
  return ibsta & ERR;
}

int readdata(int id, char * buf, int * len){
  
  ibrd(id,buf,*len);
  
  *len=ibcntl;

  // 0 OK, 1 BAD
  return ibsta & ERR;
}


int main(int argc, char ** argv){

  int id;
  int bd;
  char buf[10000000];
  int len;
  unsigned int i;

  //io.reset();

  id = ibdev(0,1,0,T300s,1,0);

  if(id < 0){
    cout << "Cannot open device" << endl;
    return 0;
  }

  cout << "Device opened" << endl;

  ibclr(id);
  
  cout << "Device cleared" << endl;

  
  cout << "send returns " <<   sendcommand(id,"*IDN?")
       << endl;

  len=100;
  readdata(id,buf,&len);
    
  cout << "Returned bytes: " << len << endl;
  cout << strlen(buf)<< " " << buf << endl;
  
  exit(1);

  //single
  sendcommand(id,":ACQ:STOPA SEQ");

  sendcommand(id,":ACQ:STATE RUN");

  buf[0]='1';

  while(buf[0]=='1'){

    char tmp;

    sendcommand(id,":ACQ:STATE?");
        
    len=100;
    readdata(id,buf,&len);

    tmp=buf[0];

    cout << "N bytes: "<< len << " Acq state: " << buf << endl;
    
    sendcommand(id,":ACQ:NUMAC?");

    len=100;
    readdata(id,buf,&len);

    cout << "N bytes: "<< len << " Acq num: " << buf << endl;

    char tmpp[len+1];

    strncpy(tmpp, buf, len);

    cout << "atoi " << atoi(tmpp) << endl;


    buf[0]=tmp;

    usleep(1000000);
  }
  
  /*if(sendcommand(id,":DAT:SOU CH1, CH2, CH3, CH4")){
    cout << "Data source" <<endl;
    return 0;
  }
  
  if(sendcommand(id,":DAT:ENC FAS")){
    cout << "Data encoding" <<endl;
    return 0;
    }*/
  
  /*if(sendcommand(id,":WFMO:BYT_N 1")){
    cout << "Byte number" <<endl;
    return 0;
    }*/
  
  if(sendcommand(id,":DAT:STAR 1")){
    cout << "Data start" <<endl;
    return 0;
  }

  if(sendcommand(id,":DAT:STOP 10000")){
    cout << "Data stop" <<endl;
    return 0;
  }
  
  if(sendcommand(id,":DAT:FRAMESTAR 1")){
    cout << "Data frame start" <<endl;
    return 0;
  }

  if(sendcommand(id,":DAT:FRAMESTOP 10000")){
    cout << "Data frame stop" <<endl;
    return 0;
  }

  if(sendcommand(id,":SAVE:WAVE:FILEF INTERN")){
    cout << "Data format" <<endl;
    return 0;
  }

  struct timeval t0,t1;
  struct timeval tx;
  struct timezone tz;
  struct stat st;

  gettimeofday(&t0, &tz);
 
  if(sendcommand(id,":SAVE:WAVE CH1,\"z:\\prova1.wfm\"")){
    cout << "Data format" <<endl;
    return 0;
  }
  
  if(sendcommand(id,":SAVE:WAVE CH1,\"z:\\prova2.wfm\"")){
    cout << "Data format" <<endl;
    return 0;
  }

  if(sendcommand(id,":SAVE:WAVE CH1,\"z:\\prova3.wfm\"")){
    cout << "Data format" <<endl;
    return 0;
  }
  
  if(sendcommand(id,":SAVE:WAVE CH1,\"z:\\prova4.wfm\"")){
    cout << "Data format" <<endl;
    return 0;
  }
    
  do{
    int res = stat("/home/dreamdaq/working/oscilloscope/prova4.wfm",&st);
    if(res!=0 && errno==ENOENT)
      continue;
    
    if(res!=0){
      cout << "Cannot open file " << errno <<endl;
      break;
    }
    
    usleep(10000);
  
  }while(st.st_size<1236774);

  gettimeofday(&t1, &tz);

  cout << "Read time: " << (t1.tv_sec-t0.tv_sec)*1000000+t1.tv_usec-t0.tv_usec
       << " usec" << endl;
  
  /*
  if(sendcommand(id,":CURV?")){
    cout << "Curve" <<endl;
    return 0;
    }*/


  /*len=10000000;

  struct timeval t0,t1;
  struct timezone tz;

  gettimeofday(&t0, &tz);
  
  if(readdata(id,buf,&len)){
    cout << "Read curve" <<endl;
    return 0;
  }

  len=ibcntl;
  
  gettimeofday(&t1, &tz);

  cout << "Read time: " << (t1.tv_sec-t0.tv_sec)*1000000+t1.tv_usec-t0.tv_usec
       << " usec" << endl;
  cout << "N bytes " << len << " Size of short " << sizeof(short)<< endl;

  unsigned int offset=0;

  do{
    char tmp[10];
    int bytesize;
    int nbyte;

    cout << "Offset " << offset << " ";

    tmp[0]=buf[offset+1];
    tmp[1]='\0';
    
    sscanf(tmp,"%d",&bytesize);

    cout << "Bytesize " << bytesize << " ";
    
    for(i=0;i<bytesize;i++){
      tmp[i]=buf[offset+i+2];
    }
    tmp[i]='\0';
    
    sscanf(tmp,"%d",&nbyte);
    
    cout << "N byte " << nbyte << endl;


    char * data = (char *) &buf[offset+bytesize+2];
    
    cout << endl;
    cout << endl;
    
    //for(i=0;i<250;i++)
    //  cout << i << " " <<(int)data[i] << " " << i << " " << (int)data[i+250]<<endl;
    
    cout << endl;
    cout << endl;
    
    offset+=2+bytesize+nbyte;
    
  }while(buf[offset]!=10);
  */
  //Close device
  ibonl(id,0);

  return 0;
}
 
