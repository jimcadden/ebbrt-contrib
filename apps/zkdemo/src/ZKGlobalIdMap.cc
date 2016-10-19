//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <cstdio>
#include <string>

#include <ebbrt/Debug.h>
#include <ebbrt/GlobalIdMap.h>

#include <ebbrt-zookeeper/ZooKeeper.h>

class GlobalIdMapWatcher : public ebbrt::ZooKeeper::Watcher {
public:
  void OnConnected() override {
    ebbrt::kprintf("watch alert: Session Connected.\n");
  }
  void OnConnecting() override {
    ebbrt::kprintf("watch alert: Session Conneting.\n");
  }
  void OnSessionExpired() override {
    ebbrt::kprintf("watch alert: Session Expired.\n");
  }
  void OnCreated(const char *path) override {
    ebbrt::kprintf("watch alert: Created!\n");
  }
  void OnDeleted(const char *path) override {
    ebbrt::kprintf("watch alert: Deleted !\n");
  }
  void OnChanged(const char *path) override {
    ebbrt::kprintf("watch alert: Changed: !\n");
  }
  void OnChildChanged(const char *path) override {
    ebbrt::kprintf("watch alert: Child Changed.\n");
  }
  void OnNotWatching(const char *path) override {
    ebbrt::kprintf("watch alert: Not Wathcing.\n");
  }
};

auto *zkwatcher = new GlobalIdMapWatcher();
ebbrt::EbbRef<ebbrt::ZooKeeper> zk;

ebbrt::GlobalIdMap::GlobalIdMap()
#ifdef __ebbrt__
    : Messagable<GlobalIdMap>(kGlobalIdMapId), val_(0) {
#else
    : Messagable<GlobalIdMap>(kGlobalIdMapId) {
#endif
  ebbrt::kprintf("ZKGlobalIdMap consturctor called\n");
  zk = ebbrt::ZooKeeper::Create(
      ebbrt::ebb_allocator->Allocate(),
      static_cast<ebbrt::ZooKeeper::Watcher *>(zkwatcher));
}

#ifdef __ebbrt__
__attribute__((weak)) void ebbrt::GlobalIdMap::SetAddress(uint32_t addr) {
  ebbrt::kprintf("ZKGlobalIdMap SetAddress() called: %u\n", addr);
}
#endif

ebbrt::Future<std::string> ebbrt::GlobalIdMap::Get(ebbrt::EbbId id) {
  ebbrt::kprintf("ZKGlobalIdMap Get() called: %d \n", id);
  auto p = new ebbrt::Promise<std::string>;
  auto ret = p->GetFuture();
  char buff[100];
  sprintf(buff, "/%d", id);
  zk->Get(std::string(buff)).Then(
          [p](ebbrt::Future<ebbrt::ZooKeeper::Znode> f) { 
            auto zn = f.Get(); 
            p->SetValue(zn.value); 
          });
  return ret;
}

ebbrt::Future<void> ebbrt::GlobalIdMap::Set(EbbId id, std::string val) {
  ebbrt::kprintf("ZKGlobalIdMap Set() called: %d %s\n", id, val.c_str());
  // if not chached, if not existed, create
  // new
  auto p = new ebbrt::Promise<void>;
  auto ret = p->GetFuture();
  char buff[100];
  sprintf(buff, "/%d", id);
  zk->New(std::string(buff), val).Then(
          [p](ebbrt::Future<ebbrt::ZooKeeper::Znode> f) { 
            auto zn = f.Get(); 
            p->SetValue(); 
          });
  return ret;
}

void ebbrt::GlobalIdMap::ReceiveMessage(Messenger::NetworkId nid,
                                        std::unique_ptr<IOBuf> &&b) {
  ebbrt::kprintf("ZKGlobalIdMap ReceiveMessage() called\n");
  EBBRT_UNIMPLEMENTED();
}
