/*************************************************************************

        doOfflineHisto.cxx
        -----------

        re-run the monitoring software offline

        Version 0.1,      M. Cascella 2010 
*********************************************************************/

#include "../myRawFile.h"

#define BUF_SIZE 30000 // 30kbyte

#include <string.h>
#include <stdio.h>
#include "../dreammon.hxx"
#include <iostream>

/************************************************************************/
int main(int argc, char **argv)
     /************************************************************************/
{
  unsigned int buf[BUF_SIZE];
  char datadir[256];
  char datafilename[256];
  unsigned int run_nr;
  bool phys;
  
  if(argc<3){
    std::cout << "Usage: " << argv[0] << " runnumber datatype"
	      << " (pedestal or data)" << std::endl;
    exit(1);
  }

  run_nr = atoi(argv[1]);

  if(run_nr == 0){
    std::cout << "first argument (" << argv[1] << ")must be the run number/n/n"
    exit(2);
  }

  if(!strcmp(argv[2],"pedestal")){
    phys=false;
  }else if(!strcmp(argv[2],"data")){
    phys=true;
  }else{
    std::cout << "Second argument (" << argv[2] << ") must be \"pdestal\" or \"data\"" << std::endl;
    exit(3);
  }

  // Create Input/Output filenames
  if (getenv("DATADIR") == NULL){
    if(phys){
      sprintf(datadir, "/home/dreamtest/working/data");
      sprintf(datafilename, "%s/datafile_run%s.dat", datadir, argv[1]);
    }else{
      sprintf(datadir, "/home/dreamtest/working/pedestal");
      sprintf(datafilename, "%s/pedestal_run%s.dat", datadir, argv[1]);
    }
  }else
    sprintf(datadir, "%s", getenv("DATADIR"));

    // Open Data file
  rc = RawFileOpen(datafilename);
  if (rc) {
    printf("doOfflineHisto: cannot open data file %s\n", argv[1]);
    return -1;
  }

  dreammon monitor(run_nr,phys);

  // Main Loop on all events
  for (;;) {
    // Read Event and write it in buffer 'buf'
    int rc = RawFileReadEvent(buf);
    if (rc == RAWDATAEOF) { 
      printf("Found EOF at event %d\n", GetEventNumber()); 
      break; 
    }
    if (rc == RAWDATAUNEXPECTEDEOF) { 
      printf("Unexpected EOF at event %d\n", GetEventNumber()); 
      break; 
    }
    rc = 0;  //
    unsigned int evtnum   = GetEventNumber();
    unsigned int doSingle = ((evtnum % evt_interval) == 1); 
    monitor.event(evtnum,buf);
  } // End of loop forever
  monitor.exit();
  // Close Data file
  RawFileClose();

  return 0;
}
