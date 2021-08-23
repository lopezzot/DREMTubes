
#include <stdio.h>
#include <stdlib.h>
#include "myRunNumber.h"


int updateRunNumber(){

  FILE *ifp;
  int tmp, n;
  char * workingdir;
  char myRunNumberFile[128];

  if (getenv("WORKDIR") == NULL)
    workingdir = getenv("PWD");
  else
    workingdir = getenv("WORKDIR");

  sprintf(myRunNumberFile, "%s/%s", workingdir, RUN_NUMBER_FILE);

  // First find current run number...
  ifp = fopen(myRunNumberFile, "r+");
  if (!ifp) {
    fprintf(stderr, "%s %s %s\n", __PRETTY_FUNCTION__, " cannot open file ",
         myRunNumberFile);
    return -1;
  }
  n = fscanf(ifp, "%10d\n", &tmp);  // read the only thing which is written...
  if (n != 1) {
    fprintf(stderr, "%s %s %s\n", __PRETTY_FUNCTION__, " wrong format of file ",
         myRunNumberFile);
    return -1;
  }
  fseek(ifp, 0, 0);             // go back at the beginning...
  tmp++;
  fprintf(ifp, "%10d\n", tmp);  // ...and write back current run number
  fclose(ifp);
  printf("update - RUN NUMBER IS %d\n", tmp);
  return tmp;
}

int readRunNumber(){

  FILE *ifp;
  int tmp, n;
  char * workingdir;
  char myRunNumberFile[128];

  if (getenv("WORKDIR") == NULL)
    workingdir = getenv("PWD");
  else
    workingdir = getenv("WORKDIR");

  sprintf(myRunNumberFile, "%s/%s", workingdir, RUN_NUMBER_FILE);

  // First find current run number...
  ifp = fopen(myRunNumberFile, "r");
  if (!ifp) {
    fprintf(stderr, "%s %s %s\n", __PRETTY_FUNCTION__, " cannot open file ",
         myRunNumberFile);
    return -1;
  }
  n = fscanf(ifp, "%10d\n", &tmp);  // read the only thing which is written...
  if (n != 1) {
    fprintf(stderr, "%s %s %s\n", __PRETTY_FUNCTION__, " wrong format of file ",
         myRunNumberFile);
    return -1;
  }
  fclose(ifp);
  printf("read - RUN NUMBER IS %d\n", tmp);
  return tmp;
}

