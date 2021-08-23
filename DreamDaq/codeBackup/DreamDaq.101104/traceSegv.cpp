
 #include <execinfo.h>
 #include <csignal>
 #include <cstdlib>

 #include <exception>
 #include <stdexcept>
 #include <iostream>

#define ERR_MESS(A) do { std::cerr << A << std::endl; } while(0)

static std::string mystr;

void sighandler ( int sig )
 {
  void * array[64];
  int32_t nSize = backtrace(array, 64);
  char ** symbols = backtrace_symbols(array, nSize);
  ERR_MESS(mystr);
  for (int32_t i = 2; i < nSize; i++)
   {
    ERR_MESS( symbols[i] );
   }
  free(symbols);
  signal(SIGABRT, SIG_DFL);
  exit(sig);
 }

class myDebug
 {
  public:
  myDebug(int sig) { signal(sig, sighandler); }
 };

static myDebug agoo(SIGSEGV);

void setMess(const char * mess) { mystr = std::string(mess); }

