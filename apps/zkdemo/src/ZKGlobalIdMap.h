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
#include <ebbrt/StaticSharedEbb.h>

#include <ebbrt-zookeeper/ZooKeeper.h>

#include "StaticEbbIds.h"

namespace ebbrt {

class ZKGlobalIdMap : public StaticSharedEbb<ZKGlobalIdMap>,
                      public CacheAligned {

public:
  typedef ebbrt::ZooKeeper::PathWatcher MapWatcher;
  typedef ebbrt::ZooKeeper::PathWatchEvent WatchEvent;

  struct RepeatAlert : MapWatcher {
    void OnChanged(const char* path){
      ebbrt::kprintf("(watched) VALUE CHANGED: %s\n", path);
    };
    bool Repeat(){ 
      
    }
  };
  Future<bool> Init();
  Future<std::vector<std::string>> List(EbbId id, std::string path = std::string());
  Future<std::string> Get(EbbId id,
                   std::string path = std::string());
  Future<void> Set(EbbId id, std::string data,
                   std::string path = std::string());
  void SetWatcher(EbbId id, MapWatcher* w,  std::string path = std::string());
  // Spawn
  void SetWatchEvent(EbbId id, WatchEvent* f, std::string path = std::string());

private:
  struct ConnectionWatcher : ebbrt::ZooKeeper::ConnectionWatcher {
    void OnConnected() override {
      ebbrt::kprintf("ZKGlobalIdMap: Session Connected.\n");
      connected_.SetValue(true);
    }
    void OnConnecting() override {
      ebbrt::kprintf("ZKGlobalIdMap: Session Conneting...\n");
    }
    void OnSessionExpired() override {
      ebbrt::kabort("ZKGlobalIdMap: Session Expired.\n");
    }
    void OnAuthFailed() override {
      ebbrt::kprintf("ZKGlobalIdMap: Session Authentication Failed.\n");
      connected_.SetValue(false);
    }
    ebbrt::Promise<bool> connected_;
  };

  ConnectionWatcher zkwatcher_;
  ebbrt::EbbRef<ebbrt::ZooKeeper> zk_;
};

constexpr auto zkglobal_id_map = EbbRef<ZKGlobalIdMap>(kZKGlobalIdMapId);
} // namespace ebbrt

#endif // EBBRT_ZKGLOBALIDMAP_H_
