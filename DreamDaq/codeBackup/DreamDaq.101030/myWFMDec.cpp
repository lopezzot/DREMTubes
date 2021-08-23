#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

int swap(void * abuf, int size){

  char * buf = (char *) abuf;
  char tmp;
  unsigned int i;

  for(i=0;i<size/2;i++){
    tmp=buf[i];
    buf[i]=buf[size-1-i];
    buf[size-1-i]=tmp;
  }
}


int main(int argc, char **argv){

  short pip=0x2400;

  cout << "P" << pip << endl;

  unsigned int buf[10000];
    //char buf[10000000];
  short corto;
  unsigned int medio;
  /*  FILE * fd;

  fd = fopen("/home/dreamdaq/working/oscilloscope/prova1.wfm","r");

  fread(&corto,sizeof(unsigned short),1,fd);

  // cout << hex <<((unsigned short *)buf)[0] << dec <<endl;

  cout << hex << corto << dec << endl;

  fread(&medio,sizeof(unsigned int),1,fd);

  cout << hex << medio << dec << endl;

  fclose(fd);*/

  int fd;

  fd=open(argv[1],O_RDONLY);
  
  read(fd,&corto,2);

  cout << hex << corto << dec << endl;
  
  read(fd,&medio,4);

  cout << hex << medio << dec << endl;
  
  lseek(fd,72,SEEK_SET);

  read(fd,&medio,4);

  swap(&medio,4);

  

  cout <<  medio  << endl;

  lseek(fd,16,SEEK_SET);

  read(fd,&medio,4);

  cout << hex << medio << dec << endl;

  swap(&medio,4);

  lseek(fd,medio,SEEK_SET);

  cout << endl;

  /*for(unsigned int i=0;i<282;i++){
    read(fd,&corto,2);
    swap(&corto,2);
    cout << corto << endl;
    }*/

  for(unsigned int j=0;j<2000;j++){
    read(fd,buf,282*2);
    
    for(unsigned int i=0;i<282;i++){
      swap(&((short *)buf)[i],2);
      cout << ((short *)buf)[i] << " " << hex << ((short *)buf)[i] << dec << endl;
    }
    cout << "===============" <<endl;
  }
  close(fd);
}

