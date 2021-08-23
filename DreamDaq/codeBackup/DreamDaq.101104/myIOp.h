//$Id: myIOp.h,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#ifndef _MYIOP_
#define _MYIOP_
/*************************************************************************

        myIOp.h
        -------

        This file contains event structures definitions

        Version 0.1,      A.Cardini 6/6/2001
        Version 0.2,      D.Raspino 27/7/2005 Add data from SLow Control

*************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

// Data Structures Definitions

typedef struct {
  int magic;            // Magic word - 0xaabbccdd
  int ruhsiz;           // Run Header size in bytes
  int runnumber;        // Run number
  int evtsinrun;        // Events in this run (if available)
  time_t begtim;        // Time at beginning of run
  time_t endtim;        // Time at end of run
} RunHeader;

typedef struct {
  int evmark;           // Beginning of event marker - 0xcafecafe
  int evhsiz;           // Event Header size in bytes
  int evsiz;            // Event size in bytes (including header)
  int evnum;            // Event number
  int tsec;             // Seconds in the day from gettimeofday
  int tusec;            // Microseconds in the day from gettimeofday
  int gaina;             // Gain Detector A
  int vmoncata;          // V mon Cathode Detector A
  int vmong1ua;          // V mon Gem 1 Up Detector A
  int vmong1da;          // V mon Gem 1 Dw Detector A
  int vmong2ua;          // V mon Gem 2 Up Detector A
  int vmong2da;          // V mon Gem 2 Dw Detector A
  int vmong3ua;          // V mon Gem 3 Up Detector A
  int vmong3da;          // V mon Gem 3 Dw Detector A
  int vmonanoa;          // V mon Anode Detector A
  int imoncata;          // I mon Cathode Detector A
  int imong1ua;          // I mon Gem 1 Up Detector A
  int imong1da;          // I mon Gem 1 Dw Detector A
  int imong2ua;          // I mon Gem 2 Up Detector A
  int imong2da;          // I mon Gem 2 Dw Detector A
  int imong3ua;          // I mon Gem 3 Up Detector A
  int imong3da;          // I mon Gem 3 Dw Detector A
  int imonanoa;          // I mon Anode Detector A
  int gainb;             // Gain Detector B
  int vmoncatb;          // V mon Cathode Detector B
  int vmong1ub;          // V mon Gem 1 Up Detector B
  int vmong1db;          // V mon Gem 1 Dw Detector B
  int vmong2ub;          // V mon Gem 2 Up Detector B
  int vmong2db;          // V mon Gem 2 Dw Detector B
  int vmong3ub;          // V mon Gem 3 Up Detector B
  int vmong3db;          // V mon Gem 3 Dw Detector B
  int vmonanob;          // V mon Anode Detector B
  int imoncatb;          // I mon Cathode Detector B
  int imong1ub;          // I mon Gem 1 Up Detector B
  int imong1db;          // I mon Gem 1 Dw Detector B
  int imong2ub;          // I mon Gem 2 Up Detector B
  int imong2db;          // I mon Gem 2 Dw Detector B
  int imong3ub;          // I mon Gem 3 Up Detector B
  int imong3db;          // I mon Gem 3 Dw Detector B
  int imonanob;          // I mon Anode Detector B
  int gainc;             // Gain Detector C
  int vmoncatc;          // V mon Cathode Detector C
  int vmong1uc;          // V mon Gem 1 Up Detector C
  int vmong1dc;          // V mon Gem 1 Dw Detector C
  int vmong2uc;          // V mon Gem 2 Up Detector C
  int vmong2dc;          // V mon Gem 2 Dw Detector C
  int vmong3uc;          // V mon Gem 3 Up Detector C
  int vmong3dc;          // V mon Gem 3 Dw Detector C
  int vmonanoc;          // V mon Anode Detector C
  int imoncatc;          // I mon Cathode Detector C
  int imong1uc;          // I mon Gem 1 Up Detector C
  int imong1dc;          // I mon Gem 1 Dw Detector C
  int imong2uc;          // I mon Gem 2 Up Detector C
  int imong2dc;          // I mon Gem 2 Dw Detector C
  int imong3uc;          // I mon Gem 3 Up Detector C
  int imong3dc;          // I mon Gem 3 Dw Detector C
  int imonanoc;          // I mon Anode Detector C
  int altitude;         // Altitude
  int dewpoint;         // Dew Point
  int humidity;         // Humidity
  int pressure;         // Pressure
  int qnh;              // QNH
  int temp1;            // Temperature 1
  int temp2;            // Temperature 2
  int toverp;           // Ratio between Temperature (Kelvin) and Pressure (hPa)
  int arflux;           // Argon Flux
  int co2flux;          // CO2 Flux
  int cf4flux;          // CF4 Flux
  int slowdate;         // Date on Slow Control
  int slowtime;         // Time on Slow Control

  int vmonpmup;         // Vmon PM Up
  int imonpmup;         // Imon PM Up
  int vmonpmdw;         // Vmon PM Dw
  int imonpmdw;         // Imon PM Dw

  int vmonpm1;          // Vmon PM 1
  int imonpm1;          // Imon PM 1
  int vmonpm2;          // Vmon PM 2
  int imonpm2;          // Imon PM 2

  int vmondetd;         // Vmon Divider Det D
  int imondetd;         // Imon Divider Det D
  int vmondete;         // Vmon Divider Det E
  int imondete;         // Imon Divider Det E  
} EventHeader;

typedef struct {
  unsigned int semk;    // Beginning of subevent marker - 0xacabacab
  int sevhsiz;          // Subevent Header size in bytes
  unsigned int id;      // Module ID
  int          size;    // Subevent size in bytes
} SubEventHeader;

/************************************************************************/
#endif












