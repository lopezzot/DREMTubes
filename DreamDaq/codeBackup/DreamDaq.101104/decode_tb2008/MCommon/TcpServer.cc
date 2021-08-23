/////////////////////////////////////////////////////////////////////////
// TcpServer
//
// Description: Class which sets up a server to transfer data via TCP/IP
// 
// create: Florian Goebel   November 2006
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h> 
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include "socklib.h"
#include "TcpServer.h"

#define noDEBUG

#define MAX_READ_COUNTS 5
#define MAX_TRIALS  20


TcpServer::TcpServer(int iport) {
  fPort = iport;

  fSendFlags = MSG_NOSIGNAL;
  /*  Use send instead of write because it allows to set this flag.
      This flag prevents crash due to "Broken pipe". 
      MSG_NOSIGNAL
      Requests not to send SIGPIPE on errors on stream  oriented  
      sockets  when  the  other  end breaks the connection. 
      The EPIPE error is still returned.    
  */

  fSocket = NULL;
  fConnected = 0;
}


int TcpServer::Connect(int *condition) {

  /* loop for ever trying to connect to server */
  while (((condition==NULL) || (*condition)) && (!fConnected)) {
    
    /* Create client socket */ 
    if (!fSocket) {
      fSocket = sopen();
    }
    if (!fSocket) {
      sleep(2);
      continue;
    }
    
    if ((fSocketDescriptor = sserver(fSocket, fPort, S_DELAY)) >= 0) {
      fConnected = 1;
      break;
    }
    
    if (fSocket) sclose(fSocket);
  }
  
  return fConnected;
}



int TcpServer::Read(void *ptr, int nBytesRequest, FILE *logptr) {
  int nBytesRemain = nBytesRequest;
  int nBytesReadTot = 0;
  int nBytesRead = 0;
  int count;

#ifdef DEBUG
  struct timeval time;
  struct timezone tz;
  int icount = 0;
#endif

  count = MAX_TRIALS*MAX_READ_COUNTS;

  while (count-->0) {

#ifdef DEBUG
    gettimeofday(&time, &tz);
    fprintf(logptr, "(%d:%06d) TcpServer: start read %d Bytes\n", 
	    (int)time.tv_sec, (int)time.tv_usec, nBytesRequest);
#endif

    nBytesRead = read(fSocketDescriptor, (void *)((int)ptr+nBytesReadTot), nBytesRemain);
    if (nBytesRead < 0) {
#ifdef DEBUG
      perror("TcpServer(read))");
#endif
      break;
    }    

    nBytesReadTot += nBytesRead; 
    nBytesRemain = nBytesRequest - nBytesReadTot;

#ifdef DEBUG
    gettimeofday(&time, &tz);
    fprintf(logptr, "(%d:%06d) TcpServer: count: %d got %d bytes\n",
	    (int)time.tv_sec, (int)time.tv_usec, icount++, nBytesReadTot);
#endif

    if (nBytesRemain == 0) {
      break;
    }

    if (count%MAX_TRIALS==0) {
      usleep(100000);   // wait 0.1 sec
    }

  }

#ifdef DUMP
  static int first=1;
  static FILE *dptr;
  if (first==1) {
    first = 0;
    dptr = fopen("dump.out", "w");
  }
  fwrite(ptr, nBytesReadTot, 1, dptr);
#endif

  return nBytesReadTot;
}


int TcpServer::ReadLine(char *ptr, int maxlen) {

  int nBytesRead = sreadline(fSocketDescriptor, ptr, maxlen);

  if (nBytesRead <= 0) {
    fConnected = 0;
  }

  return nBytesRead;

}
