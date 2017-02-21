//          Copyright Boston University SESA Group 2013 - 2016.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef EBBRT_CRTP_COUNTER_H_
#define EBBRT_CRTP_COUNTER_H_

#include <ebbrt/Debug.h>
#include <ebbrt/Future.h>
#include <ebbrt/Message.h>
#include "../../crtp/src/CRTP.h"

class Counter;
class LocalCounterRoot;
class GlobalCounterRoot;

class GlobalCounterRoot : public SharedLocalEbbShard<GlobalCounterRoot, LocalCounterRoot>, 
public ebbrt::Messagable<GlobalCounterRoot>
{
  using SharedLocalEbbShard<GlobalCounterRoot, LocalCounterRoot>::SharedLocalEbbShard;
public:
  GlobalCounterRoot(EbbId id) : SharedLocalEbbShard(id), ebbrt::Messagable<GlobalCounterRoot>(id) {
#ifdef __ebbrt__
    Register();
#endif 
  }
  std::vector<ebbrt::Future<uint64_t>> GetRemotes();
  //ebbrt::Future<uint64_t> Get();
  uint64_t Get();
  uint64_t GetLocal();
  void ReceiveMessage(ebbrt::Messenger::NetworkId nid,
                      std::unique_ptr<ebbrt::IOBuf>&& buffer);
#ifdef __ebbrt__
  // EbbRT Native
  void Register(Messenger::NetworkId nid = Messenger::NetworkId(runtime::Frontend()));
#else 
  // EbbRT Hosted
  void Register(Messenger::NetworkId nid);
#endif
 private:
  uint32_t id_{0};
  std::mutex m_;
  std::unordered_map<uint32_t, ebbrt::Promise<uint64_t>> request_map_;
  std::vector<Messenger::NetworkId> node_map_;
};

class LocalCounterRoot : public SharedLocalEbbShard<LocalCounterRoot, Counter, GlobalCounterRoot> {
  using SharedLocalEbbShard<LocalCounterRoot, Counter, GlobalCounterRoot>::SharedLocalEbbShard;
public:
  uint64_t Get();
  uint64_t GetLocal();
};

class Counter : public Ebb<Counter, LocalCounterRoot> {
  using Ebb<Counter, LocalCounterRoot>::Ebb;

public:
  void Up() { count_++; };
  void Down() { count_--; };
  uint64_t GetLocal() { return count_; };
  uint64_t Get(); 

private:
  uint64_t count_ = 0;
};

#endif
