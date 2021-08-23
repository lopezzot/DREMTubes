/*
 * socklib - simple TCP socket interface
 * Together with exmaple_client and example_server,
 * got this from:
 * http://www.hawklord.uklinux.net/system/socket/ts2.htm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

#define S_LIBRARY
#include "socklib.h"

/******************************************************************************
 * SOCKET *sopen(void)
 * 
 * The sopen() function is called to create a socket. The socket
 * descriptor, along with some internal administrative information, is
 * stored in a SOCKET structure (defined in <socklib.h>). A pointer to
 * the SOCKET structure is returned by sopen() if the call is successful
 * (a zero is returned on error). The pointer should be saved, to pass
 * into other socket library functions. The SOCKET structure is a defined
 * type, as follows:
 *              typedef struct {
 *                        struct sockaddrin sin; 
 *                        int sinlen;
 *                        int bindflag;
 *                        int sd;
 *                } SOCKET;
 *
 *  Where sd is the socket descriptor returned when the socket is
 *  created, sin and sinlen are the address and length information of
 *  the server socket, and bindflag is used in the server to ensure
 *  that the bind() system call is only used once on the server's
 *  socket. 
 *****************************************************************************/
SOCKET *sopen(void) {
  int optval=1;
  socklen_t optlen=4;
  struct linger fix_ling;
  

  SOCKET *sp;
  
  if ((sp = (SOCKET *)malloc(sizeof(SOCKET)))==0) {
    return 0;
  }
  
  if ((sp->sd = socket(AF_INET, SOCK_STREAM /*& SO_KEEPALIVE */, 0))==-1) {
    perror("sopen(socket)");
    free(sp); 
    return 0;
  }

  /*  if (setsockopt(sp->sd, SOL_SOCKET, SO_KEEPALIVE, (void *)&optval, optlen)==-1) {
    perror("sopen(setsockopt)");
    } */

  /* don't leave the socket in a TIME_WAIT state if we close the connection */
  fix_ling.l_onoff = 1;
  fix_ling.l_linger = 0; 
  if (setsockopt(sp->sd, SOL_SOCKET, SO_LINGER, &fix_ling, sizeof(fix_ling))==-1) {
    perror("sopen(setsockopt)");
  }
  if (setsockopt(sp->sd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, optlen)==-1) {
    perror("sopen(setsockopt)");
  }
  
  
  sp->sinlen = sizeof(sp->sin); 
  sp->bindflag = S_RESET;
  
  return sp;
}



/******************************************
 * sclose(SOCKET *sp)
 ******************************************/
int sclose(SOCKET *sp) {
  int sd;
  
  sd = sp->sd; 
  free(sp);
  
  return close(sd);
}



/****************************************************************************
 * sserver(SOCKET *sp, int port, int sync)
 *
 *  The sserver() function sets up the calling process as a network
 * server, and establishes client connections. The sp parameter is a
 * SOCKET pointer previously returned by sopen(). The port parameter is
 * the port number which should be bound with the local host IP address
 * to the open socket, and the sync parameter specifies whether or not
 * calls to sserver() will block if no client is waiting to establish a
 * connection. The two possible values for sync are S_DELAY and S_NDELAY,
 * which are defined in <socklib.h>. A call to sserver() will return < 0
 * if an error occurs or if S_NDELAY is specified and no client
 * connections are waiting to be accepted. In the latter case the
 * externally declared integer ermo will be set to the value
 * EWOULDBLOCK. If a connection with a client is established then the
 * return value from sserver() will be a socket descriptor which can be
 * used with read(), write(), etc., to communicate with the new client.
 ****************************************************************************/
