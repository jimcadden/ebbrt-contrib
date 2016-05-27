//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
//
#include <ebbrt/Timer.h>
#include "ZooKeeper.h"

ebbrt::ZooKeeper::ZooKeeper(const std::string &server_hosts, void* global_watcher, int timeout_ms) {

  // create zk object
  zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
  zoo_deterministic_conn_order(1); // enable deterministic order
  zk_ = zookeeper_init(server_hosts.c_str(),
                               (void*)global_watcher,
                               timeout_ms,
                               nullptr, // client id
                               this,
                               0);
  timer->Start(*this, std::chrono::milliseconds(timeout_ms/10), true);
  return;
}

ebbrt::ZooKeeper::~ZooKeeper() {
  if (zk_) {
    auto ret = zookeeper_close(zk_);
    if (ret != ZOK) {
    }
  }
}

void ebbrt::ZooKeeper::Fire() {
  std::lock_guard<SpinLock> guard(lock_);
  struct timeval tv;
  int fd;
  int interest;
  int maxfd = 1;

  zookeeper_interest(zk_, &fd, &interest, &tv);

//  struct pollfd fds[2];
//  struct adaptor_threads *adaptor_threads = zh->adaptor_priv;
 //if (fd != -1) {
 //  fds[1].fd = fd;
 //  fds[1].events = (interest & ZOOKEEPER_READ) ? POLLIN : 0;
 //  fds[1].events |= (interest & ZOOKEEPER_WRITE) ? POLLOUT : 0;
 //  maxfd = 2;
 //}
 //timeout = tv.tv_sec * 1000 + (tv.tv_usec / 1000);

 //// poll(fds, maxfd, timeout);
 // if (fd != -1) {
 //   interest = (fds[1].revents & POLLIN) ? ZOOKEEPER_READ : 0;
 //   interest |= ((fds[1].revents & POLLOUT) || (fds[1].revents & POLLHUP))
 //                   ? ZOOKEEPER_WRITE
 //                   : 0;
 // }
 // if (fds[0].revents & POLLIN) {
 //   // flush the pipe
 //   char b[128];
 //   while (read(adaptor_threads->self_pipe[0], b, sizeof(b)) == sizeof(b)) {
 //   }
 // }
  // dispatch zookeeper events
//  rc = zookeeper_process(zh, interest);
  // check the current state of the zhandle and terminate
  // if it is_unrecoverable()
 // if (is_unrecoverable(zh))
  //  break;
}

bool ebbrt::ZooKeeper::is_connected() {
  return zoo_state(zk_) == ZOO_CONNECTED_STATE;
}

bool ebbrt::ZooKeeper::is_expired() {
  return zoo_state(zk_) == ZOO_EXPIRED_SESSION_STATE;
}
