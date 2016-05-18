//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ebbrt/Debug.h>
#include <sys/socket.h>


int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_shutdown(int s, int how){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_getpeername (int s, struct sockaddr *name, socklen_t *namelen){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_getsockname (int s, struct sockaddr *name, socklen_t *namelen){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_close(int s){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_listen(int s, int backlog){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_recv(int s, void *mem, size_t len, int flags){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_read(int s, void *mem, size_t len){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_send(int s, const void *dataptr, size_t size, int flags){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_socket(int domain, int type, int protocol){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_write(int s, const void *dataptr, size_t size){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_ioctl(int s, long cmd, void *argp){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_fcntl(int s, int cmd, int val){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

