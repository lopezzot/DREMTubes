#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <cerrno>
#include <iostream>

#include "myModules.h"
#include "myV814.h"

using namespace std;

/*****************************************/
// Constructor
/*****************************************/
v814::v814(uint32_t base, const char* dev):vme(base, 0xFF, dev)
 {
  m_id = base | ID_V814;
  m_name = "CAEN V814 Discriminator";
  uint16_t ver = getVersion();
  uint16_t cod = getManCode();
  uint16_t fix = getFixed();
  cout << m_name << " id 0x" << hex << m_id << " ver 0x" << ver 
            << " cod 0x" << cod << " fix 0x" << fix << dec << endl;
  if ((cod != 0x853) || (fix != 0xfaf5))
   {
    cerr << "Wrong address or module type" << endl;
    exit(1);
   }
  reset();
 }

