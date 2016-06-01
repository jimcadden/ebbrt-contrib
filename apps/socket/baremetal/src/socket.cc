//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ebbrt/Debug.h>

#define SOCK_CLI 0
#define SOCK_SRV 0
#define SOCK_ALL 0
#define ZK_CPP 1

/*******************************************************************/
/*******************************************************************/
#if ZK_CPP

#include "Printer.h"
#include "ZooKeeper.h"


class PrinterWatcher : public ebbrt::ZooKeeper::Watcher {
  public:
    void OnConnected() override { printer->Print("watch alert: Session Connected.\n"); }
    void OnConnecting() override { printer->Print("watch alert: Session Conneting.\n"); }
    void OnSessionExpired() override { printer->Print("watch alert: Session Expired.\n"); }
    void OnCreated(const char* path) override { printer->Print("watch alert: Created!\n"); }
    void OnDeleted(const char* path) override { printer->Print("watch alert: Deleted !\n"); }
    void OnChanged(const char* path) override { printer->Print("watch alert: Changed: !\n"); }
    void OnChildChanged(const char* path) override { printer->Print("watch alert: Child Changed.\n"); }
    void OnNotWatching(const char* path) override { printer->Print("watch alert: Not Wathcing.\n"); }
};

auto *mw = new PrinterWatcher();
ebbrt::ZooKeeper *zk = new ebbrt::ZooKeeper("172.17.0.4:2181", mw);

void AppMain() { }


#endif
/*******************************************************************/
/*******************************************************************/
#if SOCK_SRV 

#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <string>


void AppMain() {
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    std::string sendBuff ("Hello World!\n");
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    //if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    //{
    //  ebbrt::kabort("\n inet_pton error occured\n");
    //    return;
    //} 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
        ebbrt::kprintf("Listening for a connection....\n");
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        //ticks = time(NULL);
        //snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));

        write(connfd, sendBuff.c_str(), sendBuff.size()); 

        close(connfd);
        //sleep(1);
     }
}
#endif // SOCK_SRV
/*******************************************************************/
/*******************************************************************/

#if SOCK_CLI

#include <cstring>
#include <sys/socket.h>
#include <errno.h>

void AppMain() {

  int sockfd = 0, n = 0;
  char recvBuff[1024];
  struct sockaddr_in serv_addr;
  char ip[15] = "0.0.0.0"; 
  
  ebbrt::kprintf("The time is: \n");
  memset(recvBuff, '0', sizeof(recvBuff));

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    ebbrt::kprintf("\n Error : Could not create socket (%d)\n", errno);
    return;
  }
  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5000); 
  
  if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
    ebbrt::kprintf("\n inet_on error occured (%d)\n");

    return;
  }
  
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    ebbrt::kprintf("Error : Connect Failed \n");
    return;
  }

  while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
    recvBuff[n] = '\0';
    ebbrt::kprintf("> %s \n", recvBuff);
  }
  close(sockfd);
}
#endif // SOCK_CLI


/*******************************************************************/
/*******************************************************************/
#if SOCK_ALL
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
}
#endif // SOCK_ALL

