/////////////////////////////////////////////////////////////////////////
// TcpClient
//
// Description: Tcp/IP client to transfer data
// 
// create: Florian Goebel   November 2006
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h> 
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "socklib.h"
#include "TcpClient.h"


TcpClient::TcpClient(char *hostname, int iport) {

  sprintf(fHostname, "%s", hostname);
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

#ifdef MAC
  // This is actually only needed on MAC
  //  since the MSG_NOSIGNAL for send does not work
  // When the connection is closed by he remote host a call to send
  //  terminates the process with the SIGPIPE signal
  // Therefore the SIGPIPE signal has to be ignored
  struct sigaction sigact;
  sigact.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &sigact, NULL);
#endif
}


TcpClient::~TcpClient() {
  if (fSocket) sclose(fSocket);
}



int TcpClient::Connect(int *condition) {

  /* loop for ever trying to connect to server */
  while (((condition==NULL) || (*condition)) && (!fConnected)) {
    
    /* Create client socket */ 
    fSocket = sopen();
    if (!fSocket) {
      sleep(2);
      continue;
    }
    
    if ((fSocketDescriptor = sclient(fSocket, fHostname, fPort)) >= 0) {
      fConnected = 1;
      break;
    }
    
    if (fSocket) sclose(fSocket);
  }
  
  return fConnected;
}


int TcpClient::Send(char *report, int len) {

  if (len==0) {
    len = strlen(report);
  }

  int nBytesSent = send(fSocketDescriptor, report, len, fSendFlags);
  //  printf("DIEGO send report IN Send; len%d, sent%d\n",len, nBytesSent);      
  // All bytes sent?
  if (nBytesSent != len) {
    fConnected = 0;
  }      

  return fConnected;
}



int TcpClient::SendReport(char *report) {
  char Report[MAX_REPORT_LENGTH];
  time_t time_now_secs;
  struct tm *time_now;

  time(&time_now_secs);
  time_now = localtime(&time_now_secs);

  sprintf(Report, "%04d:%02d:%02d:%02d:%02d:%02d: %s",
	  1900 + time_now->tm_year,
	  1 + time_now->tm_mon,
	  time_now->tm_mday,
	  time_now->tm_hour,
	  time_now->tm_min,
	  time_now->tm_sec,
	  report);

  return Send(Report);
}

