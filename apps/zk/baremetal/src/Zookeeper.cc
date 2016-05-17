//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include "Zookeeper.h"

void ebbrt::Zookeeper::TcpSession::Receive(std::unique_ptr<MutIOBuf> b) {
  return;
}

ebbrt::Zookeeper::Zookeeper(const std::string &server_hosts, void *global_watcher, int timeout_ms) {

  // create zk object
  zk_ = zookeeper_init(server_hosts.c_str(),
                               nullptr,
                               timeout_ms,
                               nullptr, // client id
                               this,
                               0);

  // setup TCP/IP session
  ebbrt::NetworkManager::TcpPcb pcb;
  pcb.Connect(ebbrt::Ipv4Address({172, 17, 0, 4}), 2181);
  tcp_session_ = new ebbrt::Zookeeper::TcpSession(this,std::move(pcb));
  tcp_session_->Install();
  
  

  return;
}
