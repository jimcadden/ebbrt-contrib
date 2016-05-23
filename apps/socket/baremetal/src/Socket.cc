//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#include "SocketManager.h"
#include "Vfs.h"

#include <ebbrt/Clock.h>
#include <ebbrt/Debug.h>
#include <ebbrt/NetMisc.h>
#include <sys/socket.h>
#include <ebbrt/Runtime.h>

int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen){

  // TODO: verify socket domain/type is valid
  auto saddr = reinterpret_cast<const struct sockaddr_in *>(name);
  auto ip_addr = saddr->sin_addr.s_addr;
  ip_addr = ebbrt::runtime::Frontend();
  auto port = ntohs(saddr->sin_port); // port arrives in network order
  ebbrt::NetworkManager::TcpPcb pcb;
  pcb.Connect(ebbrt::Ipv4Address(ntohl(ip_addr)), port);
  auto fd = ebbrt::root_vfs->Lookup(s);

  // TODO: verify fd is right for connecting
  auto connection = static_cast<ebbrt::EbbRef<ebbrt::SocketManager::SocketFd>>(fd)->Connect(std::move(pcb)).Block();
  auto  is_connected = connection.Get();
  if( is_connected ){
    return 0;
  }
  return -1;
}

int lwip_socket(int domain, int type, int protocol){
  if ( domain != AF_INET || type != SOCK_STREAM || protocol != 0 ){
    ebbrt::kabort("Socket type not supported");
    return -1;
  }
  return ebbrt::socket_manager->NewIpv4Socket();
}

int lwip_read(int s, void *mem, size_t len){
  auto fd = ebbrt::root_vfs->Lookup(s);
  auto fbuf = fd->Read(len).Block();
  auto buf = std::move(fbuf.Get());
  if( buf ){
    auto chain_len = buf->ComputeChainDataLength();
    assert(chain_len <= len);
    auto dptr = buf->GetDataPointer();
    std::memcpy(mem, dptr.Data(), chain_len);
    return chain_len;
  }else{
    ebbrt::kabort("error: NULL IOBuf");
    return -1;
  }
}


void lwip_assert(const char* fmt, ...){
  EBBRT_UNIMPLEMENTED();
  return;
}

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

int lwip_listen(int s, int backlog){
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_recv(int s, void *mem, size_t len, int flags){
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

#undef htons
u16_t lwip_htons(u16_t x){ return ebbrt::htons(x); }
#undef ntohs
u16_t lwip_ntohs(u16_t x){ return ebbrt::ntohs(x); }
#undef htonl
u32_t lwip_htonl(u32_t x){ return ebbrt::htonl(x); }
#undef ntohl
u32_t lwip_ntohl(u32_t x){ return ebbrt::ntohl(x); }

