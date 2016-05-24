
// poll.h
struct pollfd {
  int fd;        /* file descriptor */
  short events;  /* requested events */
  short revents; /* returned events */
};

#define POLLOUT 0
#define POLLIN 0

#define srandom(x)
#define random() 0 
#define poll(x,y,z) 0

#include <lwip/def.h>
