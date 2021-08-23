#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <math.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>

#define BAUDRATE B2400            

#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define LOWREF 0x02
#define HIGHREF 0x03
#define CH1 0x04
#define CH2 0x08
#define CH3 0x0C
#define VER 0x01

#define TIMEOUT 30

#define N 2

//#define ORIGIN

int readdata(int fd,unsigned int * value)
{
  unsigned char buf[2];
  int i=0;
  int bytes=0;
  int res=0;

  while(i<TIMEOUT && bytes == 0)
    {
      sleep(1);
      ioctl(fd,FIONREAD,&bytes);
      i++;
    }
  
  if(i<TIMEOUT)
    {
      res = read(fd,buf,2); 
   }
  else
    {
      printf("Timeout\n");
      return -1;
    }
  
  if(res!=2)
    return -2;


  *value=buf[0]<<8;
  *value+=buf[1];
  return 0;
}

int main()
{
  int fd,i;
  struct termios oldtio,newtio;
  unsigned char command;
  int status;
  unsigned int value;
  double lowref, highref, lowref_err,highref_err;
  long int sum;
  double squaresum;
  double mean[3], err[3]; 
  double correct, corr_err;
  double res, temp, inv, res_err, temp_err, inv_err;
     
  fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY |O_NDELAY ); 
  if (fd <0) {perror(MODEMDEVICE); exit(-1); }
  
  tcgetattr(fd,&newtio);

  bzero(&oldtio,sizeof(oldtio));
  
  cfsetispeed(&oldtio,BAUDRATE);
  cfsetospeed(&oldtio,BAUDRATE);

  oldtio.c_cflag |= CLOCAL | CREAD;

  oldtio.c_cflag &= ~CRTSCTS;

  oldtio.c_cflag &= ~PARENB;
  oldtio.c_cflag &= ~CSTOPB;
  oldtio.c_cflag &= ~CSIZE;
  oldtio.c_cflag |= CS8;

  oldtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  oldtio.c_iflag &= ~(IXON | IXOFF | IXANY);

  oldtio.c_oflag &= ~OPOST;

  if(tcsetattr(fd,TCSANOW,&oldtio)!=0){
    printf("first tcsetattr failed %d\n",errno);
    return -1;
  }

  if(tcsetattr(fd,TCSANOW,&oldtio)!=0){
    printf("second tcsetattr failed\n");
    return -1;
  }

  ioctl(fd,TIOCMGET,&status);
  
  status &= ~TIOCM_DTR;
  status |= TIOCM_RTS;
  
  ioctl(fd,TIOCMSET,&status);

  sleep(2);
      
  tcflush(fd, TCIFLUSH);

  command = VER;

  if(write(fd,&command,1)<0){
    printf("Write failed\n");
    return -1;
  }

  if(readdata(fd,&value)<0){
    printf("Read data failed\n");
    return -1;
  }

  printf("Version: %d\n",value);

  do
    {
      printf("\n\n");
      
      sum=0;
      squaresum=0;
      command=LOWREF;

      for(i=0;i<N;i++)
	{
	  	  
	  if(write(fd,&command,1)<0)
	    return -1;
	  
	  if(readdata(fd,&value)<0)
	    return -1;
	  
	  //printf("value: %d\n",value);

	  sum += value;

	  squaresum += pow(value,2)/1000.0;
	}

      lowref = 1.0*sum/N;
      lowref_err= 1.0*sqrt(1.0*(squaresum*1000.0+lowref*lowref*N-2.0*lowref*sum)/(N*(N-1)));
            
      printf("lowref %f err %f\n",lowref,lowref_err);

      
      sum=0;
      squaresum=0;
      command=HIGHREF;

      for(i=0;i<N;i++)
	{
	  	  
	  if(write(fd,&command,1)<0)
	    return -1;
	  
	  if(readdata(fd,&value)<0)
	    return -1;
	  
	  //printf("value: %d\n",value);

	  sum += value;

	  squaresum += pow(value,2)/1000.0;
	}

      highref = 1.0*sum/N;
      highref_err= 1.0*sqrt(1.0*(squaresum*1000.0+highref*highref*N-2.0*highref*sum)/(N*(N-1)));
            
      printf("highref %f err %f\n",highref,highref_err);

      sum=0;
      squaresum=0;
      command=CH1;

      for(i=0;i<N;i++)
	{
	  	  
	  if(write(fd,&command,1)<0)
	    return -1;
	  
	  if(readdata(fd,&value)<0)
	    return -1;
	  
	  //printf("value: %d\n",value);

	  sum += value;

	  squaresum += pow(value,2)/1000.0;
	}
      
      mean[0] = 1.0*sum/N;
      err[0]= 1.0*sqrt(1.0*(squaresum*1000.0+mean[0]*mean[0]*N-2.0*mean[0]*sum)/(N*(N-1)));
      sum=0;
      squaresum=0;
      command=CH2;

      for(i=0;i<N;i++)
	{
	  	  
	  if(write(fd,&command,1)<0)
	    return -1;
	  
	  if(readdata(fd,&value)<0)
	    return -1;
	  
	  //printf("value: %d\n",value);

	  sum += value;

	  squaresum += pow(value,2)/1000.0;
	}
      
      mean[1] = 1.0*sum/N;
      err[1]= 1.0*sqrt(1.0*(squaresum*1000.0+mean[1]*mean[1]*N-2.0*mean[1]*sum)/(N*(N-1)));


            sum=0;
      squaresum=0;
      command=CH3;

      for(i=0;i<N;i++)
	{
	  	  
	  if(write(fd,&command,1)<0)
	    return -1;
	  
	  if(readdata(fd,&value)<0)
	    return -1;
	  
	  //printf("value: %d\n",value);

	  sum += value;

	  squaresum += pow(value,2)/1000.0;
	}
      
      mean[2] = 1.0*sum/N;
      err[2]= 1.0*sqrt(1.0*(squaresum*1000.0+mean[2]*mean[2]*N-2.0*mean[2]*sum)/(N*(N-1)));
      for(i=0;i<3;i++){
	printf("CHANNEL %d\n",i);
	printf("Mean %f err %f\n",mean[i],err[i]);
	
	correct = 20082.0 + (mean[i] - lowref)*23691.0/(highref-lowref);
	corr_err = (23691.0/(highref-lowref))*
	  sqrt(pow(err[i],2)+
	       pow((mean[i]-lowref)/(highref-lowref),2)*pow(highref_err,2)+
	       pow((highref+mean[i]-2.0*lowref)/(highref-lowref),2)*pow(lowref_err,2));
	
	printf("Corrected %f err %f\n",correct,corr_err);
	
	
#ifdef ORIGIN
	res = (607.9+4.0785*(correct-16.0E3)+0.00017*pow(correct-16.0E3,2))/5000.0;
	res_err = sqrt(pow(0.3,2)+pow(9E-4*(correct-16000),2)+pow((correct-16000),4)*pow(7E-7,2)+pow((4.0785+2*1.7E-4*(correct-16000)*corr_err),2))/5000.0;
	
#else
	res = (601+4.105*(correct-16.0E3)+0.000156*pow(correct-16.0E3,2))/5000.0;
	res_err = sqrt(4.0+pow(5E-3*(correct-16000),2)+pow((correct-16000),4)*pow(2E-6,2)+pow((4.105+2*1.56E-4*(correct-16000)*corr_err),2))/5000.0;
	
      #endif
	
	
	printf("Resistenza ridotta: %f err %f\n",res,res_err);
	
	inv = 3.3540154E-3+2.5627725E-4*log(res)+
	  2.082921E-6*pow(log(res),2)+7.3003206E-8*pow(log(res),3);
	
	inv_err= res_err*sqrt(pow(2.5627725E-4+
				  2*2.082921E-6*log(res)+
				  3*7.3003206E-8*pow(log(res),2),2))/res;
	
	temp = pow(inv,-1)-273.15;
	
	temp_err = inv_err*sqrt(pow(inv,-4));
	
	printf("Temperatura: %f err %f\n\n",temp,temp_err);
      }
            
      printf("\nContinue?(y/n)\n");
      command = getchar();
      getchar();
    }
  while(command!='n');

  tcsetattr(fd,TCSANOW,&newtio);

  close(fd);
  
  return 0;
}









