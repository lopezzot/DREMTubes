
extern "C"{
  int mySIS3320Decode(unsigned int *buffer, unsigned int datasize);
  int myL1182Decode(unsigned int *buffer, unsigned int datasize,
		    unsigned int number);
  int myV260Decode(unsigned int *buffer, unsigned int datasize);
  int myL1176Decode(unsigned int *buffer, unsigned int datasize);
  int myKLOETDCDecode(unsigned int *buffer, unsigned int datasize);
  int mySubEvent(unsigned int *buffer, int frag);
  int myEvent(unsigned int * buffer,int aVerb);
}
