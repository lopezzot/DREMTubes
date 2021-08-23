//$Id: tcpClient.c,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
/* fpont 12/99 */
/* pont.net    */
/* tcpClient.c */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close */

#define SERVER_PORT 2055
#define MAX_MSG 100

void main (int argc, char *argv[]) {

  int sd, rc, i;
  char pippo[1024];
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;
  
  if(argc < 2) {
    printf("usage: %s <server>\n",argv[0]);
    exit(1);
  }

  h = gethostbyname(argv[1]);
  if(h==NULL) {
    printf("%s: unknown host '%s'\n",argv[0],argv[1]);
    exit(1);
  }

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
  
  pippo[rc+1]='\0'; printf("%s \n", pippo);
  close(sd);

  return;
}


