//$Id: runThread.cpp,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
int stop=1;

extern "C" {
#include <unistd.h>           
#include "myThread.h"
}


int main() {
  int j;
  int ret;
  int buf=1;
  ret=startSlowControlThread();
  for(j=0;j<15;j++) {
    sleep(2);
    printf("%i %i %i\n", j,  readSlowDate(), readSlowTime());
    if(j==2) stop=0;
    usleep(10);
  }

  printf("%i\n", readDetCG1_dwVmon());
  //  stop();
  return 1;
}
