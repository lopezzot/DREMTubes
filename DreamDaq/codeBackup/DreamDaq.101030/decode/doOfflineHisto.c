/*************************************************************************

        myDaq2Ntu.c
        -----------

        myDaq --> Ntuple conversion program

        Version 0.1,      A.Cardini C. Deplano 29/11/2001

                          E. Meoni L. La Rotonda 25/07/2006

                          M. Cascella 2010
*********************************************************************/

#include "../myRawFile.h"

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "../dreammon.h"
#include <iostream>


/************************************************************************/
void printHelp()
     /************************************************************************/
{
  printf("\nmyDaq2Ntu: usage is myDaq2Ntu [runnumber]\n\n"); 
  return;
}

/************************************************************************/
int main(int argc, char **argv)
     /************************************************************************/
{
  int rc;
  unsigned int buf[100000];
  char datadir[256];
  char datafileprefix[256];
  char datafilename[256];
  unsigned int events = 0, run_nr;
  bool phys;
  int evt_interval=1000;
  
  if(argc<3){
    std::cout << "Usage: " << argv[0] << " runnumber datatype"
         << " (pedestal or data)" << std::endl;
    exit(1);
  }

  if(!strcmp(argv[2],"pedestal")){
    phys=false;
  }else if(!strcmp(argv[2],"data")){
    phys=true;
  }else{
    std::cout << "Unknown data type: " << argv[1] << std::endl;
    exit(1);
  }


  bool drs;
  bool drs_fb_lk;
  bool drs_matrix;

  drs=false;
  drs_fb_lk=false;
  drs_matrix=false;

  bool phys_h;
  int drs_setup=0;



/* DRS 2008
  if (argc==4) {
  if(!strcmp(argv[3],"drs")){
    drs=true;
  }
  if(!strcmp(argv[3],"nodrs")){
    drs=false;
  }
  }
DRS 2008*/
  if (argc<4) {
   drs=false;
   drs_fb_lk=false;
   drs_matrix= false;
   drs_setup=0;
  }
  if (argc==4) {
    if(!strcmp(argv[3],"drs_fb_lk")){
      drs=true;
      drs_fb_lk=true;
      drs_matrix=false;
      drs_setup=1;
    }
    if(!strcmp(argv[3],"nodrs")){
      drs= false;
      drs_fb_lk=false;
      drs_matrix=false;
      drs_setup=0;
    }
    if(!strcmp(argv[3],"drs_matrix")){
      drs= true;
      drs_fb_lk=false;
      drs_matrix=true;
      drs_setup=2;
    }
  }

  run_nr = atoi(argv[1]);

  if ((drs==true)&&(drs_fb_lk==true)&&(drs_matrix==false)) {drs_setup=1;}
  if ((drs==true)&&(drs_fb_lk==false)&&(drs_matrix==true)) {drs_setup=2;}
  if  (drs==false) {drs_setup=0;}

  int max_evt = -1;
  if (getenv("MAXEVT") != NULL){
    max_evt = atoi(getenv("MAXEVT"));
  }

  // Create Input/Output filenames
  if (getenv("DATADIR") == NULL){
    sprintf(datadir, "/home/dreamtest/working/data");
  }else{
    sprintf(datadir, "%s", getenv("DATADIR"));
  }
  if(phys){
    phys_h=true;
    sprintf(datadir, "/home/dreamtest/working/data");
    sprintf(datafileprefix, "%s/datafile_run%d", datadir, run_nr);
  }else{
    phys_h=false;
    sprintf(datadir, "/home/dreamtest/working/pedestal");
    sprintf(datafileprefix, "%s/pedestal_run%d", datadir, run_nr);
  }
  
  bool multifile = false;
  int filecount = 0;
  sprintf(datafilename, "%s.dat", datafileprefix);

  // Open Data file
  rc = RawFileOpen(datafilename);
  if (rc==-1) {
    multifile = 
    printf("doOfflineHisto: %s not found\n", datafilename);
    sprintf(datafilename, "%s_%d.dat", datafileprefix,filecount);
    printf("doOfflineHisto: trying to open %s\n", datafilename);
    rc = RawFileOpen(datafilename);
  }

  if (rc==-1) {
    printf("doOfflineHisto: cannot open data file for run %d\n", run_nr);
    return -1;
  }

  dreammon_init(argv, run_nr,drs,drs_setup,phys_h);

  // Main Loop on all events
  for (;;) {

    // Read Event and write it in buffer 'buf'
    rc = RawFileReadEvent(buf);
    if (rc == RAWDATAEOF) { 
      printf("Found EOF at event %d\n", GetEventNumber()); 
    } else if (rc == RAWDATAUNEXPECTEDEOF) { 
      printf("Unexpected EOF at event %d\n", GetEventNumber()); 
    }
    if (multifile && (rc == RAWDATAEOF | rc == RAWDATAUNEXPECTEDEOF )) {
      RawFileClose(); 
      filecount++;
      sprintf(datafilename, "%s_%d.dat", datafileprefix,filecount);
      rc = RawFileOpen(datafilename);
      if(rc==-1){
	break;
      }
    }
    unsigned int evtnum   = GetEventNumber();
    if(max_evt>0 & max_evt<evtnum) break;
    unsigned int doSingle = ((evtnum % evt_interval) == 1); 
    dreammon_event(doSingle,events++,buf,drs,drs_setup);
  } // End of loop forever
  dreammon_exit(0,drs,drs_setup);
  // Close Data file
  if(rc!=-1)RawFileClose();

  return 0;
}
