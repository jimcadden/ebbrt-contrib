//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <cstdio>
#include <string>

#include "ZKGlobalIdMap.h"

ebbrt::Future<bool> ebbrt::ZKGlobalIdMap::Init() {
  zk_ = ebbrt::ZooKeeper::Create(ebbrt::ebb_allocator->Allocate(), &zkwatcher_);
  return zkwatcher_.connected_.GetFuture();
  ;
}

ebbrt::Future<std::string> ebbrt::ZKGlobalIdMap::Get(ebbrt::EbbId id,
                                                     std::string path) {
  char buf[15];
  sprintf(buf, "/%d", id);
  return zk_->GetValue(std::string(buf));
}

ebbrt::Future<void> ebbrt::ZKGlobalIdMap::Set(EbbId id, std::string val,
                                              std::string path) {
  auto p = new ebbrt::Promise<void>;
  auto ret = p->GetFuture();
  char buff[100];
  sprintf(buff, "/%d", id);
  zk_->Exists(std::string(buff)).Then([this, p, buff, val](auto b) {
    if (b.Get() == true) {
      zk_->Set(std::string(buff), val).Then([p](auto f) { p->SetValue(); });
    } else {
      zk_->New(std::string(buff), val).Then([p](auto f) { p->SetValue(); });
    }
  });
  return ret;
}

//ebbrt::Future<std::vector<std::string>> ebbrt::ZKGlobalIdMap::List(ebbrt::EbbId id, std::string path = std::string()){
//  return;
//}

void ebbrt::ZKGlobalIdMap::SetWatcher(EbbId id, MapWatcher* w, std::string path) {
  char buf[15];
  sprintf(buf, "/%d", id);
  zk_->Stat(std::string(buf), w).Block(); 
  return;
}

void ebbrt::ZKGlobalIdMap::SetWatchEvent(EbbId id, WatchEvent* f, std::string path) {
  char buf[15];
  sprintf(buf, "/%d", id);
  zk_->Stat(std::string(buf), f).Block(); 
  return;
}
