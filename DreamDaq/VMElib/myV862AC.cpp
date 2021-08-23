#include "myV862AC.h"
#include "myModules.h"

v862ac::v862ac(uint32_t base, const char* dev) : v792ac(base, dev)
 {
  m_id = base | ID_V862AC;
  m_name = "CAEN V862AC Charge ADC";
 }

