
#include <string>

//282*2*4
//#define OSCDATASIZEB 2256
#define ADDITIONALPTS 32

struct oscheader{
  unsigned int chn;
  unsigned int chmask;
  unsigned int pts;
  unsigned int samrate;
  unsigned int scale[4];
  unsigned int position[4];
  unsigned int timediff[4];
};


extern "C"{
  std::string filename(int run, int spill, int channel, bool local);
  void swapbytes(void * abuf, unsigned int size);
  unsigned int chdatasizebyte(unsigned int pts, unsigned int nch);
}
