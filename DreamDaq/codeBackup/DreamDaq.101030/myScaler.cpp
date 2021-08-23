//$Id: myScaler.cpp,v 1.2 2006/08/05 15:32:51 cvsdream Exp $
/*****************************************

  myScaler.cpp
  ----------

  Small test program.

  D. Raspino / Universita' di Cagliari and INFN Cagliari

  Version 0.1: 17/9/2004

*****************************************/



/****************************************/
// Standard C header files
/****************************************/
extern "C" {
#include <unistd.h>           // Needed for usleep
#include <stdio.h>
}

/****************************************/
// C++ header files
/****************************************/
#include <iostream>
#include <fstream>
#include <signal.h>
using namespace std;

/****************************************/
// My header files with all classes
/****************************************/
#include "myVme.h"       // VME Base class
#include "myV513.h"      // CAEN I/O register V513 Base class
#include "myV792AC.h"    // CAEN ADC V792AC
#include "myV767A.h"     // CAEN TDC V767A
#include "myV488.h"      // CAEN TDC V488A
#include "myV260.h"      // CAEN TDC V260
/****************************************/
// Declare all the hardware
/****************************************/
v513    dio1(0x0C0000,"/dev/vmedrv24d16");
v792ac  adc1(0x040000,"/dev/vmedrv24d16");
v767a   tdc1(0x010000,"/dev/vmedrv24d16");
v767a   tdc2(0x020000,"/dev/vmedrv24d16");
v488    tdc0(0x0D0000,"/dev/vmedrv24d16");
v260    sca1(0x0B0000,"/dev/vmedrv24d16");
/****************************************/




/****************************************/
void bufdump(unsigned int *buf, int size)
/****************************************/
{
  int temp;
  for (int i=0; i<10; i++) {
    temp=buf[i]&0xFFFFFF;
    cout << "     Channel  " << dec << i << " Counts: " << dec << temp  << dec << endl;
  }
  for (int i=10; i<size; i++) {
    temp=buf[i]&0xFFFFFF;
    cout << "     Channel " << dec << i << " Counts: " << dec << temp  << dec << endl;
  }

  return;
}


/****************************************/
int main(int argc, char **argv)
/****************************************/

    
{
  int sz;
  unsigned int buf[1000];	
  int gate;

  ios::sync_with_stdio();          // To syncronize with stdio library (really needed?)


  /* Check number of parameters */

  if (argc!=2) {
    printf("\n%s: Usage is %s [gate] in usec\n\n", argv[0], argv[0]);
    return -1;
  }

  gate=atoi(argv[1]);

  cout << endl << endl;
  cout << "************************************"<< endl;
  cout << "************************************"<< endl;
  cout << "*                                  *"<< endl;
  cout << "*                                  *"<< endl;
  cout << "*            My Scaler             *"<< endl;
  cout << "*                                  *"<< endl;
  cout << "*                                  *"<< endl;

  // Scaler Init
  sca1.clear();
  sca1.inhibitReset();
  sca1.scaleIncrement(); 
  usleep(gate);

  sca1.inhibitSet();
  

  if (sca1.inhibitGet()) {               // Only readout the scalers when the module is inhibited...
    sz = sca1.readEvent(buf);
    sca1.clear();                        // ... then clear it...
    bufdump(buf, sz);
  }

  cout << "*                                  *"<< endl;
  cout << "*                                  *"<< endl;
  cout << "************************************"<< endl;
  cout << "************************************"<< endl;

//   tdc0.setHighThr(0xC0);
  return 0;
}













