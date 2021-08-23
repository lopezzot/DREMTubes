#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mhistof.h"


mhistof::mhistof(char *Name, int aSize) {

  name = (char *) calloc(strlen(Name)+1, sizeof *name);
  sprintf(name, "%s", Name);

  asize = aSize;

  n    = (int *)   calloc(asize, sizeof *n);
  sum  = (float *) calloc(asize, sizeof *sum);
  sum2 = (float *) calloc(asize, sizeof *sum2);

  reset();
}


mhistof::~mhistof() {
  free(name);
  free(n);
  free(sum);
  free(sum2);
}


void mhistof::reset(char *Name) {
  if (Name != NULL) {
    name = (char *)realloc(name, (strlen(Name)+1) * sizeof *name);
    sprintf(name, "%s", Name);    
  }
  for (int i=0; i<asize; i++) {
    sum[i] = sum2[i] = 0;
    n[i] = 0;
  }
}


void mhistof::fill(float *vals) {
  for (int i=0; i<asize; i++) {
    n[i]++;
    sum[i] += vals[i];
    sum2[i] += vals[i]*vals[i];
  }
}

void mhistof::fill(int i, float val) {
  n[i]++;
  sum[i] += val;
  sum2[i] += val*val;
}


float mhistof::mean(int i) {
  if (n[i] > 0) {
    return sum[i]/n[i];
  } else {
    return 0;
  }
}


float mhistof::rms(int i) {
  if (n[i] > 0) {
    float val = sum2[i]/n[i] - sum[i]*sum[i]/n[i]/n[i];
    if (val>0) {
      return sqrt(val);
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}


void mhistof::means(float *means) {
  for (int i=0; i<asize; i++) {
    if (n[i] > 0) {
      means[i] += sum[i]/n[i];
    } else {
      means[i] = 0;
    }
  }
}


void mhistof::rmss(float *rms) {
  for (int i=0; i<asize; i++) {
    if (n[i] > 0) {
      float val = sum2[i]/n[i] - sum[i]*sum[i]/n[i]/n[i];
      if (val>0) {
	rms[i] = sqrt(val);
      } else {
	rms[i] = 0;
      }
    } else {
      rms[i] = 0;
    }
  }
}


int mhistof::print(FILE *fptr, int option) {

  fprintf(fptr, "%s\n", name);

  if (option == 0) {// profile option
    for (int i=0; i<asize; i++) {
      fprintf(fptr, "%3d %7.2f %7.2f\n", i+1, mean(i), rms(i));
    }
  } else if (option == 1) {  // also print entries
    for (int i=0; i<asize; i++) {
      fprintf(fptr, "%3d %7.2f %7.2f  %5d\n", i+1, mean(i), rms(i), n[i]);
    }
  } else { // counting option
    for (int i=0; i<asize; i++) {
      fprintf(fptr, "%3d %5d\n", i+1, n[i]);
    }
  }

  return 0;
}
