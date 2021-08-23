//$Id: prova.c,v 1.1.1.1 2006/07/28 08:05:51 vandelli Exp $
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>

#include "myVme.h"       // VME Base class
#include "myCorbo.h"      // Creative Electronic Systems RCB 8047 CORBO class derived from VME
#include "myV792AC.h"    // CAEN ADC V792AC class derived from VME
#include "myKLOETDC.h"    // 32ch KLOETDC class derived from VME
#include "mySIS3320.h"   // Flash ADC
#include "myModules.h"


using namespace std;

int main(){

  sis3320 fadc(0x10000000,"/dev/vmedrvb32d32");

  int fd = open("datafile", O_RDWR);

  void * buf=mmap(0,0x1FFFFFF,PROT_READ,MAP_SHARED,fd,0x0);

  if(errno){
    cout << "myVme: Cannot mmap at address "<< endl;
    if(errno==ENOMEM)
      cout<< "ENOMEM" << endl;
    if(errno==EINVAL)
      cout<< "EINVAL" << endl;
    if(errno==EAGAIN)
      cout<< "EAGAIN" << endl;
    exit(1);
  }

  munmap(buf,0x1FFFFFF);
  return 0;
  
}
