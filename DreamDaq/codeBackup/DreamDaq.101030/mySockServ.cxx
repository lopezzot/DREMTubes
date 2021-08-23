/*****************************************

*****************************************/

// Standard C header files
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
// C++ header files
#include <iostream>
#include <string>
using namespace std;

#define BUFFSIZE 65536

/*****************************************/
/*****************************************/
class tcpserv {
  
 public:
  
  // Constructor
  tcpserv ();
  ~tcpserv();
  void tcp_close();
  void tcp_open(uint16_t port);
  void tcp_disconnect();
  void tcp_accept();
  int tcp_recv (char * message, uint32_t size);
  int tcp_send (const char * message, uint32_t size);

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

void tcpserv::tcp_accept()
 {
  struct sockaddr_in sa_client;
  unsigned int clen = sizeof(sa_client);

  cout << "tcpserv::tcp_accept waiting\n";
  /* Wait for client connection */
  struct sockaddr * sp = (struct sockaddr *) &sa_client;
  m_clientsock = accept(m_socket, sp, &clen);
  if (m_clientsock < 0)
    throw "Failed to accept client connection";

  char hbuf[100], sbuf[100];
  if (getnameinfo(sp, clen, hbuf, 100, sbuf, 100, 0) == 0)
    fprintf(stdout, "Client connected: %s %s\n", hbuf, sbuf);
  else
    fprintf(stdout, "Client connected: %s\n", inet_ntoa(sa_client.sin_addr));
 }

int tcpserv::tcp_recv (char * message, uint32_t size)
 {
  if (m_clientsock == 0) return -1;
  /* Receive message */
  int received(0);
  int ret(0);
  int flags = fcntl(m_clientsock, F_GETFL, 0);
  assert(flags != -1);
  fcntl(m_clientsock, F_SETFL, flags & ~O_NONBLOCK);
  do
   {
    ret = recv(m_clientsock, message, size, 0);
    if (ret <= 0) break;
    message[ret] = '\0';
    fprintf(stdout, "GOT: %x %s\n", ret, message);
    received += ret;
    message += ret;
    size -= ret;
    fcntl(m_clientsock, F_SETFL, flags | O_NONBLOCK);
   } while (size > 0);
  if (ret == 0) this->tcp_disconnect();
  return received;
 }

int tcpserv::tcp_send (const char * message, uint32_t size)
 {
  if (m_clientsock == 0) return -1;

  int sent(0);
  int ret(0);
  while (size > BUFFSIZE)
   {
    ret = send(m_clientsock, message, BUFFSIZE, 0);
    if (ret <= 0) return ret;
    sent += ret;
    message += ret;
    size -= ret;
   }
  if (size) ret = send(m_clientsock, message, size, 0);
  if (ret < 0) return ret;
  return sent;
 }
