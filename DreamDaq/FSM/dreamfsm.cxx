
#include <ctime>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <stdint.h>
#include "dreamDaqClass.h"

inline void nnsleep( uint32_t ndelay )
 {
  struct timespec tsleep = { 0, ndelay };
  nanosleep(&tsleep, NULL);
 }
 
volatile sig_atomic_t quit_run(0);
char message[256]={0};

static void cntrl_c_handler(int32_t sig)
 {
  time_t timestr(time(NULL));
  char* stime = ctime(&timestr);
  stime[24] = 0;
  fprintf(stderr,"%s exiting run on ctrl-c [%s]\n\n", stime, message);
  quit_run = 1;
 }

void* doWork ( void* argv )
 {
  dreamDaq* ddq = (dreamDaq*)argv;

  ddq->Run();

  pthread_exit(0);
  return NULL;
 }

int32_t main(int32_t argc, char** argv)
 {
  pthread_t m_thread;
  pthread_attr_t attr;
  int32_t rc;
  void *status;

  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  argc --; argv ++;
  dreamDaq* ddq = new dreamDaq( argc, argv );

  printf("Main: creating thread\n");
  rc = pthread_create(&m_thread, &attr, doWork, (void *)ddq); 
  if (rc)
   {
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
   }
  pthread_attr_destroy(&attr);

  signal(SIGINT, cntrl_c_handler);

  while ( !quit_run ) { nnsleep( 1000000 ); }

  ddq->exitPlease();

  /* Free attribute and wait for the other threads */
  rc = pthread_join(m_thread, &status);
  printf("Thread joined\n");
  if (rc)
   {
    printf("ERROR; return code from pthread_join() is %d\n", rc);
    exit(-1);
   }
  printf("Main: thread joined with status %ld. Exiting.\n",(long)status);

  pthread_exit(NULL);
  return 0;
 }
