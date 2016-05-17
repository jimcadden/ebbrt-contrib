#ifndef ZK_KLUDGE_H_
#define ZK_KLUDGE_H_

#include <stdlib.h>
#include <errno.h>

#define UNIMPLEMENTED()                                                  \
  do {                                                                         \
    abort();                            \
  } while (0)

#define srandom(X) 

typedef unsigned int socklen_t;

//void srandom(int s){ return; }

#define EAI_NONAME 2
#define EAI_MEMORY 3

#define AF_UNSPEC 0
#define SOCK_STREAM 0
#define IPPROTO_TCP 0

#define POLLOUT 0


#endif
