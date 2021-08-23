/////////////////////////////////////////////////////////////////////////
// TcpServer
//
// Description: Class which sets up a server to transfer date via TCP/IP
// 
// created: Florian Goebel   November 2006
/////////////////////////////////////////////////////////////////////////

#ifndef TCP_SERVER_H_SEEN
#define TCP_SERVER_H_SEEN

#include "socklib.h"

class TcpServer {
  
 public:
  TcpServer(int iport);

  int Connect(int *condition=NULL);
  int Read(void *ptr, int nBytesRequest, FILE *logptr=stderr); //diego: this is probably not needed
  int ReadLine(char *ptr, int maxlen=1000);

  int SD() {return fSocketDescriptor;};
  int IsConnected() {return fConnected;};
  
 private:
  int fPort;

  SOCKET *fSocket;
  int fSendFlags;
  int fSocketDescriptor;
  int fConnected;
};

#endif





















