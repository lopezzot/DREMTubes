//$Id: my1182.wainer.h,v 1.2 2008/07/18 11:11:51 dreamdaq Exp $
/*****************************************

  my1182.h
  ---------

  Definition of the LeCroy QADC 1182 class.
  This class is a class derived from the class vme.

*****************************************/

// Standard C header files
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include "myModules.h"
#include <sys/time.h>
}

// C++ header files
#include <iostream>
using namespace std;

/*****************************************/
// The LeCroy1182 class 
/*****************************************/
class LeCroy1182 : public vme {
  
 public:
  
  // Constructor
  LeCroy1182(unsigned int base, const char *dev);    
  
  
  //Generic Commands
  inline unsigned int id();                          // Module ID 
  // inline void reset();                            // Module Reset

  void  print();



 protected:

 private:

};


/*****************************************/
// Constructor
/*****************************************/
LeCroy1182::LeCroy1182(unsigned int base, const char *dev):vme(base, 0x200, dev) {};


/*****************************************/
// Generic Commands
/*****************************************/

inline unsigned int LeCroy1182::id() { return ba | ID_LECROY1182; }

/*****************************************/
void LeCroy1182::print() 
/*****************************************/
{
  cout << "************************************" << endl;  
  cout << "  LeCroy  ADC  1182    Module  /  base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "************************************" << endl;
  return;
}
