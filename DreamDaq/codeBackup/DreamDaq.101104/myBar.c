//$Id: myBar.c,v 1.2 2006/08/05 15:29:04 cvsdream Exp $
/*************************************************************************

        myBar.c
        -------

	Simple progress bar, with ETA estimate.
	The bar prints on stederr and is automatically 
	according to user requests...
	
        Version 0.1,      A.Cardini 7/6/2001

*************************************************************************/
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <math.h>

#include "myBarp.h"
#include "myBar.h"

// Some definitions
#define MAXSIZE 40
#define AVERAGESIZE 100

// Global variables
int maxCounts = 0;            // Target counts
int nowCounts = 0;            // Current number of counts

/************************************************************************/
void alrmHandler(int sig)
/************************************************************************/
{
  printProgressBar();
  signal(SIGALRM, alrmHandler);       
}


/************************************************************************/
int initProgressBar(int max, int msec)
/************************************************************************/
{
  struct itimerval in, out;

  maxCounts = max;

  // Initialize timer interval to msec
  in.it_interval.tv_sec = (int)(msec/1000);
  in.it_interval.tv_usec = (int)((1000*msec)%1000000);
  in.it_value.tv_sec = (int)(msec/1000);
  in.it_value.tv_usec = (int)((1000*msec)%1000000);

  setitimer(ITIMER_REAL, &in, &out); 
  
  // On SIGALRM call alarm handler
  signal(SIGALRM, alrmHandler);       

  return 0;
}

/************************************************************************/
int  updateProgressBar(int cnt)
/************************************************************************/
{
  nowCounts = cnt;
  return 0;
}

/************************************************************************/
int  resetProgressBar()
/************************************************************************/
{
  struct itimerval in, out;

  // Reset timer
  in.it_interval.tv_sec = 0;
  in.it_interval.tv_usec = 0;
  in.it_value.tv_sec = 0;
  in.it_value.tv_usec = 0;

  setitimer(ITIMER_REAL, &in, &out); 
  
  return 0;
}

/************************************************************************/
void printProgressBar()
/************************************************************************/
{
  char buffer[100];
  char bar[MAXSIZE+1];
  int rot;
  double pctg;                // percentage completed
  static int rotation = 0; 
  static int prev = 0;
  static unsigned int firsttime;
  static unsigned int lasttime = 0;    // time of previous call
  struct timeval now;         // time from gettimeofday
  unsigned int thistime;
  double eta;
  int i, last;
  static int first = 1;

  gettimeofday(&now, NULL);
  thistime = now.tv_sec*1000 + now.tv_usec/1000;       // time in ms
  if (first) { firsttime = thistime; first = 0; }
  
  // Percentage Completed
  pctg = 100.*(double)nowCounts/(double)maxCounts;

  // ETA Calculation: this is an average on all the run, so possible
  // idle states can increase the value, this ETA should be considered
  // as an upper limit of remaining time...

  if (nowCounts < prev) 
    eta = -1;         // infinite time to complete run
  else
    eta = 0.001* (double)(maxCounts-nowCounts)*(double)(thistime-firsttime)/(double)nowCounts;
  
  // Rotation character
  switch (rotation) {
  case 0:
    rot = 45;
    break;
  case 1:
    rot = 92;
    break;
  case 2:
    rot = 124;
    break;
  default:
    rot = 47;
  }

  // Make progress bar

  last = (int)(MAXSIZE*pctg/100.);
  for (i=0; i<last; i++) bar[i] = 45;
  bar[last++] = 62;
  for (i=last; i<MAXSIZE; i++) bar[i] = 32;
  bar[MAXSIZE] = '\0';

  if (eta>99999 || eta == -1) // Simple patch to avoid bad formatting...
    sprintf(buffer, "    %c  [%40s] %5.1f %% done, ETA: not available (no triggers?)", rot, bar, pctg);
  else 
    sprintf(buffer, "    %c  [%40s] %5.1f %% done, ETA: %5.0f s                     ", rot, bar, pctg, eta);

  // Now print it on stderr
  fprintf(stderr, "%s\r", buffer);

  // This before exiting from the function
  prev = nowCounts;
  lasttime = thistime;
  ++ rotation;
  rotation %= 4;
}

/************************************************************************/
unsigned int runningAverage(unsigned int this, int avg)
/************************************************************************/
{
  static int items = 0;  
  static int curr = 0;
  static unsigned int mprev;
  static unsigned int val[AVERAGESIZE];
  unsigned int tot;
  int i;

  // Protection
  if (avg > AVERAGESIZE) avg = AVERAGESIZE;

  if (items<avg) items++;

  // Insert items
  val[curr++] = this; 
  curr = curr%avg;

  // Mean
  tot = 0;
  for (i=0; i<items; i++) {
    tot += val[i];
  }
  mprev = tot/items;
  return mprev;

}


