#include "mySockServ.h"
#include "dreamtcpserv.h"

dreamtcpserv::dreamtcpserv() : isthere(0)
 {
  ts = new tcpserv;
 }

dreamtcpserv::~dreamtcpserv()
 {
  delete ts;
 }

void dreamtcpserv::open(uint16_t port)
 {
  ts->tcp_open(port);
 }
bool dreamtcpserv::accept(const char * host)
 {
  if (isthere) return false;
  isthere = ts->tcp_accept(host);
  return isthere;
 }
void dreamtcpserv::disc()
 {
  if (isthere) ts->tcp_disconnect();
  isthere = false;
 }
void dreamtcpserv::sendcommand(const void * cmd, size_t len)
 {
  if (len > 255) return;
  uint8_t sz = len;
  size_t cnt = ts->tcp_send(&sz,1);
  if (cnt != 1) printf("error in sending size %d\n",cnt);
  cnt = ts->tcp_send(cmd,sz);
  if (cnt != sz)
   {
    char errmess[100];
    sprintf(errmess, "Mismatch in sent byte counts %d != %d", cnt, sz);
    throw std::string(errmess);
   }
 }
int dreamtcpserv::getcommand(void * cmd, size_t len)
 {
  accept();
  uint8_t sz;
  int nr = ts->tcp_recv(&sz, 1);
  if (nr != 1) fprintf(stdout, "error for size %d\n", nr);
  nr = ts->tcp_recv(cmd, sz);
  if (nr != sz) fprintf(stdout, "error for data %d\n", nr);
  if (nr == 0) disc();
  return nr;
 }
