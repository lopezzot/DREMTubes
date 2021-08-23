//$Id: myBarp.h,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#ifndef _MYBARP_
#define _MYBARP_
/*************************************************************************

        myBarp.h
        --------

        Version 0.1,      A.Cardini 7/6/2001

*************************************************************************/

#include <stddef.h>
#include <time.h>
#include <stdio.h>

// Private function prototypes

void alrmHandler(int sig);
void printProgressBar();
unsigned int runningAverage(unsigned int this, int avg);

/************************************************************************/

#endif


