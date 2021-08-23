//$Id: myThread.h,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#ifndef _MYTHREAD_
#define _MYTHREAD_


#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* close */
#include <string.h>
 
#define SERVER_PORT 2055
#define MAX_MSG 100

//int stop;

//pointer to data from SlowControl
double *dtmp; 

// Functions to read Slow Control Values

//Detector A
int readDetACathodeVmon(){
  return int(dtmp[0]*1000);
}
int readDetAG1_upVmon(){
  return int(dtmp[1]*1000);
}
int readDetAG1_dwVmon(){
  return int(dtmp[2]*1000);
}
int readDetAG2_upVmon(){
  return int(dtmp[3]*1000);
}
int readDetAG2_dwVmon(){
  return int(dtmp[4]*1000);
}
int readDetAG3_upVmon(){
  return int(dtmp[5]*1000);
}
int readDetAG3_dwVmon(){
  return int(dtmp[6]*1000);
}
int readDetAAnodeVmon(){
  return int(dtmp[7]*1000);
}
int readDetACathodeImon(){
  return int(dtmp[8]*1000);
}
int readDetAG1_upImon(){
  return int(dtmp[9]*1000);
}
int readDetAG1_dwImon(){
  return int(dtmp[10]*1000);
}
int readDetAG2_upImon(){
  return int(dtmp[11]*1000);
}
int readDetAG2_dwImon(){
  return int(dtmp[12]*1000);
}
int readDetAG3_upImon(){
  return int(dtmp[13]*1000);
}
int readDetAG3_dwImon(){
  return int(dtmp[14]*1000);
}
int readDetAAnodeImon(){
  return int(dtmp[15]*1000);
}
//Detector B
int readDetBCathodeVmon(){
  return int(dtmp[16]*1000);
}
int readDetBG1_upVmon(){
  return int(dtmp[17]*1000);
}
int readDetBG1_dwVmon(){
  return int(dtmp[18]*1000);
}
int readDetBG2_upVmon(){
  return int(dtmp[19]*1000);
}
int readDetBG2_dwVmon(){
  return int(dtmp[20]*1000);
}
int readDetBG3_upVmon(){
  return int(dtmp[21]*1000);
}
int readDetBG3_dwVmon(){
  return int(dtmp[22]*1000);
}
int readDetBAnodeVmon(){
  return int(dtmp[23]*1000);
}
int readDetBCathodeImon(){
  return int(dtmp[24]*1000);
}
int readDetBG1_upImon(){
  return int(dtmp[25]*1000);
}
int readDetBG1_dwImon(){
  return int(dtmp[26]*1000);
}
int readDetBG2_upImon(){
  return int(dtmp[27]*1000);
}
int readDetBG2_dwImon(){
  return int(dtmp[28]*1000);
}
int readDetBG3_upImon(){
  return int(dtmp[29]*1000);
}
int readDetBG3_dwImon(){
  return int(dtmp[30]*1000);
}
int readDetBAnodeImon(){
  return int(dtmp[31]*1000);
}
//Detector C
int readDetCCathodeVmon(){
  return int(dtmp[32]*1000);
}
int readDetCG1_upVmon(){
  return int(dtmp[33]*1000);
}
int readDetCG1_dwVmon(){
  return int(dtmp[34]*1000);
}
int readDetCG2_upVmon(){
  return int(dtmp[35]*1000);
}
int readDetCG2_dwVmon(){
  return int(dtmp[36]*1000);
}
int readDetCG3_upVmon(){
  return int(dtmp[37]*1000);
}
int readDetCG3_dwVmon(){
  return int(dtmp[38]*1000);
}
int readDetCAnodeVmon(){
  return int(dtmp[39]*1000);
}
int readDetCCathodeImon(){
  return int(dtmp[40]*1000);
}
int readDetCG1_upImon(){
  return int(dtmp[41]*1000);
}
int readDetCG1_dwImon(){
  return int(dtmp[42]*1000);
}
int readDetCG2_upImon(){
  return int(dtmp[43]*1000);
}
int readDetCG2_dwImon(){
  return int(dtmp[44]*1000);
}
int readDetCG3_upImon(){
  return int(dtmp[45]*1000);
}
int readDetCG3_dwImon(){
  return int(dtmp[46]*1000);
}
int readDetCAnodeImon(){
  return int(dtmp[47]*1000);
}
int readAltitude(){
  return int(dtmp[48]*1000000);
  }
