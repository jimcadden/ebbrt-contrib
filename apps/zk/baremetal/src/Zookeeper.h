//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef APPS_ZK_BAREMETAL_SRC_ZOOKEEPER_H_
#define APPS_ZK_BAREMETAL_SRC_ZOOKEEPER_H_

#include <zookeeper.h>

#include <ebbrt/Net.h>
#include <ebbrt/NetTcpHandler.h>
#include <ebbrt/RcuTable.h>
#include <ebbrt/SpinLock.h>
#include <ebbrt/StaticSharedEbb.h>
#include <ebbrt/SharedIOBufRef.h>

namespace ebbrt {

static const char* state2String(int state){
  if (state == 0)
    return "CLOSED_STATE";
  if (state == ZOO_CONNECTING_STATE)
    return "CONNECTING_STATE";
  if (state == ZOO_ASSOCIATING_STATE)
    return "ASSOCIATING_STATE";
  if (state == ZOO_CONNECTED_STATE)
    return "CONNECTED_STATE";
  if (state == ZOO_EXPIRED_SESSION_STATE)
    return "EXPIRED_SESSION_STATE";
  if (state == ZOO_AUTH_FAILED_STATE)
    return "AUTH_FAILED_STATE";

  return "INVALID_STATE";
}

static const char* type2String(int state){
  if (state == ZOO_CREATED_EVENT)
    return "CREATED_EVENT";
  if (state == ZOO_DELETED_EVENT)
    return "DELETED_EVENT";
  if (state == ZOO_CHANGED_EVENT)
    return "CHANGED_EVENT";
  if (state == ZOO_CHILD_EVENT)
    return "CHILD_EVENT";
  if (state == ZOO_SESSION_EVENT)
    return "SESSION_EVENT";
  if (state == ZOO_NOTWATCHING_EVENT)
    return "NOTWATCHING_EVENT";

  return "UNKNOWN_EVENT_TYPE";
}

class Zookeeper {
public:
  class TcpSession : public ebbrt::TcpHandler {
  public:
    TcpSession(Zookeeper *zk, ebbrt::NetworkManager::TcpPcb pcb)
      : ebbrt::TcpHandler(std::move(pcb)), zk_(zk) {}
    void Close(){}
    void Abort(){}
    void Receive(std::unique_ptr<MutIOBuf> b);

  private:
    std::unique_ptr<ebbrt::MutIOBuf> buf_;
    ebbrt::NetworkManager::TcpPcb pcb_;
    Zookeeper *zk_;
  };

  Zookeeper(){};
  Zookeeper(const std::string& server_hosts,
            void* global_watcher = nullptr,
            int timeout_ms = 5 * 1000);
  Zookeeper(const Zookeeper&) = delete;
  Zookeeper& operator=(const Zookeeper&) = delete;

private:
  clientid_t myid_;
  std::mutex m_;
  zhandle_t* zk_ = nullptr;
  std::unordered_map<std::string, ebbrt::Promise<std::string>> promise_map_;
  ebbrt::Zookeeper::TcpSession *tcp_session_;
};


}// ebbrt namespace
#endif  // APPS_ZK_BAREMETAL_SRC_ZOOKEEPER_H_
