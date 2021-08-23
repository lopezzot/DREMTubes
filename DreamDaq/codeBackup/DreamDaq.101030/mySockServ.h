#ifndef _MY_SOCK_SERV_H_
#define _MY_SOCK_SERV_H_

// Standard C header files
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
// C++ header files
#include <iostream>
#include <string>
using namespace std;

class tcpserv {
  
 public:
  
  // Constructor
  tcpserv ();
  ~tcpserv();
  void tcp_close();
  void tcp_open(uint16_t port);
  void tcp_disconnect();
  bool tcp_accept(const char * cname = 0);
  int tcp_recv (void * message, uint32_t size);
  int tcp_send (const void * message, uint32_t size);

 private:
  int m_socket;
  uint16_t m_port;
  int m_clientsock;
};

tcpserv::tcpserv()
 {
 }

tcpserv::~tcpserv()
 {
  if (m_clientsock) close(m_clientsock);
  if (m_socket) close(m_socket);
 }

void tcpserv::tcp_close()
 {
  if (m_clientsock) close(m_clientsock);
  if (m_socket) close(m_socket);
  m_socket = m_port = m_clientsock = 0;
 }

void tcpserv::tcp_open (uint16_t port)
 {
  int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
   {
    cout << "Failed to create socket: " << sock << endl;
    throw "Failed to create socket";
   }

  struct sockaddr_in sa_server;
  /* Construct the server sockaddr_in structure */
  memset(&sa_server, 0, sizeof(sa_server));       /* Clear struct */
  sa_server.sin_family = AF_INET;                  /* Internet/IP */
  sa_server.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
  sa_server.sin_port = htons(port);       /* server port */

  /* Bind the server socket */
  struct sockaddr * sp = (struct sockaddr *) &sa_server;
  int ret = bind(sock, sp, sizeof(sa_server));
  if (ret < 0)
   {
    cout << "Failed to bind the server socket " << ret << endl;
    throw "Failed to bind the server socket";
   }

  /* Listen on the server socket */
  if (listen(sock, 1) < 0)
    throw "Failed to listen on server socket";

  m_socket = sock;
  m_port = port;
 }

void tcpserv::tcp_disconnect()
 {
  if (m_clientsock) close(m_clientsock);
  m_clientsock = 0;
 }

bool tcpserv::tcp_accept(const char * cname)
 {
  if (m_clientsock != 0) return false;
  struct sockaddr_in sa_client;
  unsigned int clen = sizeof(sa_client);

  cout << "tcpserv::tcp_accept waiting\n";
  /* Wait for client connection */
  struct sockaddr * sp = (struct sockaddr *) &sa_client;
  int sock = accept(m_socket, sp, &clen);
  if (sock < 0)
    throw "Failed to accept client connection";

  char hbuf[100], sbuf[100];
  if (getnameinfo(sp, clen, hbuf, 100, sbuf, 100, 0) == 0)
    fprintf(stdout, "Client connected: %s %s\n", hbuf, sbuf);
  else
    fprintf(stdout, "Client connected: %s\n", inet_ntoa(sa_client.sin_addr));
  if (cname && (strcmp(hbuf,cname)!=0))
    close(sock);
  else{
    m_clientsock = sock;
    uint8_t flag = 1;
    int res = 
      setsockopt(m_clientsock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    if(res < 0 ){
      fprintf(stdout, "Cannot set TCP_NODELAY because: %s\n", strerror(errno));
    }
  }
  return (m_clientsock != 0);
 }

int tcpserv::tcp_recv (void * message, uint32_t size)
 {
  if (m_clientsock == 0) return -1;
  int received = recv(m_clientsock, message, size, MSG_WAITALL);
  return received;
 }

int tcpserv::tcp_send (const void * message, uint32_t size)
 {
  if (m_clientsock == 0) return -1;
  struct iovec iv;
  iv.iov_base = const_cast<void*>(message);
  iv.iov_len = size;
  struct msghdr hdr = {0, 0, &iv, 1, 0, 0, 0};
  uint32_t sent = ::sendmsg(m_clientsock, &hdr, MSG_NOSIGNAL);
  return sent;
 }

#endif // _MY_SOCK_SERV_H_
