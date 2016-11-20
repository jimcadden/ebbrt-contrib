//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <cstdio>
#include <string>

#include "ZKGlobalIdMap.h"


ebbrt::Future<std::string> ebbrt::ZKGlobalIdMap::Get(ebbrt::EbbId id) {
  ebbrt::kprintf("ZKGlobalIdMap Get() called: %d \n", id);
  auto p = new ebbrt::Promise<std::string>;
  auto ret = p->GetFuture();
  char buff[100];
  sprintf(buff, "/%d", id);
  // TODO: use GetVal
  zk_->Get(std::string(buff)).Then(
          [p](ebbrt::Future<ebbrt::ZooKeeper::Znode> f) { 
            auto zn = f.Get(); 
            p->SetValue(zn.value); 
          });
  return ret;
}

ebbrt::Future<void> ebbrt::ZKGlobalIdMap::Set(EbbId id, std::string val) {
  ebbrt::kprintf("ZKGlobalIdMap Set() called: %d %s\n", id, val.c_str());
  // if not chached, if not existed, create
  // new
  auto p = new ebbrt::Promise<void>;
  auto ret = p->GetFuture();
  char buff[100];
  sprintf(buff, "/%d", id);
  zk_->New(std::string(buff), val).Then(
          [p](ebbrt::Future<ebbrt::ZooKeeper::Znode> f) { 
            auto zn = f.Get(); 
            p->SetValue(); 
          });
  return ret;
}
