#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MX 19
#define NX (512*1024)

typedef struct { double real, imag; } complex;

void FFT ( complex A[], int m, int INV ) {

/* This is a c-adaption by                                  */ 
/* Bernard Metsch                                           */
/* from the FORTRAN programme FFT                           */
/* from Paul. L. DeVries, Computerphysik,                   */
/* Spektrum Akademischer Verlag,                            */
/* Heidelberg 1994,                                         */
/* which is in turn an adaption from                        */
/* a FORTRAN Code of the Cooley & Tukey method by:          */
/* Cooley, Lewis and Welch, IEEE Transactions E-12          */
/* 1965                                                     */

/* The array A contains the complex data to be transformed, */
/* `m' is log2(N), and INV is an index = 1 if the inverse   */
/* transform is to be computed (The forward transform is    */
/* evaluated if INV is not = 1).                            */

  complex u, w, t;
  double ang;
  int N, Nd2, i, j, k, l, le, le1, ip;

/*  This routine computes the Fast Fourier Transform of the   */
/*  input data and returns it in the same array. Note that    */
/*  the k's and x's are related in the following way:         */
/*                                                            */
/*    IF    K = range of k's      and     X = range of x's    */
/*                                                            */
/*    THEN  delta-k = 2 pi / X    and   delta-x = 2 pi / K    */
/*                                                            */
/*  When the transform is evaluated, it is assumed that the   */
/*  input data is periodic. The output is therefore periodic  */
/*  (you have no choice in this). Thus, the transform is      */
/*  periodic in k-space, with the first N/2 points being      */
/*  'most significant'. The second N/2 points are the same    */
/*  as the Fourier transform at negative k!!! That is,        */
/*                                                            */
/*              FFT(N+1-i) = FFT(-i)  ,i = 1,2,....,N/2       */
/*                                                            */
  N   = pow(2,m);
  Nd2 = N/2;
  j   = 1;
  for (i=1; i<N; i++) {
    if (i<j) { t = A[j-1]; A[j-1] = A[i-1]; A[i-1] = t; }
    k = Nd2;
    while (k<j) { 
      j -= k; 
      k /= 2; 
    } 
    j += k;
  }
  le = 1;       
  for (l=1; l<=m; l++) {
    le1 = le;
    le  += le;
    
    u.real = 1.0; u.imag = 0.0;
    ang = M_PI / le1;
    w.real = cos(ang); w.imag = -sin(ang);
    if (INV==1) w.imag = -w.imag;
    
    for (j=1; j<=le1; j++) {
      for (i=j; i<=N; i+=le) {
	ip = i + le1;
	t.real = A[ip-1].real * u.real
	       - A[ip-1].imag * u.imag;
	t.imag = A[ip-1].real * u.imag
	       + A[ip-1].imag * u.real;
	A[ip-1].real = A[i-1].real - t.real;
	A[ip-1].imag = A[i-1].imag - t.imag;
	A[i -1].real = A[i-1].real + t.real;
	A[i -1].imag = A[i-1].imag + t.imag;
      }
      t.real = u.real * w.real
	     - u.imag * w.imag;
      t.imag = u.real * w.imag
             + u.imag * w.real;
      u = t;
    }
  }
  if (INV!=1) for (i=0; i<N; i++) {
    A[i].real /= N; 
    A[i].imag /= N; 
  }
} /* end FFT */

int main(int argc,char **argv){

    int i, j;
    int n, m, n2;
    complex F[NX];
    double a;

    n = 0;

    while ( scanf("%lg",&a)!=EOF ) {
	F[n].real = a;
	F[n].imag = 0.0;
	if (n==NX) {
	    printf("too many data points!\n");
	    return 0 ;
	}
	n++;
    }
    printf("# number of data points read: %d\n",n);

    // determine m:
    
    j = 1;
    m = -1;
    for (i=0; i<=MX; i++) {
	if (j==n) {
	    m = i;
	}
	j *=2;
    }
    if (m==-1) {
	printf("# number of data points not a power of 2 !!!\n");
	return 0;
    } else {
	printf("# log_2(%d) = %d\n", n, m);
    }

    FFT ( F, m, 0 );
    
    n2 = n / 2;
    
    for (i=0; i<n; i++) {
	printf("%lg + %lg * i\n",F[i].real,F[i].imag);
    }

    return 0;

} /* main */
