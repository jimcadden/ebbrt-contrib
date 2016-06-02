//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "SocketManager.h"
#include "Vfs.h"

#include <ebbrt/Clock.h>
#include <ebbrt/Debug.h>
#include <ebbrt/NetMisc.h>
#include <ebbrt/Runtime.h>

#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <poll.h>

#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/memp.h"
#include "lwip/pbuf.h"

int lwip_listen(int s, int backlog) {
  // XXX: backlog len ignored
  try {
    auto fd = ebbrt::root_vfs->Lookup(s);
    return static_cast<ebbrt::EbbRef<ebbrt::SocketManager::SocketFd>>(fd)->Listen();
  } catch (std::invalid_argument &e) {
    errno = EBADF;
    return -1;
  }
}

int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen) {
  try {
    auto fd = ebbrt::root_vfs->Lookup(s);
    return static_cast<ebbrt::EbbRef<ebbrt::SocketManager::SocketFd>>(fd)
        ->Accept()
        .Block()
        .Get();
  } catch (std::invalid_argument &e) {
    errno = EBADF;
    return -1;
  }
}

int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen) {
  try {
    auto fd = ebbrt::root_vfs->Lookup(s);
    auto saddr = reinterpret_cast<const struct sockaddr_in *>(name);
    auto port = ntohs(saddr->sin_port); // port arrives in network order
    return static_cast<ebbrt::EbbRef<ebbrt::SocketManager::SocketFd>>(fd)->Bind(
        port);
  } catch (std::invalid_argument &e) {
    errno = EBADF;
    return -1;
  }
}

// LWIP_ERROR("lwip_connect: invalid address", ((namelen == sizeof(struct
// sockaddr_in)) &&
//((name->sa_family) == AF_INET) && ((((mem_ptr_t)name) % 4) == 0)),
// sock_set_errno(sock, err_to_errno(ERR_ARG)); return -1;);
// name_in = (const struct sockaddr_in *)(void*)name;
//
//
int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen) {
  // TODO: verify socket domain/type is valid
  auto saddr = reinterpret_cast<const struct sockaddr_in *>(name);
  auto ip_addr = saddr->sin_addr.s_addr; // ip arrives in network order
  auto port = ntohs(saddr->sin_port);    // port arrives in network order
  ebbrt::NetworkManager::TcpPcb pcb;
  pcb.Connect(ebbrt::Ipv4Address(ip_addr), port);
  auto fd = ebbrt::root_vfs->Lookup(s);
  // TODO: verify fd is right for connecting
  auto connection =
      static_cast<ebbrt::EbbRef<ebbrt::SocketManager::SocketFd>>(fd)
          ->Connect(std::move(pcb))
          .Block();
  auto is_connected = connection.Get();
  if (is_connected) {
    return 0;
  }
  return -1;
}

int lwip_socket(int domain, int type, int protocol) {
  if (domain != AF_INET || type != SOCK_STREAM || protocol != 0) {
    ebbrt::kabort("Socket type not supported");
    return -1;
  }
  return ebbrt::socket_manager->NewIpv4Socket();
}

// A read with len=0 will create a future that signals when a non-blocking socket has data
int lwip_read(int s, void *mem, size_t len) {
  auto fd = ebbrt::root_vfs->Lookup(s);
  auto fdref = static_cast<ebbrt::EbbRef<ebbrt::SocketManager::SocketFd>>(fd);

  if (len && !fdref->IsReadReady()){
    errno = EAGAIN;
    return -1;
  }
  auto fbuf = fdref->Read(len).Block();
  auto buf = std::move(fbuf.Get());
  auto chain_len = buf->ComputeChainDataLength();
  assert(chain_len <= len);
  if (chain_len > 0) {
    auto dptr = buf->GetDataPointer();
    std::memcpy(mem, dptr.Data(), chain_len);
    return chain_len;
  }
  {
    errno = EIO; 
    return -1;
  }
}

int lwip_write(int s, const void *dataptr, size_t size) {
  auto fd = ebbrt::root_vfs->Lookup(s);
  auto buf = ebbrt::MakeUniqueIOBuf(size);
  std::memcpy(reinterpret_cast<void *>(buf->MutData()), dataptr, size);
  fd->Write(std::move(buf));
  return size;
}

