#include "mySockServ.h"

void Die(const char* mess) { perror(mess); exit(1); }

#include <string.h>

#define BUFFSIZE 65536

int main(int argc, char** argv)
 {
  tcpserv * ts;
  if (argc != 2) {
    fprintf(stderr, "USAGE: echoserver <port>\n");
    exit(1);
  }
  ts = new tcpserv();
  ts->tcp_open(atoi(argv[1]));
do
 {
  ts->tcp_accept();

  char buffer[BUFFSIZE];
  int nr;
  do
   {
    nr = ts->tcp_recv(buffer, BUFFSIZE);

    if (nr > 0)
     {
      buffer[nr] = '\0';
      fprintf(stdout, "%s\n", buffer);
     }
   }
  while (nr >= 0);
 }
while (1);
  delete ts;
  return 0;
 }
