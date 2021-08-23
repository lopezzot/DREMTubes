/*****************************************/
// The v862ac class 
/*****************************************/

#ifndef _MY_V862AC_H_
#define _MY_V862AC_H_

#include <iostream>

#include "myV792AC.h"

class v862ac : public v792ac {

 public:

  v862ac(unsigned int base, const char *dev) : v792ac(base, dev)
	{ m_id = base | ID_V862AC; m_name = "CAEN Charge ADC V862AC"; }

 protected:

 private:

};

/*****************************************/

#endif // _MY_V862AC_H_
