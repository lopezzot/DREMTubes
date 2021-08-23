//$Id: mySlowContRead.cpp,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
/****************************************/
// Standard C header files
/****************************************/
extern "C" {
#include <unistd.h>           // Needed for sleep
#include "myTcpClient.h"
}

int main (int argc, char *argv[]) {
  

  for(int i=0; i<100; i++){
    tcpClient(argc,argv);
    sleep(1);
  }
  
  return 0;
}
