#ifndef _DREAM_TCP_SERV_H
#define _DREAM_TCP_SERV_H

#include <stdint.h>
#include <string.h>

class tcpserv;

class dreamtcpserv
 {
  public:
    dreamtcpserv();
    ~dreamtcpserv();
    void open(uint16_t port);
    bool accept(const char * host = 0);
    void disc();
    void sendcommand(const void * cmd, size_t len);
    int getcommand(void * cmd, size_t len);
  private: 
    tcpserv * ts;
    bool isthere;
 };

#endif // _DREAM_TCP_SERV_H
