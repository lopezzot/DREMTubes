#ifndef _MY_SOCK_SERV_UDP_H_
#define _MY_SOCK_SERV_UDP_H_

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
#include <sys/poll.h>
//#include <netinet/udp.h>
// C++ header files
#include <iostream>
#include <string>
using namespace std;

class udpsock {
  
 public:
  
  // Constructor
  udpsock ();
  ~udpsock();
  void udp_close();
  void udp_open(const char* peername, uint16_t port);
  int udp_recv (void * message, uint32_t size);
  int udp_recv (void * message, uint32_t size, uint32_t timeout);
  int udp_send (const void * message, uint32_t size);

 private:
  int m_socket;
  struct sockaddr_in* m_peer;
  struct addrinfo* m_result;
};

udpsock::udpsock():
  m_socket(0),
     m_peer(0),
     m_result(0){
}

udpsock::~udpsock()
 {
   this->udp_close();
 }

void udpsock::udp_close()
 {
   if (m_socket){ 
     close(m_socket);
     freeaddrinfo(m_result);
   }
   
   m_socket = 0;
 }

void udpsock::udp_open (const char * peername, uint16_t port)
 {
   //create socket
   if ((m_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
     throw "Failed to create socket";
   }

   //Bind the local port
   struct sockaddr_in localaddr;
   memset(&localaddr, 0, sizeof(localaddr));       /* Clear struct */
   localaddr.sin_family = AF_INET;                  /* Internet/IP */
   localaddr.sin_addr.s_addr = htonl(INADDR_ANY);   /* Any IP address */
   localaddr.sin_port = htons(port);                /* server port */
   
   /* Bind the socket */
   size_t serverlen = sizeof(localaddr);
   if (bind(m_socket, (struct sockaddr *) &localaddr, serverlen) < 0) {
     throw "Failed to bind";
   }
       
   struct addrinfo hints;
   
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = 0;
   hints.ai_flags = 0;
   hints.ai_protocol = 0;
   
   int s = getaddrinfo(peername, NULL, &hints, &m_result);
   if (s != 0){
     string ermess("getaddrinfo: ");
     ermess += gai_strerror(s);
     throw ermess;
   }
   
   m_peer = (struct sockaddr_in *)m_result->ai_addr;
   m_peer->sin_family = AF_INET;         /* Internet/IP */
   m_peer->sin_port = htons(port);       /* server port */
 }


int udpsock::udp_recv (void * message, uint32_t size, uint32_t timeout){

  struct pollfd pollopt;
  
  pollopt.fd = m_socket;
  pollopt.events = POLLIN;
  pollopt.revents = 0;

  if(!poll(&pollopt,1,timeout))
    return -1;
  
  struct sockaddr_in client;
  socklen_t clientlen = sizeof(client);
  return recvfrom(m_socket, message, size, 0,
		  (struct sockaddr *)&client, &clientlen);

}

int udpsock::udp_recv (void * message, uint32_t size)
 {
   struct sockaddr_in client;
   socklen_t clientlen = sizeof(client);
   return recvfrom(m_socket, message, size, MSG_WAITALL,
		   (struct sockaddr *)&client, &clientlen);
 }

int udpsock::udp_send (const void * message, uint32_t size)
 {
   return sendto(m_socket, message, size, 0, 
		 (struct sockaddr *) m_peer, sizeof(*m_peer));
 }

#endif // _MY_SOCK_SERV_UDP_H_
