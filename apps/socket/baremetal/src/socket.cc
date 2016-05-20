//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstring>
#include <sys/socket.h>

#include <ebbrt/Debug.h>

void AppMain() {

  int sockfd = 0, n = 0;
  char recvBuff[1024];
  struct sockaddr_in serv_addr;
  char ip[15] = "10.3.124.1"; 

  memset(recvBuff, '0', sizeof(recvBuff));

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    ebbrt::kprintf("\n Error : Could not create socket \n");
    return;
  }
  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = 5000;
  
  int stop=1;
  while(stop)
    ;

  if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
    printf("\n inet_pton error occured\n");
    return;
  }
  

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    ebbrt::kprintf("\n Error : Connect Failed \n");
    return;
  }

  int stop2=1;
  while(stop2)
    ;

  while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
    recvBuff[n] = '\0';
    ebbrt::kprintf(" Read: %s \n", recvBuff);
  }
  if (n < 0) {
    ebbrt::kprintf("\n Read error \n");
    return;
  }
#if 0
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
#endif
}
