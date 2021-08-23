
#include <sys/time.h>
#include <sys/resource.h>
#include <csignal>

#include <iostream>
#include <sstream>

#include "myV2718.h"

using namespace std;

v2718 v2718m(0,"/V2718/cvA24_U_DATA/0");

uint64_t rsec, rusec;
uint64_t dsec, dusec;

struct itimerval del1, del2;
struct rusage usage1, usage2;

void alarm_wakeup1(int32_t i);
void alarm_wakeup2(int32_t i);
uint64_t m_which(0), udt1(0), udt2(0);

void alarm_wakeup1(int32_t i)
 {
  // cout << " got 1 " << i << endl;
  getrusage(RUSAGE_SELF, &usage1);
  m_which = 1;
  udt2 = (usage1.ru_utime.tv_sec + usage1.ru_stime.tv_sec)*1000000
        + (usage1.ru_utime.tv_usec + usage1.ru_stime.tv_usec)
        - (usage2.ru_utime.tv_sec + usage2.ru_stime.tv_sec)*1000000
        - (usage2.ru_utime.tv_usec + usage2.ru_stime.tv_usec);
  signal(SIGALRM,alarm_wakeup2);
  if (0)
  cout << " got 1 " << i << " udt2 " << udt2
       << " (rsec*1000000+rusec) " << (rsec*1000000+rusec)
       << " (dsec*1000000+dusec) " << (dsec*1000000+dusec)
       << endl;
  if (udt2 > (rsec*1000000+rusec))
   {
    if (rusec) {
     if ((rsec > 0) || (rusec > 1)) rusec --;
    } else {
     rsec --; rusec = 999999;
    }
    del2.it_interval.tv_sec = rsec;
    del2.it_interval.tv_usec = rusec;
    del1.it_value.tv_sec = rsec;
    del1.it_value.tv_usec = rusec;
   }
  if (udt2 < (rsec*1000000+rusec))
   {
    if (rusec < 999999) {
     rusec ++;
    } else {
     rsec ++; rusec = 0;
    }
    del2.it_interval.tv_sec = rsec;
    del2.it_interval.tv_usec = rusec;
    del1.it_value.tv_sec = rsec;
    del1.it_value.tv_usec = rusec;
   }
  setitimer(ITIMER_REAL, &del2,0);
  v2718m.setOutputRegister(0x100);
  return;
 }

void alarm_wakeup2(int32_t)
 {
  getrusage(RUSAGE_SELF, &usage2);
  m_which = 2;
  udt1 = (usage2.ru_utime.tv_sec + usage2.ru_stime.tv_sec)*1000000
        + (usage2.ru_utime.tv_usec + usage2.ru_stime.tv_usec)
        - (usage1.ru_utime.tv_sec + usage1.ru_stime.tv_sec)*1000000
        - (usage1.ru_utime.tv_usec + usage1.ru_stime.tv_usec);
  signal(SIGALRM,alarm_wakeup1);
  if (udt1 > (dsec*1000000+dusec))
   {
    if (dusec) {
     if ((dsec > 0) || (dusec > 1)) dusec --;
    } else {
     dsec --; dusec = 999999;
    }
    del1.it_interval.tv_sec = dsec;
    del1.it_interval.tv_usec = dusec;
    del2.it_value.tv_sec = dsec;
    del2.it_value.tv_usec = dusec;
   }
  if (udt1 < (dsec*1000000+dusec))
   {
    if (dusec < 999999) {
     dusec ++;
    } else {
     dsec ++; dusec = 0;
    }
    del1.it_interval.tv_sec = dsec;
    del1.it_interval.tv_usec = dusec;
    del2.it_value.tv_sec = dsec;
    del2.it_value.tv_usec = dusec;
   }
  setitimer(ITIMER_REAL, &del1,0);
  v2718m.clearOutputRegister(0x100);
  return;
 }

int32_t main( int32_t argc, char** argv )
 {
  uint16_t chan;
  double rate, dur;
  if (argc != 4)
   {
    cout << "Usage " << argv[0] << " <channel> <rate[Hz]> <duration[usec]" << endl;
    exit(1);
   }
  stringstream ss (stringstream::in | stringstream::out);
  ss << argv[1] << " " << argv[2] << " " << argv[3];
  ss >> chan >> rate >> dur;
  cout << " chan " << chan << " rate " << rate << " duration " << dur << endl;
  double xusec = 0.5+1e+6/rate - dur;
  rsec = uint64_t(xusec*1e-6);
  rusec = uint64_t(xusec-1e+6*double(rsec));
  cout << " delay 1 " << rsec << ". " << rusec << endl;
  double yusec = 0.5+dur;
  dsec = uint64_t(yusec*1e-6);
  dusec = uint64_t(yusec-1e+6*double(dsec));
  cout << " delay 2 " << dsec << ". " << dusec << endl;
  del1.it_interval.tv_sec = dsec;
  del1.it_interval.tv_usec = dusec;
  del1.it_value.tv_sec = rsec;
  del1.it_value.tv_usec = rusec;
  del2.it_interval.tv_sec = rsec;
  del2.it_interval.tv_usec = rusec;
  del2.it_value.tv_sec = dsec;
  del2.it_value.tv_usec = dusec;
  getrusage(RUSAGE_SELF, &usage1);
  getrusage(RUSAGE_SELF, &usage2);
  setitimer(ITIMER_REAL, &del1, 0);
  signal(SIGALRM,alarm_wakeup1);
  while(1)
   {
   }
  return 0;
 }
