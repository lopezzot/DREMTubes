//$Id: myTH03.h,v 1.4 2007/06/01 08:19:28 cvsdream Exp $

struct th03_data{
  bool stop;
  bool running;
  bool init;
  unsigned int refs;
  unsigned int ch12;
  unsigned int ch3;
};

void * th03_handler(void *);

