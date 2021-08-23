#include <unistd.h>
#include <execinfo.h>

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <exception>
#include <stdexcept>
#include <iostream>
#include <string>

using namespace std;

#define ERR_MESSCPP(A) do { cerr << A << endl; } while(0)
#define ERR_MESSC(A) do { write(2, A, strlen(A)); write(2, "\n", 1); } while(0)

static string mystr("");

static string timeprint()
 {
  time_t tt(time(NULL));
  char tbuf[26];
  ctime_r( &tt, tbuf);
  tbuf[24] = '\0';
  return string(tbuf);
 }

void printtrace()
 {
  void * array[64];
  int32_t nSize = backtrace(array, 64);
  char ** symbols = backtrace_symbols(array, nSize);
  ERR_MESSC(timeprint().c_str());
  if (mystr != "") ERR_MESSC(mystr.c_str());
  for (int32_t i = 2; i < nSize; i++)
   {
    ERR_MESSC( symbols[i] );
   }
  free(symbols);
 }

void catch_siginfo(int32_t /* signum */ )
{
    sigset_t mask_set;	/* used to set a signal masking set. */
    sigset_t old_set;	/* used to store the old mask set.   */

    /* re-set the signal handler again to catch_int, for next time */
    signal(SIGPWR, catch_siginfo);
    /* mask any further signals while we're inside the handler. */
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set);
    ERR_MESSC("*** GOT SIGINFO ***");
    printtrace();
}

void catch_sigsegv(int32_t /* signum */ )
{
    sigset_t mask_set;	/* used to set a signal masking set. */
    sigset_t old_set;	/* used to store the old mask set.   */

    /* mask any further signals while we're inside the handler. */
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set);
    ERR_MESSC("*** GOT SIGSEGV ***");
  if (mystr != "") ERR_MESSC(mystr.c_str());
    printtrace();
    ERR_MESSC("*** GOING TO ABORT ***");
    abort();
}

class myDebug
 {
  public:
  myDebug() { signal(SIGPWR, catch_siginfo); signal(SIGSEGV, catch_sigsegv);}
 };

static myDebug agoo;

void setMess(const char * mess) { mystr = string(mess); }

