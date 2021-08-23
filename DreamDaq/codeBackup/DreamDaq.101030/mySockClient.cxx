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
class tcpclient {
  
 public:
  
  // Constructor
  tcpclient (const char * host);
  ~tcpclient();
  void tcp_connect(uint16_t port);
  int tcp_recv (char * message, uint32_t size);
  int tcp_send (const char * message, uint32_t size);
  void tcp_close();

 private:
  std::string m_name;
  in_addr_t m_haddr;
  int m_sock;
  uint16_t m_port;
};

tcpclient::tcpclient(const char * host) : m_name(host)
 {
  struct addrinfo hints;
  struct addrinfo* result;
  struct sockaddr_in* srv;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = 0;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  int s = getaddrinfo(host, NULL, &hints, &result);
  if (s != 0)
   {
    string ermess("getaddrinfo: ");
    ermess += gai_strerror(s);
    throw ermess;
   }

  srv = (struct sockaddr_in *)result->ai_addr;
  m_haddr = srv->sin_addr.s_addr;
  m_sock = m_port = 0;
 }

tcpclient::~tcpclient()
 {
  if (m_sock) close(m_sock);
 }

void tcpclient::tcp_close()
 {
  if (m_sock) close(m_sock);
  m_sock = m_port = 0;
 }

void tcpclient::tcp_connect (uint16_t port)
 {
  int sock;
  struct sockaddr_in echoserver;

  /* Create the TCP socket */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    throw "Failed to create socket";

  /* Construct the server sockaddr_in structure */
  memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
  echoserver.sin_family = AF_INET;                  /* Internet/IP */
  echoserver.sin_addr.s_addr = m_haddr;
  echoserver.sin_port = htons(port);                /* server port */
  /* Establish connection */
  while (connect(sock, (struct sockaddr *) &echoserver,
                 sizeof(echoserver)) < 0) usleep(10000);
  m_sock = sock;
  m_port = port;
 }

int tcpclient::tcp_recv (char * message, uint32_t size)
 {
  if (m_sock == 0) return -1;
  /* Receive message */
  int received(0);
  int ret(0);
  int flags = fcntl(m_sock, F_GETFL, 0);
  assert(flags != -1);
  fcntl(m_sock, F_SETFL, flags & ~O_NONBLOCK);
  do
   {
    ret = recv(m_sock, message, size, 0);
    if (ret <= 0) break;
    message[ret] = '\0';
    fprintf(stdout, "GOT: %x %s\n", ret, message);
    received += ret;
    message += ret;
    size -= ret;
    fcntl(m_sock, F_SETFL, flags | O_NONBLOCK);
   } while (size > 0);
  return received;
 }

int tcpclient::tcp_send (const char * message, uint32_t size)
 {
  if (m_sock == 0) return -1;

  int sent(0);
  int ret(0);
  while (size > BUFFSIZE)
   { 
    ret = send(m_sock, message, BUFFSIZE, 0);
    if (ret < 0) return ret;
    if (ret == 0) break;
    sent += ret;
    message += ret;
    size -= ret;
   }
  if (size) ret = send(m_sock, message, size, 0);
  if (ret < 0) return ret;
  sent += ret;
  return sent;
 }
