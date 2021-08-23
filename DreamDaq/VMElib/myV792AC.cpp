#include <iostream>

#include "myV792AC.h"
#include "myModules.h"

using namespace std;

/**************************************************************/
//   En/Dis able Channel
/**************************************************************/

void v792ac::enableChannels()
{
  int32_t addr;
  uint16_t j;

  for (int32_t cha=0; cha<32; cha++) {
    addr = 0x1080 + 2*cha;
    read16phys(addr, &j);
    write16phys(addr, 0x00FF&j);
  }
}

void v792ac::enableChannel(int32_t cha)
{
  int32_t addr;
  uint16_t j;

  if ((cha < 0)||(cha > 31)) {
    cout << m_name << " " << hex << ba << dec << " enableChannel * ERROR * channel " << cha << " does not exist" << endl;
    return;
  }
  addr = 0x1080 + 2*cha;
  read16phys(addr, &j);
  write16phys(addr, 0x0FF&j);
}

void v792ac::disableChannel(int32_t cha)
{
  int32_t addr;
  uint16_t j;
  
  if ((cha < 0)||(cha > 31)) {
    cout << m_name << " " << hex << ba << dec << " disableChannel * ERROR * channel " << cha << " does not exist" << endl;
    return;
  }
  addr = 0x1080 + 2*cha;
  read16phys(addr, &j);
  write16phys(addr, 0x100|j);
}

/**************************************************************/
//   Set/Get  Channel Threshold
/**************************************************************/

void v792ac::setChannelThreshold(uint16_t thr)
{
  int32_t addr;
  uint16_t data;
  for (int32_t cha=0; cha<32; cha++) {
    addr = 0x1080 + 2*cha;
    read16phys(addr, &data);
    write16phys(addr, (data&0x100)|(0xFF&thr));
  }
  return;
}

void v792ac::setChannelThreshold(int32_t cha, uint16_t thr)
{
  int32_t addr;
  uint16_t data;
  
  if ((cha < 0)||(cha > 31)) {
    cout << m_name << " " << hex << ba << dec << " setChannelThreshold * ERROR * channel " << cha << " does not exist" << endl;
    return;
  }
  addr = 0x1080 + 2*cha;
  read16phys(addr, &data);
  write16phys(addr, (data&0x100)|(0xFF&thr));
  return;
}

uint16_t v792ac::getChannelThreshold(int32_t cha)
{
  int32_t addr;
  uint16_t thr; 
  
  if ((cha < 0)||(cha > 31)) {
    cout << m_name << " " << hex << ba << dec << " getChannelThreshold * ERROR * channel " << cha << " does not exist" << endl;
    return   0xFF;
  }
  addr = 0x1080 + 2*cha;
  read16phys(addr, &thr);
  return 0xFF&thr;
}
int32_t v792ac::readEvent(uint32_t* evbuff)
{
  uint32_t ccc(0), tout(0);
  while (!this->dready()) { m_usleep(1); tout ++; if (tout > 100) return 0; }
  while (0) {
    uint32_t evcnt = eventCounter();
    if ((ccc % 10000) == 0)
     {
      if ((m_eventcounter != evcnt) && (m_eventcounter != ((evcnt+1)&0xffffff)))
       {
        cout << ccc << "                  *** ERROR IN THE NUMBER OF EVENTS *** v792ac id " << hex
             << m_id << dec << " --> ADC counts: " << evcnt
             << " while DAQ counts: " << m_eventcounter
             << " busy " << this->busy() << " data ready " << this->dready() << endl;
       }
      else
       {
        cout << ccc << " v792ac id " << hex << m_id << dec << " - hw event: " << evcnt
             << " - total triggers now: " << m_trignum_now << " pre: " << m_trignum_pre
             << " - sw event: " << m_eventcounter << " busy " << this->busy()
             << " data ready " << this->dready() << endl;
       }
     }
    ccc ++;
   }

  int32_t sz = read32block(0, (uint8_t*)evbuff, 34);

  m_eventcounter++;

  m_trignum_pre = m_trignum_now;

  return sz/4;
}

/*****************************************/
// Constructor
/*****************************************/
v792ac::v792ac(uint32_t base, const char* dev):vme(base, 0x2000, dev)
 {
  cout << "v792ac id " << hex << base << " dev " << dev << endl;
  m_eventcounter = 0;
  m_trignum_pre = 0;
  m_trignum_now = 0;
  m_id = base | ID_V792AC;
  m_name = "CAEN V792AC Charge ADC";
  uint16_t fwrev,geo,ouimsb,oui,ouilsb,ver,idmsb,id,idlsb,rev,smsb,slsb;
  read16phys(0x1000,&fwrev);
  read16phys(0x1002,&geo);
  read16phys(0x8026,&ouimsb);
  read16phys(0x802a,&oui);
  read16phys(0x802e,&ouilsb);
  read16phys(0x8032,&ver);
  read16phys(0x8036,&idmsb);
  read16phys(0x803a,&id);
  read16phys(0x803e,&idlsb);
  read16phys(0x804e,&rev);
  read16phys(0x8f02,&smsb);
  read16phys(0x8f06,&slsb);
  cout << "v792ac id " << hex << m_id << " fwrev " << fwrev << " geo " << geo
       << " ouimsb oui ouilsb " << ouimsb << " " << oui << " " << ouilsb
       << " ver " << ver << " idmsb id idlsb "  << idmsb << " " << id
       << " " << idlsb << " rev " << rev << " smsb slsb " << smsb
       << " " << slsb << dec << endl;
 }

