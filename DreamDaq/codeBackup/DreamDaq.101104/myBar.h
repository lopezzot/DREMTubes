//$Id: myBar.h,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#ifndef _MYBAR_
#define _MYBAR_
/*************************************************************************

        myBar.h
        -------

        Version 0.1,      A.Cardini 7/6/2001

*************************************************************************/

// Public function prototypes

int  initProgressBar(int max, int msec);
int  updateProgressBar(int cnt);
int  resetProgressBar();

/************************************************************************/

#endif


