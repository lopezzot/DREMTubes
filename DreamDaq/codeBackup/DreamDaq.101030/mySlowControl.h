//$Id: mySlowControl.h,v 1.2 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  mySlowControl.h
  ---------------

  Definition of all is needed to exchange
  slow control data between Slow Control computer
  and DAQ computer.

  Note that a Slow Control record is identical to 
  all other subevents.

  A. Cardini / INFN Cagliari
  D. Raspino / Universita' Cagliari & INFN Cagliari

  Version 0.1: 15/04/2004

*****************************************/

// Standard C header files
extern "C" {
  // Generic
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>

  // Network
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

  // Multi-thread
#include <pthread.h>

  // Specific
#include "myModules.h"
}

// C++ header files
#include <iostream>


#define MAX_MSG 540  // non toccare piu' questo numero a meno di modifiche allo SlowControl

//using namespace std;

/*****************************************/
// The slowControl class 
/*****************************************/
class slowControl {

 public:

  slowControl(char *ddaddr, int port);                                         // Constructor
  ~slowControl();                                                              // Destructor

  // Generic Commands
  inline unsigned int id();                                                    // Returns module ID
  char *ipaddr();                                                              // Returns server TCP/IP address in decimal-dotted form
  int port();                                                                  // Returns server TCP/IP port

  // Readout Function
  void *DavideClient(char *pippo);
  void* myLoop(char *buf);
  void* myPrint(void* unused);
  void* createTh(char *buf);
  void * readData(void *evbuff);                                   // Read data and stores it in user-provided buffer
  
  //thread function
  //  int startThread();
  void *  threadLoop(char *buff);

  // Crap
  void print();                                                                // Simple dump function
  
 protected:
  
  struct sockaddr_in srvaddr;                                                  // Server IP address
  int enabled;                                                                 // 1 if connection ok, 0 otherwise
  int socketDescriptor;
  
 private:
  
};

/*****************************************/
// Constructor
/*****************************************/
slowControl::slowControl(char *ddaddr, int port) 
{ 
  int rc;

  srvaddr.sin_family = AF_INET;                  /* internet protocols is always AF_INET */
  rc = inet_aton(ddaddr, &(srvaddr.sin_addr));   /* address */
  srvaddr.sin_port = htons(port);                /* port */
  //  cout <<  inet_ntoa(srvaddr.sin_addr) << endl;
  //  cout <<  port << endl;
  /* qui leggo i dati per prova e vedo se la comunicazione e' ok */
  enabled = 1;  /* TCP/IP communication with slow control enabled */
};

/*****************************************/
// Destructor
/*****************************************/
slowControl::~slowControl() 
{ 
  return;
};

/*****************************************/
// Generic Commands
/*****************************************/
inline unsigned int slowControl::id()            { return ID_SLOWCONTROL; };
inline int   slowControl::port()                 { return ntohs(srvaddr.sin_port); };
inline char  *slowControl::ipaddr()              { return inet_ntoa(srvaddr.sin_addr); };

/*****************************************/
// Readout Commands
/*****************************************/

  void* readSlow::DavideClient(char *pippo) {
    
    double dtmp[100]; 
    int j; 
    char s2[] = " "; 
    char *p; 
    int sd, rc, i;
    
    struct sockaddr_in localAddr, servAddr;
    struct hostent *h;
    
    h = gethostbyname("lhcb09.ca.infn.it");
    
    servAddr.sin_family = h->h_addrtype;
    memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    servAddr.sin_port = htons(SERVER_PORT);

    
    /* create socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd<0) { perror("cannot open socket "); exit(1); }
    
    /* connect to server */
    rc = connect(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));
    if(rc<0) { perror("cannot connect "); exit(1); }
    
    /* read data */
    rc = recv(sd, pippo, 1000, 0);
    if(rc<0) { perror("cannot read data "); close(sd); exit(1); }
    
    pippo[rc+1]='\0'; 
    //  printf("%s \n", pippo);
    close(sd);
    
    printf("%s\n",strtok(pippo,s2));
    j=0;
    while ((p = strtok(NULL, s2)) != NULL) {
      dtmp[j]=atof(p);
      //    printf("%lf\n",dtmp[j]);
      j++;
    }
    putchar('\n');
    
    for(j=0;j<58;j++) {
      printf("%lf\n",dtmp[j]);
    }
  
    
    //  close(sockfd);
    return;
  }

  void* readSlow::myLoop(char *buf) {
    int k;
    for(k=0; k<1; k++) {
      DavideClient(buf);
    }
    pthread_exit(NULL);
    return;
  }


  void* readSlow::myPrint(void* unused)
    {
      int i;
      char pippo[1050];
      //  DavideClient();
      printf("davide\n");
    }

  void* readSlow::createTh(char *buf) {
    pthread_t myThID;
    pthread_create(&myThID, NULL, myLoop(buf), NULL);
  }


void * slowControl::readData(void *evbuff) {

  int sd;
  int rc;

  /* Open a TCP socket */
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd<0) { cout << "Error creating socket " << endl; }

  /* connect to Slow Control Server */
  rc = connect(sd, (struct sockaddr *) &srvaddr, sizeof(srvaddr));
  if(rc<0) { cout << "Error connecting to Slow ControlServer " << endl; close(sd);}

  /* read data */
  rc = recv(sd, evbuff, MAX_MSG, 0);
  if(rc<0) { cout << "Error while reading data from Slow ControlServer " << endl; close(sd); }
}

void *  slowControl::threadLoop(char *evbuff) {
  for(int i=0;i<10;i++) {
    readData(evbuff);
    cout << evbuff << endl;
  }
}


/*****************************************/
// Thread  Commands
/*****************************************/
/* int slowControl::startThread() */
/* { */
/*   char buf[1000]; */
/*   pthread_t myThID; */
/*   pthread_create(&myThID, NULL, threadLoop, buf); */
/*   //  result = pthread_create(&myThID, NULL, threadLoop, evbuff); */
/*   //  pthread_exit(retval); /\* don't track it *\/ */
/*   return 0; */
/* } */


/*****************************************/
// Crap
/*****************************************/
void slowControl::print() 
{
  cout << "*************************************" << endl;  
  cout << "Slow Control SW Module v0.1 / port " << port() << " on node " << ipaddr() << endl;
  cout << "*************************************" << endl;
  return;
}
