
#ifdef _SENZA_HEADERS_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>

#include <iostream>
using namespace std;

/*****************************************/
// The class
/*****************************************/
class vme {

 public:

  vme(unsigned int base, unsigned int size, const char *dev);     // Constructor
  ~vme();                                                         // Destructor
  void print();                                                   // Simple dump function
  int id();                                                       // Returns base address

  // Read/Write at module (local) physical address (base address subtracted)
  int read8phys(unsigned int addr, unsigned char *data);
  int read16phys(unsigned int addr, unsigned short *data);
  int read32phys(unsigned int addr, unsigned int *data);
  int write8phys(unsigned int addr, unsigned char data);
  int write16phys(unsigned int addr, unsigned short data);
  int write32phys(unsigned int addr, unsigned int data);
  unsigned int getBaseAddr(){return ba;};
  int getLength(){return length;};
  void * getBuffer(){return vbuf;};


 protected:

  unsigned int ba;          // Base Address
  int fd;                   // File Descriptor for I/O port
  void *vbuf;               // Pointer to mmap space for this module
  int length;               // Size of the memory mapping
  int irqlev;               // VME Interrupt Level
  int irqvec;               // VME Interrupt Vector

 private:
};

/*****************************************/
// VME Read / Write Functions
/*****************************************/
inline int vme::read8phys(unsigned int addr, unsigned char *data) { *data = static_cast<unsigned char *>(vbuf)[addr]; return 0; }
inline int vme::write8phys(unsigned int addr, unsigned char data) { static_cast<unsigned short *>(vbuf)[addr] = data; return 0; }

inline int vme::read16phys(unsigned int addr, unsigned short *data) { *data = static_cast<unsigned short *>(vbuf)[addr/2]; return 0; }
inline int vme::write16phys(unsigned int addr, unsigned short data) { static_cast<unsigned short *>(vbuf)[addr/2] = data; return 0; }

inline int vme::read32phys(unsigned int addr, unsigned int *data) { *data = static_cast<unsigned int *>(vbuf)[addr/4]; return 0; }
inline int vme::write32phys(unsigned int addr, unsigned int data) { static_cast<unsigned int *>(vbuf)[addr/4] = data; return 0; }

/*****************************************/
// Constructor
/*****************************************/
vme::vme(unsigned int base, unsigned int size, const char *dev)
{
  ba = base;
  length = size;
  irqlev = 0;
  irqvec = 0;

  errno = 0;
  fd = open(dev, O_RDWR);
  if (errno) {
    cout << "myVme: Cannot open device " << dev << endl;
    exit(-1);
  }
 
  lseek(fd, 0, SEEK_SET);         // Set offset of file descriptor to zero
  if (errno) {
    cout << "myVme: Cannot lseek file descriptor " << fd << endl;
    exit(-1) ;
  }

  vbuf = mmap(0, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base);
  if (errno) {
    cout << "myVme: Cannot mmap at address " << hex << ba << dec << endl;
    if(errno==ENOMEM)
      cout<< "ENOMEM" << endl;
    if(errno==EINVAL)
      cout<< "EINVAL" << endl;
    if(errno==EAGAIN)
      cout<< "EAGAIN" << endl;
    exit(-1) ;
  }
  
  //  cout << "myVME: object created for VME module at base address " << ba << " (device " << dev << ")" << endl;
  return;
}

/*****************************************/
// Destructor
/*****************************************/
vme::~vme()
{
  munmap(vbuf, length);
  if (errno) {
    // cout << "myVme: Cannot unmmap at address " << ba << endl;
    // exit(-1);
  }
  close(fd);
  //  cout << "myVME: object destroyed for VME module at base address " << ba << endl;
  return;
}

/*****************************************/
// Dump Function
/*****************************************/
void vme::print() 
{
  cout << "*************************************" << endl;  
  cout << "Generic VME Module / base " << ba << ", mmap " << &vbuf << ", length " << length << endl;
  cout << "*************************************" << endl;
  return;
}

/*****************************************/
// ID function
/*****************************************/
inline int vme::id() { return ba; }

#endif

#include "myModules.h"
#include "myVme.h"
#include "myL1176.h"

#define TTNUMBER_VERSION (0x4)
#define TTNUMBER_COUNTER (0xC)
#define TTNUMBER_NUMBER (0x8)
  
int main( int argc, char * * argv)
{
 uint32_t data;
 vme IOpisa(0x100000,0x100,"/dev/vmedrv32d32");
 l1176 tdc0(0x300000,"/dev/vmedrvb24d32");

 IOpisa.read32phys(0x4, &data);
 std::cout << "VERSION (0x4) 0x" << std::hex << data << std::dec << " " << data << std::endl;
 IOpisa.read32phys(0x8, &data);
 std::cout << "NUMBER  (0x8) 0x" << std::hex << data << std::dec << " " << data << std::endl;
 IOpisa.read32phys(0xC, &data);
 std::cout << "COUNTER (0xC) 0x" << std::hex << data << std::dec << " " << data << std::endl;

 tdc0.setCommStart();
 tdc0.setCommStartTimeout(L1176_COMMSTART_TIMEOUT_1);
 tdc0.enableRisingEdge();
 tdc0.disableFallingEdge();
 tdc0.setTriggerPulse();
 tdc0.enableAcq();
 tdc0.clear();
 int ugo;
 std::cin >> ugo;
 unsigned int enm = tdc0.evtNum();
 std::cout << " evt num 1 " << enm << std::endl;
 unsigned int buffer[1024]; 
 unsigned int size=tdc0.readSingleEvent(buffer);
 std::cout << " size " << size << std::hex << std::endl;
 for (unsigned int i=0; i<size; i++) std::cout << " " << buffer[i];
 std::cout << std::dec << std::endl;
 tdc0.clear();
do {
 enm = tdc0.evtNum();
 std::cout << " evt num 2 " << enm << std::endl;
 size=tdc0.readSingleEvent(buffer);
 std::cout << " size " << size << std::hex << std::endl;
 for (unsigned int i=0; i<size; i++) std::cout << " " << buffer[i];
 std::cout << std::dec << std::endl;
 getchar();
 tdc0.clear();
} while(1);
 return 0;
}
