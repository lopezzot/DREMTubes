#ifndef _MY_V513_H_
#define _MY_V513_H_

#define CH_IS_OUTPUT 0
#define CH_IS_INPUT 1
#define CH_IS_NEGATIVE 0
#define CH_IS_POSITIVE 2
#define CH_IS_GLITCHED 0
#define CH_IS_NORMAL 4
#define CH_IS_TRANSPARENT 0
#define CH_IS_STROBED 8

#define TRANSPARENT_INPUT (CH_IS_INPUT | CH_IS_POSITIVE | CH_IS_NORMAL | CH_IS_TRANSPARENT)
#define TRANSPARENT_OUTPUT (CH_IS_OUTPUT | CH_IS_POSITIVE | CH_IS_NORMAL | CH_IS_TRANSPARENT)

/*****************************************

  myV513.h
  --------

  Definition of the CAEN V513 IO Register.
  This class is a class derived from the class vme.

*****************************************/

#include "myVme.h"

/*****************************************/
// The v513 class 
/*****************************************/
class v513 : public vme {

 public:

  // Constructor
  v513(unsigned int base, const char *dev);    

  // Generic Commands
  inline void reset() { write16phys(0x42, 0); }               // Module Reset

  inline void initStatusRegister() { write16phys(0x46, 0); }  // Initialize Status Register

  inline void clearDataRegister() { write16phys(0x48, 0); }   // Clear Data Register

  inline void clearOutputMask(unsigned short mask)         // clear "mask" in Output Register
   { unsigned short val; read16phys(0x04, &val); val &= ~mask; write16phys(0x04, val); }

  inline void setOutputMask(unsigned short mask)         // set "mask" in Output Register
   { unsigned short val; read16phys(0x04, &val); val |= mask; write16phys(0x04, val); }

  inline void writeOutputRegister(unsigned short val)         // Write 16 bits to Output Register
   { write16phys(0x04, val); }

  inline unsigned short readInputRegister()         // Read 16 bits from Input Register
   { unsigned short val; read16phys(0x04, &val); return val; }

  inline void setChannelConf(unsigned short chan, unsigned short conf)         // Write 4 bits to Channel Configuration Register
   { write16phys(0x10+chan*2, conf); }

  inline void setChannelInput(unsigned short chan)         // Write 4 bits to Channel Configuration Register
   { write16phys(0x10+chan*2, TRANSPARENT_INPUT); }

  inline void setChannelOutput(unsigned short chan)         // Write 4 bits to Channel Configuration Register
   { write16phys(0x10+chan*2, TRANSPARENT_OUTPUT); }

  int setChannelStatusRegister(int channel, unsigned short val); // Set Channel Statur Register
  unsigned short getChannelStatusRegister(int channel);       // Get Channel Statur Register
  unsigned short xreadInputRegister();                         // Read 16 bit from Input Register

  void setOutputBit(int bit);                                 // Write 1 in bit 'bit' in Output Register
  void clearOutputBit(int bit);                               // Write 0 in bit 'bit' in Output Register
  void pulseOutputBit(int bit);                               // pulse bit 'bit' in Output Register

  //GenericFunction
  
  void set0(int bit);                                         // Write 0 in bit 'bit' in Output Register
  void set1(int bit);                                         // Write 1 in bit 'bit' in Output Register
  void singlePulse(int bit);                                  // Write 1 0 in bit 'bit' in Output Register
  
 protected:
  
 private:
   unsigned short m_status;
};

#endif // _MY_V513_H_
