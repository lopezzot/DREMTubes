//$Id: myIO.h,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#ifndef _MYIO_
#define _MYIO_
/*************************************************************************

        myIO.h
        ------

        This file contains all related I/O stuff prototypes
        and useful definitions...

        Version 0.1,      A.Cardini 3/6/2001

*************************************************************************/

#include <stddef.h>
#include <time.h>

// Function prototypes

int myMaxEvents();
int myDownscaleFactor();
int myReloadTDCConfig();
int myOpenRun();
int myCloseRun();
int myWriteEvent();
int myUpdateRunEventNumber();
int myNewEvent();
int myFormatSubEvent(unsigned int *src, int size, unsigned int moduleId);

/************************************************************************/
#endif