int lwip_send(int s, const void *dataptr, size_t size, int flags) {
  return lwip_write(s, dataptr, size);
}

int lwip_recv(int s, void *mem, size_t len, int flags) {
  return lwip_read(s, mem, len);
}

int lwip_close(int s) {
  auto fd = ebbrt::root_vfs->Lookup(s);
  fd->Close().Block();
  return 0;
}

void lwip_assert(const char *fmt, ...) {
  EBBRT_UNIMPLEMENTED();
  return;
}

int lwip_shutdown(int s, int how) {
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_getpeername(int s, struct sockaddr *name, socklen_t *namelen) {
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_getsockname(int s, struct sockaddr *name, socklen_t *namelen) {
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_getsockopt(int s, int level, int optname, void *optval,
                    socklen_t *optlen) {
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_setsockopt(int s, int level, int optname, const void *optval,
                    socklen_t optlen) {

  switch (level) {
  case IPPROTO_TCP:
    switch (optname) {
    case TCP_NODELAY:
    case TCP_KEEPALIVE:
      //    case TCP_KEEPIDLE:
      //    case TCP_KEEPINTVL:
      //    case TCP_KEEPCNT:
      break;
    default:
      EBBRT_UNIMPLEMENTED();
    }
    break;
  default:
    EBBRT_UNIMPLEMENTED();
  }
  return ERR_OK;
}

int lwip_recvfrom(int s, void *mem, size_t len, int flags,
                  struct sockaddr *from, socklen_t *fromlen) {
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_sendto(int s, const void *dataptr, size_t size, int flags,
                const struct sockaddr *to, socklen_t tolen) {
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset,
                fd_set *exceptset, struct timeval *timeout) {
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_ioctl(int s, long cmd, void *argp) {
  EBBRT_UNIMPLEMENTED();
  return 0;
}

int lwip_fcntl(int s, int cmd, int val) {
  auto fd = ebbrt::root_vfs->Lookup(s);
  int f, newflag;
  switch (cmd) {
  case F_GETFL:
    f = (int)fd->GetFlags();
    return f;
    break;
  case F_SETFL:
    // File access mode (O_RDONLY, O_WRONLY, //O_RDWR) and file creation
    // flags (i.e., O_CREAT, O_EXCL, O_NOCTTY, O_TRUNC) in arg  are  ignored. On
    // Linux this command can change only the O_APPEND, O_ASYNC, O_DIRECT,
    // O_NOATIME, and O_NONBLOCK flags.
    newflag = val & O_NONBLOCK;
    if (newflag) {
      auto f = fd->GetFlags();
      fd->SetFlags(f | newflag);
    } else {
      EBBRT_UNIMPLEMENTED();
    }
    break;
  default:
    EBBRT_UNIMPLEMENTED();
  };
  return 0;
}

err_t netconn_gethostbyname(const char *name, ip_addr_t *addr) {
  if (inet_pton(AF_INET, name, addr) <= 0) {
    // failed to convert ip address
    EBBRT_UNIMPLEMENTED();
    return -1;
  }
  return ERR_OK;
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout) {
  if (nfds > 1) {
    EBBRT_UNIMPLEMENTED();
  }
  for (uint32_t i = 0; i < nfds; ++i) {
    auto pfd = fds[i];
    if (pfd.fd < 0 || pfd.events == 0) {
      EBBRT_UNIMPLEMENTED();
      fds[i].revents = 0;
      break;
    }
    auto fd = static_cast<ebbrt::EbbRef<ebbrt::SocketManager::SocketFd>>(
        ebbrt::root_vfs->Lookup(pfd.fd));

    if ((pfd.events & POLLIN) && fd->IsReadReady()) {
      fds[i].revents |= POLLIN;
    }
    if ((pfd.events & POLLOUT) && fd->IsWriteReady()) {
      fds[i].revents |= POLLOUT;
    }
    if ((~(POLLIN | POLLOUT) & pfd.events) != 0) {
      ebbrt::kabort("Poll event type unsupported: %x", pfd.events);
    }
  }
  return 0;
}


