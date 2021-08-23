#include "mySockClient.h"

void Die(const char* mess) { perror(mess); exit(1); }

#include <string.h>

int main(int argc, char** argv)
 {
  tcpclient * tc;
  if (argc != 4) {
    fprintf(stderr, "USAGE: TCPecho <server name> <port> <string>\n");
    exit(1);
  }
  tc = new tcpclient(argv[1]);
  tc->tcp_connect(atoi(argv[2]));
  char * bffr = (char*)malloc(65536);
  strcpy(bffr,argv[3]);
do
 {
  size_t echolen = strlen(bffr);
  size_t cnt = tc->tcp_send(bffr,65000);
  if (cnt != 65000)
   {
    free(bffr);
    delete tc;
printf("Mismatch in number of sent bytes %d != 65000\n", cnt);
    Die("Mismatch in number of sent bytes");
   }

  printf("enter new string:");
  scanf("%s",bffr);
} while (strcmp(bffr,"quit")!=0);
  free(bffr);

  delete tc;
  return 0;
 }
