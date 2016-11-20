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

class ZKGlobalIdMap : public StaticSharedEbb<ZKGlobalIdMap>, public CacheAligned {
public:
  ZKGlobalIdMap(){
    auto *zkwatcher = new ConnectionWatcher();
    zk_ = ebbrt::ZooKeeper::Create(ebbrt::ebb_allocator->Allocate(), zkwatcher);
  };
  Future<std::string> Get(EbbId id);
  Future<void> Set(EbbId id, std::string data);

private:
  struct ConnectionWatcher : ebbrt::ZooKeeper::ConnectionWatcher {
    void OnConnected() override {
      ebbrt::kprintf("ZKGlobalIdMap: Session Connected.\n");
    }
    void OnConnecting() override {
      ebbrt::kprintf("ZKGlobalIdMap: Session Conneting.\n");
    }
    void OnSessionExpired() override {
      ebbrt::kabort("ZKGlobalIdMap: Session Expired.\n");
    }
    void OnAuthFailed() override {
      ebbrt::kabort("ZKGlobalIdMap: Session Authentication Failed.\n");
    }
  };

  ebbrt::EbbRef<ebbrt::ZooKeeper> zk_;
  ebbrt::SpinLock lock_;
  uint64_t val_;
};

constexpr auto zkglobal_id_map = EbbRef<ZKGlobalIdMap>(kZKGlobalIdMapId);
} // namespace ebbrt

#endif // EBBRT_ZKGLOBALIDMAP_H_
