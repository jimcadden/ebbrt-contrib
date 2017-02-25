//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef EBBRT_ZKGLOBALIDMAP_H_
#define EBBRT_ZKGLOBALIDMAP_H_

#include <string>

#include <ebbrt/CacheAligned.h>
#include <ebbrt/Debug.h>
#include <ebbrt/EbbRef.h>
#include <ebbrt/Future.h>
#include <ebbrt/Message.h>
#include <ebbrt/Runtime.h>
#include <ebbrt/StaticIds.h>
#include <ebbrt/SharedEbb.h>

#include <ebbrt-zookeeper/ZooKeeper.h>

#include "StaticEbbIds.h"

namespace ebbrt {

class GlobalIdMap : public SharedEbb<GlobalIdMap>,
                      public CacheAligned {

public:
  typedef ebbrt::ZooKeeper::Watcher Watcher;
  typedef ebbrt::ZooKeeper::WatchEvent WatchEvent;

  GlobalIdMap();
  void SetAddress(uint32_t addr);
  ///
  Future<bool> Init();
  Future<std::vector<std::string>> List(EbbId id, std::string path = std::string());
  Future<std::string> Get(EbbId id,
                   std::string path = std::string());
  Future<void> Set(EbbId id, std::string data,
                   std::string path = std::string());
  void SetWatcher(EbbId id, Watcher* w, std::string path = std::string());

private:
  struct ConnectionWatcher : ebbrt::ZooKeeper::ConnectionWatcher {
    void OnConnected() override {
      ebbrt::kprintf("GlobalIdMap: Session Connected.\n");
      connected_.SetValue(true);
    }
    void OnConnecting() override {
      ebbrt::kprintf("GlobalIdMap: Session Conneting...\n");
    }
    void OnSessionExpired() override {
      connected_.SetValue(false);
      ebbrt::kabort("GlobalIdMap: Session Expired.\n");
    }
    void OnAuthFailed() override {
      connected_.SetValue(false);
      ebbrt::kabort("GlobalIdMap: Session Authentication Failed.\n");
    }
    ebbrt::Promise<bool> connected_;
  };
  ConnectionWatcher zkwatcher_;
  ebbrt::EbbRef<ebbrt::ZooKeeper> zk_;
};

constexpr auto global_id_map = EbbRef<GlobalIdMap>(kGlobalIdMapId);
} // namespace ebbrt
#endif // EBBRT_ZKGLOBALIDMAP_H_
