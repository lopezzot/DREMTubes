#include <iostream>

#include "myV262.h"
#include "myModules.h"

using namespace std;

void v262::reset()
 {
  NIMOutMask=0;
  ECLOutMask=0;
  write16phys(V262_ECL,0);
  write16phys(V262_NIM_PULSE,0);
  write16phys(V262_NIM_LEVEL,0);
 }

/*****************************************/
// Constructor
/*****************************************/

v262::v262(uint32_t base, const char* dev):vme(base, 0xFF, dev)
 {
  m_id = base | ID_V262;
  m_name = "CAEN V262 I/O Register";
  uint16_t ver = getVersion();
  uint16_t cod = getManCode();
  uint16_t fix = getFixed();
  cout << m_name << " id 0x" << hex << m_id << " ver 0x" << ver
       << " cod 0x" << cod << " fix 0x" << fix << dec << endl;
  if ((cod != 0x801) || (fix != 0xfaf5))
   {
    cerr << "Wrong address or module type\n";
    exit(1);
   }
  reset();
 }

