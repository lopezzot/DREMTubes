
#include <time.h>
#include <string>
#include <iostream>

std::string tprint()
 {
  time_t tt(time(NULL));
  char tbuf[26];
  ctime_r( &tt, tbuf);
  tbuf[24] = '\0';
  return std::string(tbuf);
 }
using namespace std;
int main()
 {
  cout << tprint() << " - Run ending" << endl;
  return 0;
 } 
