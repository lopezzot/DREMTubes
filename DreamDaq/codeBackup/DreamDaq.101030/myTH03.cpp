//$Id: myTH03.cpp,v 1.5 2007/06/01 08:19:28 cvsdream Exp $
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <iostream>
#include <pthread.h>

#include "myTH03.h"

#define LOWREF 0x02
#define HIGHREF 0x03
#define CH1 0x04
#define CH2 0x08
#define CH3 0x0C
#define VER 0x01

#define BAUDRATE B2400            
#define MODEMDEVICE "/dev/ttyS0"

#define TIMEOUT 10

using namespace std;

int readdata(int device,unsigned int * value)
{
  unsigned char buf[2];
  int i=0;
  int bytes=0;
  int res=0;

  while(i<TIMEOUT && bytes == 0){
      sleep(1);
      ioctl(device,FIONREAD,&bytes);
      i++;
  }
  
  if(i<TIMEOUT){
      res = read(device,buf,2); 
  }else{
    return -1;
  }
  
  if(res!=2)
    return -2;

  *value=buf[0]<<8;
  *value+=buf[1];
  return 0;
}

void * th03_handler(void * tmpth03){

  int device;
  struct termios newtio,oldtio;
  unsigned char command;
  int status;
  unsigned int value;
  volatile unsigned int tmp;
  
  struct th03_data * th03 = (struct th03_data *)tmpth03;
  th03->running=true;
  
  device = open(MODEMDEVICE, O_RDWR | O_NOCTTY |O_NDELAY ); 
  if(device<0){
    cout << "Unable to open device "
	 << MODEMDEVICE << ". Stopping here." << endl;
    th03->running=false;
    pthread_exit(0);
  }

  tcgetattr(device,&oldtio);

  bzero(&newtio,sizeof(newtio));

  cfsetispeed(&newtio,BAUDRATE);
  cfsetospeed(&newtio,BAUDRATE);

  newtio.c_cflag |= CLOCAL | CREAD;

  newtio.c_cflag &= ~CRTSCTS;

  newtio.c_cflag &= ~PARENB;
  newtio.c_cflag &= ~CSTOPB;
  newtio.c_cflag &= ~CSIZE;
  newtio.c_cflag |= CS8;

  newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  newtio.c_iflag &= ~(IXON | IXOFF | IXANY);

  newtio.c_oflag &= ~OPOST;

  tcsetattr(device,TCSANOW,&newtio);
  
  ioctl(device,TIOCMGET,&status);
  
  status &= ~TIOCM_DTR;
  status |= TIOCM_RTS;
  
  ioctl(device,TIOCMSET,&status);

  sleep(2);
      
  tcflush(device, TCIFLUSH);

  while(th03->stop==false){
    
    command=HIGHREF;
    if(write(device,&command,1)==1){
      if((readdata(device,&value))==0){
	tmp=(th03->refs & 0xFFFF)|(value<<16);
	th03->refs=tmp;
      }
    }

    if(th03->stop)
      break;

    command=LOWREF;
    if(write(device,&command,1)==1){
      if((readdata(device,&value))==0){
	tmp=(th03->refs & 0xFFFF0000)|(value);
	th03->refs=tmp;
      }
    }

    if(th03->stop)
      break;

    command=CH1;
    if(write(device,&command,1)==1){
      if((readdata(device,&value))==0){
	tmp=(th03->ch12 & 0xFFFF)|(value<<16);
	th03->ch12=tmp;
      }
    }

    if(th03->stop)
      break;

    command=CH2;
    if(write(device,&command,1)==1){
      if((readdata(device,&value))==0){
	tmp=(th03->ch12 & 0xFFFF0000)|(value);
	th03->ch12=tmp;
      }
    }

    if(th03->stop)
      break;

    command=CH3;
    if(write(device,&command,1)==1){
      if((readdata(device,&value))==0){
	tmp=(th03->ch3 & 0xFFFF)|(value<<16);
	th03->ch3=tmp;
      }
    }

    if(th03->stop)
      break;

    th03->init=true;

    sleep(2);
  }
  
  tcsetattr(device,TCSANOW,&oldtio);

  close(device);

  th03->running=false;
  pthread_exit(0);
}
