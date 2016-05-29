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
                               0,
                               0);
  timer->Start(*this, std::chrono::milliseconds(500), true);
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
  struct timeval tv;
  int fd;
  int interest, events;

  zookeeper_interest(zk_, &fd, &interest, &tv);
  events = 0;
  //if (fd != -1) {
  //  if (interest & ZOOKEEPER_WRITE){
  //    events |= ZOOKEEPER_WRITE;
  //  }
  //  else if (interest & ZOOKEEPER_READ) {
  //    // block until there is data to read
  //    // TODO: timeout
  //    ebbrt::kprintf("bloking on read\n");
  //    read(fd,nullptr,0);
  //    events |= ZOOKEEPER_READ;
  //    ebbrt::kprintf("unbkocd on read\n");
  //  }
  //}
  //  dispatch zookeeper events
  zookeeper_process(zk_, interest);
  // check the current state of the zhandle and terminate
  // if it is_unrecoverable()
  if (is_unrecoverable(zk_))
    ebbrt::kabort("Unrecoverable zookeeper error\n");
}

bool ebbrt::ZooKeeper::is_connected() {
  return zoo_state(zk_) == ZOO_CONNECTED_STATE;
}

bool ebbrt::ZooKeeper::is_expired() {
  return zoo_state(zk_) == ZOO_EXPIRED_SESSION_STATE;
}

ebbrt::NodeStat ebbrt::ZooKeeper::Stat(const std::string& path) {
  ebbrt::NodeStat stat;
  auto zoo_code = zoo_exists(zk_, path.c_str(), false, &stat);
  
  ebbrt::kbugon(zoo_code != ZOK);


  return stat;
}

std::string ebbrt::ZooKeeper::Get(const std::string& path, bool watch) {
  std::string value_buffer;

  auto node_stat = Stat(path);

  value_buffer.resize(node_stat.dataLength);

  int buffer_len = value_buffer.size();
  auto zoo_code = zoo_get(zk_,
                          path.c_str(),
                          watch,
                          const_cast<char*>(value_buffer.data()),
                          &buffer_len,
                          &node_stat);

  ebbrt::kbugon(zoo_code != ZOK);

  value_buffer.resize(buffer_len);
  return value_buffer;
}
