//$Id: myTest.cpp,v 1.2 2008/07/05 12:21:55 dreamdaq Exp $
/*****************************************

  myTest.cpp
  ----------

  Small test program.

  D. Raspino / Universita' di Cagliari and INFN Cagliari
  A. Cardini / INFN Cagliari

  Version 0.1: 24/1/2003

*****************************************/



/****************************************/
// Standard C header files
/****************************************/
extern "C" {
#include <unistd.h>           // Needed for usleep
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
#include "myVme.h"          // VME Base class
#include "myV513.h"         // CAEN I/O register V513 Base class
#include "myV792AC.h"       // CAEN ADC V792AC
#include "myV767A.h"        // CAEN TDC V767A
#include "myV488.h"         // CAEN TDC V488A
#include "myV260.h"         // CAEN TDC V260
#include "myV258.h"
#include "myTriggerNumber.h"
//#include "mySlowControl.h"  // GEM Slow Control
/****************************************/
// Declare all the hardware
/****************************************/
/*v513    dio1(0x0C0000,"/dev/vmedrv24d16");
v792ac  adc1(0x040000,"/dev/vmedrv24d16");
v767a   tdc1(0x010000,"/dev/vmedrv24d16");
v767a   tdc2(0x020000,"/dev/vmedrv24d16");
v488    tdc0(0x0D0000,"/dev/vmedrv24d16");
v260    sca1(0x0B0000,"/dev/vmedrv24d16");*/
//vTriggerNumber vtt(0x00100000,"/dev/vmedrv32d32");
v258 disc(0xFF0000, "/dev/vmedrv24d16");
//slowControl gemSlow("192.135.33.128", 2055);
/****************************************/


// /****************************************/
// void dumpADC(unsigned int *buf, int sz)
// /****************************************/
// {
//   int i;
//   int adc, un, ov, cha, geo, nch, cra, evn;
//   unsigned int data;

//   for (i=0; i<sz; i++) {
//     data = buf[i];
//     switch((data>>24)&0x7) {
//     case 0x0:
//       adc = data&0xFFF;
//       ov  = (data>>12)&0x1;
//       un  = (data>>13)&0x1;
//       cha = (data>>16)&0x3F;
//       geo = (data>>27)&0x1F;
//       cout << "<DATA> geo "<<dec<<geo<<" cha "<<cha<<" adc "<<adc<<" ov "<<ov<<" un "<<un<<endl; 
//       break;
//     case 0x2:
//       nch = (data>>8)&0x3F;
//       cra = (data>>16)&0xFF;
//       geo = (data>>27)&0x1F;
//       cout << "<HEAD> geo "<<dec<<geo<<" cra "<<cra<<" nch "<<nch<<endl; 
//       break;
//     case 0x4:
//       evn = data&0xFFFF;
//       geo = (data>>27)&0x1F;      
//       cout << "<EOB> geo "<<dec<<geo<<" evn "<<evn<<endl; 
//       break;
//     default:
//       cout << "<INVALID> data "<<hex<<data<<endl; 
//       break;
//     }
//   }
// }


// /****************************************/
// void dumpTDC0(unsigned int *buf, int sz)
// /****************************************/
// {
//   int i, tdc, ch, nch, evn;
//   unsigned int data;

//   for (i=0; i<sz; i++) {
//     data = buf[i];
//     switch((data&0x8000)>>15) {
//     case 1:
//       nch=((data&0x7000)>>12)+1;
//       evn=(data&0xFFF);
//       cout << "<HEADER> Number of ch converted: " << nch  <<", ev number: " << evn << endl;
//       break;     
//     case 0:
//       ch=(data&0x7000)>>12;
//       tdc=(data&0xFFF);    
//       cout << "<DATA> TDC0 on Ch: " << ch << " | tdc: " << tdc << endl;
//       break;
//     default:
//       cout << "<INVALID> data "<<hex<<data<<endl; 
//       break;
//     }
//   }
// }

// /****************************************/
// void bufdump(unsigned int *buf, int size)
// /****************************************/
// {
//   for (int i=0; i<size; i++) 
//     cout << dec << i << " Data " << hex << buf[i] << dec << endl;
//   return;
// }

/****************************************/
int main()
/****************************************/
  
{



  // ios::sync_with_stdio();          // To syncronize with stdio library (really needed?)

  // char buf[1000];

  std::cout << std::hex <<  disc.getManCode() << std::endl;
  
  return 0;
}
//         cout << "Resetting..." << endl;
//         dio1.reset();

//         cout << "Initialize Status Register..." << endl;
//         dio1.initStatusRegister();
//         usleep(1000000);
//         cout << "set Channel Status Register..." << endl;
    
//         dio1.setChannelStatusRegister(15, 0x4);
//     	dio1.clearDataRegister();
//         for (i=0; i<100000000; i++) {
// 	  dio1.setOutputBit(15);
// 	  dio1.clearOutputBit(15);
// 	}


//      cout << " ADC Test" << endl;
//      cout << " Id is " << hex << adc2.id() << endl; 

//      cout << " Resetting ADC ... " << endl;
//      adc2.singleShotReset();

//      cout << "Geo Address Before Setting "<< hex << adc2.getGeoAddress() << endl;      
//      adc2.setGeoAddress(0x0);
//      adc2.singleShotReset();
//      cout << "Geo Address After Setting " << hex << adc2.getGeoAddress() << endl;      
    
//      for (i=0; i<32; i++) { adc2.setChannelThreshold(i, 0); }
//      //for (i=0; i<32; i++) { cout << "Threshold of channel " << i << " is " << dec << adc2.getChannelThreshold(i) << endl; }
//      for (i=0; i<32; i++) { adc2.enableChannel(i); }

//      adc2.setCrateNumber(0x0);
//      cout << "Crate Number " << adc2.getCrateNumber() << endl;

//      cout << " Disabling Overflow Suppression " << endl;
//      adc2.disableOverflowSuppression();
    
//      cout << " Disabling Zero Suppression " <<  endl;
//      adc2.disableZeroSuppression(); 
    

//     for (i=0; i<1; i++) {
     //      dio1.setOutputBit(15);
     // dio1.clearOutputBit(15);
     // }
     
//      cout << " Number of Events " << hex  <<adc2.eventCounter() << endl;



//  //      // ************* Memory Test *******************

//      adc2.singleShotReset();
//      usleep(1000000);
//      adc2.clearData();



//      adc2.singleShotReset();    
//      adc2.enableTestMemory();


//        adc2.memTestAddrW(0x4);
//        adc2.memTestWord(0x55555555);
//        adc2.memTestAddrW(0x4);
//        adc2.memTestAddrR(0x4);
//        unsigned int data;
//        adc2.read32phys(0x4, &data); 
//        cout << " ADC Word " << hex << data << endl; 
    

//        adc2.memTestAddrW(0x4);
//        adc2.memTestWord(0xAAAAAAAA);
//        adc2.memTestAddrW(0x4);
//        adc2.memTestAddrR(0x4);
//        adc2.read32phys(0x4, &data); 
//        cout << " ADC Word " << hex << data << endl; 
//        adc2.disableTestMemory();




//      adc2.enableTestMode();   // si accende il led di busy???????????
//      adc2.disableTestMode();

//      adc2.testEventWriteReg(0x1CAF);
//      adc2.enableTestMode();
//      //******************************************

//  	  cout << " Now The TDC Write/Read Test " << endl;


//  	  unsigned int zzz;
//  	  tdc1.reset();
//  	  tdc1.enableTestMode();

//  	  tdc1.memTestWord(0xAAAAAAAA);
//  	  for (i=0; i<1; i++) {
//  	    tdc1.read32phys(0x0, &zzz);
//  	    cout << " TDC Word " << hex  << zzz << endl;
//  	  }

//  	  tdc1.memTestWord(0x55555555);
//  	  for (i=0; i<1; i++) {
//  	    tdc1.read32phys(0x0, &zzz);
//  	    cout << " TDC Word " << hex  << zzz << endl;
//  	  }

//  	  tdc1.disableTestMode();


//  	  //	  int sz;


//  	//  	  sz = adc2.readEvent(buf);
//  	//  	  dumpADC(buf, sz);

//	int i;

// 	//  dio1.initStatusRegister();
// 		 dio1.reset();
// 	  dio1.setChannelStatusRegister(10, 0x4);
// 	  //	for (i=0; i<1000000; i++) {dio1.set1(10);dio1.set0(10);}
// 	//	  	dio1.clearDataRegister();	  
// 	  dio1.set1(10);
// 	  dio1.set0(10);

// 	  //   	  dio1.singlePulse(10);

// 	  	  dio1.setChannelStatusRegister(10, 0x3);
// 	  //		  	  dio1.set0(10);

// //  	  dio1.set0(10);

//  	  usleep(5000000);
//  	  dio1.set0(10);




//  return 0;
//}














