#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "MLog.h"

MLog::MLog(char logpath[]) {
  char logfile[FILENAME_MAX_SIZE];
  time_t time_now_secs;
  struct tm *time_now;

  pthread_mutex_init (&mlog_mutex, NULL);

  time(&time_now_secs);
  time_now = localtime(&time_now_secs);

  // open log file
  sprintf(logfile, "%s/DominoDaq_%04d_%02d_%02d_%02d_%02d_%02d.log", 
	  logpath,
	  1900 + time_now->tm_year,
	  1 + time_now->tm_mon,
	  time_now->tm_mday,
	  time_now->tm_hour,
	  time_now->tm_min,
	  time_now->tm_sec);
  if ((logptr = fopen(logfile, "w")) == NULL) {
    fprintf(stderr,"MLog::MLog: Could not open file %s\n", logfile);
    exit(1);
  }  

  LogWrite("Start Logfile\n");
}


MLog::~MLog() {
  LogWrite("End Logfile\n");
  fclose(logptr);
  pthread_mutex_destroy (&mlog_mutex);
}


int MLog::LogWrite(char logtext[], int console_flag, int colflag) {
  time_t time_now_secs;
  struct tm *time_now;

  time(&time_now_secs);
  time_now = localtime(&time_now_secs);

  pthread_mutex_lock(&mlog_mutex);
  fprintf(logptr, "%04d:%02d:%02d:%02d:%02d:%02d: %s",
	  1900 + time_now->tm_year,
	  1 + time_now->tm_mon,
	  time_now->tm_mday,
	  time_now->tm_hour,
	  time_now->tm_min,
	  time_now->tm_sec,
	  logtext);
  fflush(logptr);

//   if (console_flag == 1) {
//     fprintf(stderr, "%s", logtext);
//   }

 if (console_flag == 1) {
    if (colflag) ftextcol(stderr, colflag);
    fprintf(stderr, "%s", logtext);
    if (colflag) ftextcolreset(stderr);
  }
  pthread_mutex_unlock(&mlog_mutex);

  return 0;
}
