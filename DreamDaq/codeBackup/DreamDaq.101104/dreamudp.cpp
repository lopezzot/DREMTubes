#include "mySockUdp.h"
#include "dreamudp.h"

dreamudp::dreamudp(){
  ts = new udpsock();
}

dreamudp::~dreamudp()
 {
   delete ts;
 }

void dreamudp::open(const char * host, uint16_t port)
 {
  ts->udp_open(host,port);
 }

void dreamudp::close()
 {
   ts->udp_close();
 }

void dreamudp::sendcommand(const void * cmd, size_t len)
 {
  if (len > 255) return;
  uint8_t sz = len;
  size_t cnt = ts->udp_send(&sz,1);
  if (cnt != 1) printf("error in sending size %d\n",cnt);
  usleep(100);
  cnt = ts->udp_send(cmd,sz);
  if (cnt != sz)
   {
    char errmess[100];
    sprintf(errmess, "Mismatch in sent byte counts %d != %d", cnt, sz);
    throw std::string(errmess);
   }
 }

int dreamudp::getcommand(void * cmd, size_t len)
{
  uint8_t sz;
  int nr = ts->udp_recv(&sz, 1);
  if (nr != 1) fprintf(stdout, "error for size %d\n", nr);
  nr = ts->udp_recv(cmd, sz);
  if (nr != sz) fprintf(stdout, "error for data %d %d\n", nr, int(sz));
  return nr;
}

int dreamudp::getcommand(void * cmd, size_t len, uint32_t timeout)
 {
   return ts->udp_recv(cmd, len, timeout);
 }

void dreamudp::connect(){

  int res;
  char buffer[16];
  do{
    res = this->getcommand(buffer,16,500);
    this->sendcommand("ping",5);
  }while(res <= 0 || strcmp(buffer,"ping") != 0);
  
  this->sendcommand("pong",5);
  
  do{
    res = this->getcommand(buffer,16,500);
  }while(res <= 0 || strcmp(buffer,"pong") != 0);

}