int sserver(SOCKET *sp, int port, int sync) {
  int flags; 
  struct hostent *hostent; 
  char localhost[S_NAMLEN+1];

  if (sp->bindflag==S_RESET) {
    
    if (gethostname(localhost, S_NAMLEN)==-1
	|| (hostent = gethostbyname(localhost))==0) {
      perror("sserver(gethostbyname)");
      return -1;
    }
    
    sp->sinlen = sizeof(sp->sin); 
    //    sp->sin.sin_len = 0;
    memset(&sp->sin, 0, sizeof(sp->sin));
    sp->sin.sin_family = (short)hostent->h_addrtype; 
    sp->sin.sin_port = htons((unsigned short)port); 
    //    sp->sin.sin_addr.s_addr = *(unsigned long *)hostent->h_addr;

    if (bind(sp->sd, (struct sockaddr *)&sp->sin, sizeof(sp->sin))==-1) {
      perror("sserver(bind)");
      return -2;
    }
    if (listen(sp->sd, 4)==-1) {
      perror("sserver(listen)");
      return -3;
    }
    
    sp->bindflag = S_SET;
    
    if ((flags = fcntl(sp->sd, F_GETFL))==-1) {
      perror("fcntl(F_GETFL)");
      return -4;
    }

    switch (sync) {
      
	case S_DELAY:
	  if (fcntl(sp->sd, F_SETFL, flags&~O_NDELAY)==-1) 
	    return -4;
	  
	  break;
	  
	case S_NDELAY:
	  if (fcntl(sp->sd, F_SETFL, flags|O_NDELAY)==-1) 
	    return -4;
	  
	  break;
	  
	default:
	  return -4;
    }                   
  }
  
  return accept(sp->sd, (struct sockaddr *)&sp->sin, &sp->sinlen);


  /* below is an alternative code using select

  fd_set in;
  int new_sock;
  
  FD_ZERO(&in);
  FD_SET(sp->sd, &in);
  if (select(sp->sd + 1, &in, NULL, NULL, NULL) < 0) {
    perror("sserver(select)");
    return -5;
  }

  if (FD_ISSET(sp->sd, &in)) {
    new_sock = accept(sp->sd, NULL, NULL);
    if (new_sock < 0) {
      perror("sserver(accept)");
    }
    return new_sock;
  }

  return -1; */

}



/**************************************************************************
 * sclient(SOCKET *sp, char *name, int port) 
 * 
 * The sclient() function tries to connect to a specified server on a
 * given machine. Its three parameters are sp, which is a socket pointer
 * returned from sopen(), name, which is the machine name of the server,
 * and port, which is the server's port number on the machine. The
 * function waits until a connection with the server is established and
 * then returns a socket descriptor connected to the server, or -1 on
 * error.
 ***************************************************************************/
int sclient(SOCKET *sp, char *name, int port) {
  struct hostent *hostent;
  
  if ((hostent = gethostbyname(name))==0) {
    return -1;
  }
  
  sp->sinlen = sizeof(sp->sin); 
  //  sp->sin.sin_len = 0;
  memset(&sp->sin, 0, sizeof(sp->sin));
  sp->sin.sin_family = (short)hostent->h_addrtype; 
  sp->sin.sin_port = htons((unsigned short)port); 
  sp->sin.sin_addr.s_addr = *(unsigned long *)hostent->h_addr;
  
  if (connect(sp->sd, (struct sockaddr *)&sp->sin, sp->sinlen)==-1) {
    //  perror("sclient(connect)");
    return -1;
  }
  return sp->sd;
}



/***************************************************************************
*
* sreadline - read a line from socket 
*
* Read a line from a descriptor.  Read the line one byte at a time,
* looking for the newline.  We store the newline in the buffer,
* then follow it with a null (the same as fgets(3)).
* We return the number of characters up to, but not including,
* the null (the same as strlen(3)).
*
* RETURNS
* number of bytes read
****************************************************************************/

int sreadline(
	int fd,			/* the sockets file descriptor */
	char *ptr,		/* pointer where data are put in */
	int maxlen		/* maximum number of bytes to read */
	) {
  int n, rc;
  char c;
  
  for (n = 1; n < maxlen; n++) {
    if ( (rc = recv(fd, &c, 1, 0 )) == 1) {
      /* read exactly one char, like it should do */
      *ptr++ = c;
      if (c == '\n')
       break;
    } else if (rc == 0) {
      if (n == 1)
	return(0);	/* EOF, no data read */
      else
	break;		/* EOF, some data was read */
    } else {	/* rc == -1 ?? */
      int errno_tmp;
      errno_tmp = errno;
#ifdef DEBUG
      perror("readline(recv())");
#endif
      if (errno_tmp ==  EINTR) {
	continue;
      } else {
	return(-1);	/* error */
      }
    }
  }
 
  *ptr = '\0';
  return(n);
}



/***************************************************************************
*
* sreadbuffer - read buffer from socket 
*
* Read buffer from a descriptor.  Read buffer
* with a specified length at a time.
* We return the number of characters read.
*
* RETURNS
* number of bytes read
*****************************************************************************/

int sreadbuffer(
	int fd,			/* the sockets file descriptor */
	char *ptr,		/* pointer where data are put in */
	int maxlen		/* maximum number of bytes to read */
	) {
  int rc;

  if ( (rc = read(fd, ptr, maxlen) ) == maxlen) {    
    /* read the max number of characters */
  } else if (rc == 0) {
    return(0);	/* EOF, no data read */
  } else if (rc > 0) {
    /* some data read */
  } else {	/* rc == -1 ?? */
    int errno_tmp;
    errno_tmp = errno;
    perror("readbuffer(read())");
    if(errno_tmp ==  EINTR) {
      printf("EINTR\n");
      return(-1);
      /*continue;*/
    } else {
      return(-1);	/* error */
    }
  }
  
  return(rc);
}

