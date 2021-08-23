#include <assert.h>
#include <sched.h>
#include <iostream>

#include "myV513.h"
#include "myModules.h"

using namespace std;

/*****************************************/
// Generic Commands
/*****************************************/

unsigned short v513::xreadInputRegister()
{ 
  unsigned short rc; 
  read16phys(0x04, &rc);
  volatile unsigned short rcvol = rc;  
  return rcvol;
}

int v513::setChannelStatusRegister(int channel, unsigned short val)
{

  // if ((channel < 0)||(channel > 15)) {
  //   cout << "setChannelStatusRegister: cannot set - channel " << channel << " does not exist" << endl;
  //   return -1;
  // }

  assert ((channel & ~0xf) == 0);

  int addr = 0x10 + 2*channel;

  write16phys(addr, val);

  return 0;
}

unsigned short v513::getChannelStatusRegister(int channel)
{
  
  // if ((channel < 0)||(channel > 15)) {
  //   cout << "getChannelStatusRegister: error - channel " << channel << " does not exist" << endl;
  //   return 0xFFFF;
  // }

  assert ((channel & ~0xf) == 0);

  int addr = 0x10 + 2*channel;
  unsigned short val;

  read16phys(addr, &val);
  return val&0xf;
}

void v513::setOutputBit(int bit) 
{

  // if ((bit < 0)||(bit > 15)) {
  //   cout << "setOutputBit: cannot write at bit " << bit << " - only bits from 0 to 15 are allowed" << endl;
  //   return;
  // }

  assert ((bit & ~0xf) == 0);

  unsigned short val;

  read16phys(0x04, &val);
  write16phys(0x04, val|(0x1<<bit));
}

void v513::clearOutputBit(int bit) 
{
  // if ((bit < 0)||(bit > 15)) {
  //   cout << "clearOutputBit: cannot write at bit " << bit << " - only bits from 0 to 15 are allowed" << endl;
  //   return;
  // }

  assert ((bit & ~0xf) == 0);

  unsigned short val;
  
  read16phys(0x04, &val);
  write16phys(0x04, val&(~(0x1<<bit)));
}

void v513::pulseOutputBit(int bit) 
{
  // if ((bit < 0)||(bit > 15)) {
  //   cout << "pulseOutputBit: cannot write at bit " << bit << " - only bits from 0 to 15 are allowed" << endl;
  //   return;
  // }

  assert ((bit & ~0xf) == 0);
  
  unsigned short val;
  
  read16phys(0x04, &val);
  write16phys(0x04, val&(~(0x1<<bit)));
  sched_yield();

  write16phys(0x04, val|(0x1<<bit));
  sched_yield();
}

/*****************************************/
// Generic Functions
/*****************************************/

// Please set in the V513 init setChannelStatusRegister(bit, 0x6) before to use this function;
void v513::set0(int bit) { setOutputBit(bit); }

// Please set in the V513 init setChannelStatusRegister(bit, 0x4) before to use this function;
void v513::set1(int bit) { clearOutputBit(bit); }

// Please set in the V513 init setChannelStatusRegister(bit, 0x4) before to use this function;
void v513::singlePulse(int bit) { pulseOutputBit(bit); }

/*****************************************/
// Constructor
/*****************************************/
v513::v513(unsigned int base, const char *dev):vme(base, 0x1000, dev)
 {
  m_id = base | ID_V513;
  m_name = "CAEN V513 I/O Register";
  m_status = 0;
 }

