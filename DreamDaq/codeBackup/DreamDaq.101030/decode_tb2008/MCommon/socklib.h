#ifndef SOCKLIB_H_SEEN
#define SOCKLIB_H_SEEN
/*
 * socklib - simple TCP socket interface
 * modified 10/6/2002 to read a whole buffer (readbuffer)
 */

#include <netinet/in.h>

#define S_DELAY 0
#define S_NDELAY 1

#ifdef S_LIBRARY
#define S_RESET 0
#define S_SET 1
#define S_NAMLEN 64
#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0x0
#endif

typedef struct
{
    struct sockaddr_in sin; 
    int sinlen;
    int bindflag; 
    int sd;
} SOCKET;

#ifdef __cplusplus
extern "C" {
#endif

extern SOCKET *sopen(void);
extern int sclose(SOCKET *);
extern int sserver(SOCKET *, int, int);
extern int sclient(SOCKET *, char *, int);
extern int sreadline(int fd, char *ptr, int maxlen);
extern int sreadbuffer(int fd, char *ptr, int maxlen);

#ifdef __cplusplus
}
#endif

#endif
