#include "mySockServ.h"

void Die(const char* mess) { perror(mess); exit(1); }

#include <string.h>

#define BUFFSIZE 65536

tcpserv ts;
bool isthere(0);

void open(int port)
 {
  ts.tcp_open(port);
 }
bool accept()
 {
  if (isthere) return false;
  isthere = ts.tcp_accept("pcpisadrc.cern.ch");
 }
void disc()
 {
  if (isthere) ts.tcp_disconnect();
  isthere = false;
 }
int getcmd(char * cmd, size_t len)
 {
  accept();
  uint8_t sz;
  int nr = ts.tcp_recv(&sz, 1);
  if (nr != 1) fprintf(stdout, "error for size %d\n", nr);
  nr = ts.tcp_recv(cmd, sz);
  if (nr != sz) fprintf(stdout, "error for data %d\n", nr);
  if (nr == 0) disc();
  return nr;
 }

int main(int argc, char** argv)
 {
  if (argc != 2) {
    fprintf(stderr, "USAGE: echoserver <port>\n");
    exit(1);
  }
  open (atoi(argv[1]));
do
 {
  char buffer[BUFFSIZE];
  int nr;
  do
   {
    nr = getcmd(buffer, BUFFSIZE);

    if (nr > 0)
     {
      buffer[nr] = '\0';
      fprintf(stdout, "%s\n", buffer);
     }
   }
  while (nr != 0);
 }
while (1);
  return 0;
 }
