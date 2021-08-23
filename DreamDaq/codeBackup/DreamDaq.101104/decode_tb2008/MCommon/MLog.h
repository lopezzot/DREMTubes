#ifndef MLOG_H_SEEN
#define MLOG_H_SEEN

#include <stdio.h>
#include <pthread.h>
#include "MagicTypes.h"
#include "textcolor.h"

#define MAX_LOG_SIZE 1000

class MLog {

 public:
  MLog(char logpath[]);
  ~MLog();
  
  int LogWrite(char logtext[], int console_flag = 0, int colflag = 0);
  
 public:
  FILE *logptr;
  char logtext[MAX_LOG_SIZE];

 private:
  pthread_mutex_t mlog_mutex;
};

#endif
