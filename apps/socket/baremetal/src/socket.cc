//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "Printer.h"
#include <sys/socket.h>

void AppMain() { 
  
  int fd;
  const struct sockaddr *saddr = nullptr;
  socklen_t namelen = {};

  fd = socket(AF_INET, SOCK_STREAM, 0);
  fd = recv(fd, nullptr, 0, 0);      

  
  // ALL
  // socket.h
  fd = accept(0, nullptr, nullptr);      
  fd = bind(0, nullptr, namelen);      
  fd = connect(0, saddr, namelen);      
  fd = closesocket(fd); // WIN32
  fd = getpeername(0, nullptr, nullptr);      
  fd = getsockname(0, nullptr, nullptr);      
  fd = getsockopt(0, SOL_SOCKET, SO_ERROR, nullptr, nullptr);
  fd = ioctlsocket(0,0,nullptr); // WIN32
  fd = listen(0,0);      
  fd = recv(0, nullptr, 0, 0);      
  fd = recvfrom(0, nullptr, 0, 0, nullptr, nullptr);      
  fd = send(0, nullptr, 0, 0);      
  fd = sendto(0, nullptr, 0, 0, nullptr, namelen);      
  fd = setsockopt(0, SOL_SOCKET, SO_ERROR, saddr, namelen);
  fd = shutdown(0,0);
  fd = socket(0,0,0);

  // fcntl.h
  fd = read(0,nullptr,0);
  fd = write(0,nullptr,0);
  fd = close(0);
  fd = fcntl(0,0,0);

  // newlib
   //fd = fstat();

  printer->Print("SOCKET BACKEND UP.\n"); 

}