int readDewPoint(){
  return int(dtmp[49]*1000000);
}
int readHumidity(){
  return int(dtmp[50]*1000000);
}
int readPressure(){
  return int(dtmp[51]*1000000);
}
int readQnh(){
  return int(dtmp[52]*1000000);
}
int readTemp1(){
  return int(dtmp[53]*1000000);
}
int readTemp2(){
  return int(dtmp[54]*1000000);
}
int readToverP(){
  return int(dtmp[55]*1000000);
}
int readArFlux(){
  return int(dtmp[56]*1000000);
}
int readCO2Flux(){
  return int(dtmp[57]*1000000);
}
int readCF4Flux(){
  return int(dtmp[58]*1000000);
}
int readSlowTime(){
  return int(dtmp[59]);
}
int readSlowDate(){
  return int(dtmp[60]);
}
int readPMUpVmon(){
  return int(dtmp[61]);
}
int readPMUpImon(){
  return int(dtmp[62]);
}
int readPMDwVmon(){
  return int(dtmp[63]);
}
int readPMDwImon(){
  return int(dtmp[64]);
}
int readPM1Vmon(){
  return int(dtmp[65]);
}
int readPM1Imon(){
  return int(dtmp[66]);
}
int readPM2Vmon(){
  return int(dtmp[67]);
}
int readPM2Imon(){
  return int(dtmp[68]);
}
int readDivDetDVmon(){
  return int(dtmp[69]);
}
int readDivDetDImon(){
  return int(dtmp[70]);
}
int readDivDetEVmon(){
  return int(dtmp[71]);
}
int readDivDetEImon(){
  return int(dtmp[72]);
}
int readGainDetA(){
  return int(dtmp[73]);
}
int readGainDetB(){
  return int(dtmp[74]);
}
int readGainDetC(){
  return int(dtmp[75]);
}



////////////////////////////////////////////////////  
// Client Function
////////////////////////////////////////////////////

void* readSlowControl( void *buf) {
//void* readSlowControl(void *unused) {
  
  //  double *dtmp; 
  int j; 
  char s2[] = " "; 
  char *p; 
  int sd, rc;
  char pippo[1024];
  
  //    struct sockaddr_in localAddr, servAddr;
  struct sockaddr_in servAddr;
  struct hostent *h;
 
  dtmp = (double *) buf;

    //(double *) buf;
  
  
  h = gethostbyname("lhcb12.ca.infn.it");
  
  servAddr.sin_family = h->h_addrtype;
  memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  servAddr.sin_port = htons(SERVER_PORT);
  
  //  printf("%i +++++++",stop);

  while(1) {
    sleep(1); 
    /* create  socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd<0) { perror("cannot open socket "); exit(1); }
    
    /* connect to server */
    rc = connect(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));
    if(rc<0) { perror("cannot connect "); exit(1); }
    
    /* read data */
    rc = recv(sd, pippo, 1000, 0);
    if(rc<0) { perror("cannot read data "); close(sd); exit(1); }
    
    pippo[rc+1]='\0'; 
    //    printf("%s \n", pippo);
    close(sd);
    
    //    printf("%s",strtok(pippo,s2));
    dtmp[0]=atof(strtok(pippo,s2));
    //    printf("0 %lf\n", dtmp[0]);
    
    j=1;
    while ((p = strtok(NULL, s2)) != NULL) {
      dtmp[j]=atof(p);
      //      printf("%i %lf\n",j, dtmp[j]);
      j++;
    }
  }
  return NULL;
}
  

/////////////////////////////////////////////////////////////////
//   Start the thread
/////////////////////////////////////////////////////////////////



int startSlowControlThread()
{
  pthread_t thread1;
  double buf[1024];
  int  iret1;
  
  iret1 = pthread_create( &thread1, NULL,readSlowControl, (void*) buf);
  //iret1 = pthread_create( &thread1, NULL,readSlowControl, NULL);
  pthread_detach( thread1);  
  return iret1;
}

void stopSlowControlThread()
{
  pthread_exit((void *)1);
  return;
}


#endif  


