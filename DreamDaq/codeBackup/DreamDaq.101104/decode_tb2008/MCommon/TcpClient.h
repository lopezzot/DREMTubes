/////////////////////////////////////////////////////////////////////////
// TcpClient
//
// Description: Class which sets up a server to transfer date via TCP/IP
// 
// created: Florian Goebel   November 2006
/////////////////////////////////////////////////////////////////////////

#ifndef TCP_CLIENT_H_SEEN
#define TCP_CLIENT_H_SEEN

#include "socklib.h"
#include "MagicTypes.h"

class TcpClient {
  
 public:
  TcpClient(char *hostname, int iport);
  ~TcpClient();
 
  int Connect(int *condition=NULL);
  int Send(char *report, int len=0);
  int SendReport(char *report);

  int SD() {return fSocketDescriptor;};
  int IsConnected() {return fConnected;};
  int SetDisconnected() {fConnected=0; return fConnected;};
  
 private:
  char fHostname[HOSTNAME_SIZE];
  int fPort;

  SOCKET *fSocket;
  int fSendFlags;
  int fSocketDescriptor;
  int fConnected;
};

#endif





















