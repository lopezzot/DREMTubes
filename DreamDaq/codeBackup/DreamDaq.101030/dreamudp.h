#ifndef _DREAM_UDP_H
#define _DREAM_UDP_H

#include <stdint.h>
#include <string.h>

class udpsock;

class dreamudp
 {
  public:
    dreamudp();
    ~dreamudp();
    void open(const char * host, uint16_t port);
    void close();
    void sendcommand(const void * cmd, size_t len);
    int getcommand(void * cmd, size_t len);
    int getcommand(void * cmd, size_t len, uint32_t timeout);
    void connect();
  private: 
    udpsock * ts;
 };

#endif // _DREAM_UDP_H
