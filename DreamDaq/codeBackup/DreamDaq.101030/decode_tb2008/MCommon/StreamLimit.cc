#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "StreamLimit.h"


StreamLimit::StreamLimit(int N) {
  int i;

  n   = N;
  val  = (unsigned int *) calloc(N, sizeof(unsigned int));
  mask = (unsigned int *) calloc(N, sizeof(unsigned int));

  min = 0;
  for (i=0; i<n; i++) {
    val[i] = 0;
    mask[i] = STREAM_ACTIV;
  }
}


StreamLimit::~StreamLimit() {

  free(val);
  free(mask);
}


void StreamLimit::reset() {
  int i;
  
  min = 0;
  for (i=0; i<n; i++) {
    val[i] = 0;
  }  
}


unsigned int StreamLimit::update(unsigned int Val, unsigned int Id = 0) {
  
  val[Id] = Val; 
  min = INT_MAX;

  for (int i=0; i<n; i++) {
    if ((val[i] < min) && (mask[i])) {
      min = val[i];
    }
  }
  return min;
}


unsigned int StreamLimit::update() {
  int i;

  min = INT_MAX;

  for (i=0; i<n; i++) {
    if ((val[i] < min) && (mask[i])) {
      min = val[i];
    }
  }

  return min;
}


unsigned int StreamLimit::limit() {

  return min;
}


void StreamLimit::set_limit(unsigned int Val, unsigned int Id = 0) {

  val[Id] = Val;   
}

void StreamLimit::set_mask(unsigned int Mask, unsigned int Id = 0) {

  mask[Id] = Mask;
}

