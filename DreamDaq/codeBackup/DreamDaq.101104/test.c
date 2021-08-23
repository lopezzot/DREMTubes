//$Id: test.c,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>         /* per gethostbyname() */
#include <stdio.h>

main()
{
  struct sockaddr_in srvaddr;
  struct hostent *h;

  char string[]="192.168.0.1";

  inet_aton(string, &(srvaddr.sin_addr)); /* converte da decimal-dotted notation in struct in_addr */
  srvaddr.sin_port=77;
  srvaddr.sin_family = AF_INET;
  
  printf("Address is %lu, port is %d\n", srvaddr.sin_addr.s_addr, srvaddr.sin_port);
  printf("Address is %s, port %d\n", inet_ntoa(srvaddr.sin_addr), srvaddr.sin_port);

  h = gethostbyname(string);
  printf("Host %s\n", h->h_name);
  
  return 0;
}
