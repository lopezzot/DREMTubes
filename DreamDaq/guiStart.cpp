
#include <unistd.h>
#include <stdint.h>

#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

using namespace std;

int32_t main()
 {
  char hname[1024];
  int32_t rc = gethostname(hname, 1024);
  string hnamestr(hname);
  if ((rc != 0) ||
      ((hnamestr != "pcdreamdaq2") && (hnamestr != "pcdreamdaq2.cern.ch") &&
       (hnamestr != "pcdreamdaq") && (hnamestr != "pcdreamdaq.cern.ch") &&
       (hnamestr != "pcdreamus") && (hnamestr != "pcdreamus.cern.ch")))
   {
    cout << rc << " wrong hostname " << hname << ": should be one of [pcdreamdaq, pcdreamdaq.cern.ch, pcdreamdaq2, pcdreamdaq2.cern.ch, pcdreamus, pcdreamus.cern.ch]" << endl;
    return 2;
   }
  cout << " starting GUI on hostname " << hname << endl;
  char oldname[] ="/home/dreamtest/working/daqguilog.txt";
  char newname[] ="/home/dreamtest/working/daqguilog.sav";
  int32_t result= rename( oldname , newname );
  cout << " logfile " << oldname << " moved to " << newname << " " << result << endl;
  system("DaqControlInterface 2>&1 >/home/dreamtest/working/daqguilog.txt &");
  return result;
 }

