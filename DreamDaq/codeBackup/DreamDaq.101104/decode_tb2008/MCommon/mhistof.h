#ifndef MHISTOF_H_SEEN
#define MHISTOF_H_SEEN

#include <stdio.h>

class mhistof {

 public:
   mhistof(char *name, int aSize);
   ~mhistof();

   void  reset(char *name=NULL);
   void  fill(float *vals);
   void  fill(int i, float val);
   float mean(int i);
   float rms(int i);
   void  means(float *means);
   void  rmss(float *rmss);
   int   print(FILE *fptr, int option=0);
   inline int num(int i) {return n[i];};

 private:
   char *name;
   int asize;
   int   *n;
   float *sum;
   float *sum2;
};

#endif
