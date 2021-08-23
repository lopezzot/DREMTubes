#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mrunnum.h"

unsigned int mrunnum() {
  FILE *fptr;
  unsigned int runnum;

  if ((fptr = fopen(MRUNFILE, "r")) == NULL) {
    fprintf(stderr, "mrunnum: Could not open file: %s for reading\n", MRUNFILE);
    exit(0);
  }
  fscanf(fptr, "%d", &runnum);
  fclose(fptr);

  runnum++;

  if ((fptr = fopen(MRUNFILE, "w")) == NULL) {
    fprintf(stderr, "mrunnum: Could not open file: %s for writing\n", MRUNFILE);
    exit(0);
  }
  fprintf(fptr, "%d", runnum);
  fclose(fptr);

  /* introduce sync here to make sure .mrunnum is actually written to tape */
  /* this might cure the observed problem of double runnumbers */
  sync();

  return runnum;
}
