
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <unistd.h>

int main()
 {
  char hname[1024];
  int rc = gethostname(hname, 1024);
  std::string hnamestr(hname);
  if ((rc != 0) || 
      ((hnamestr != "pcdreamdaq2") && (hnamestr != "pcdreamdaq2.cern.ch") &&
       (hnamestr != "pcdreamdaq") && (hnamestr != "pcdreamdaq.cern.ch") &&
       (hnamestr != "pcdreamus") && (hnamestr != "pcdreamus.cern.ch")))
   {
    std::cout << rc << " wrong hostname " << hname << ": should be one of [pcdreamdaq, pcdreamdaq.cern.ch, pcdreamdaq2, pcdreamdaq2.cern.ch, pcdreamus, pcdreamus.cern.ch]\n";
    return 2;
   }
  char oldname[] ="/home/dreamtest/working/daqguilog.txt";
  char newname[] ="/home/dreamtest/working/daqguilog.sav";
  int result= rename( oldname , newname );
  system("DaqControlInterface 2>&1 >/home/dreamtest/working/daqguilog.txt &");
  return result;
 }

