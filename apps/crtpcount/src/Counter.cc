// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include "Counter.h"
#include <ebbrt/UniqueIOBuf.h>

EBBRT_PUBLISH_TYPE(, GlobalCounterRoot);

// GlobalCounterRoot
//
uint64_t GlobalCounterRoot::GetLocal() {
  kprintf("GlobalCounter GetLocal called\n");
  auto sum = 0;
  for (auto i : local_reps_) {
    sum += i.second->GetLocal();
  }
  return sum;
};

uint64_t GlobalCounterRoot::Get() {
  auto sum = 0;
  auto futs = GetRemotes();
  for( auto &f : futs){
    sum += f.Block().Get();
  }
  return sum;
};
std::vector<ebbrt::Future<uint64_t>> GlobalCounterRoot::GetRemotes() {
  uint32_t id;
  std::vector<ebbrt::Future<uint64_t>> ret;
  {
    std::lock_guard<std::mutex> guard(m_);
    for (auto &nid : node_map_) {
      Promise<uint64_t> promise;
      id = id_; // Get a new id (always even)
      id_ += 2;
      bool inserted;
      auto f = promise.GetFuture();
      ret.push_back(std::move(f));
      std::tie(std::ignore, inserted) =
          request_map_.emplace(id, std::move(promise));
      assert(inserted);
      auto buf = MakeUniqueIOBuf(sizeof(uint32_t));
      auto dp = buf->GetMutDataPointer();
      dp.Get<uint32_t>() = id + 1; // Ping messages are odd
      SendMessage(nid, std::move(buf));
    }
  }
  return ret;
}
#if 0
ebbrt::Future<uint64_t> GlobalCounterRoot::Get() {
  kprintf("GlobalCounterRoot Get called\n");
  uint32_t id;
  Promise<uint64_t> promise;
  auto ret = promise.GetFuture();
  {
    std::lock_guard<std::mutex> guard(m_);
    id = id_; // Get a new id (always even)
    id_ += 2;
    bool inserted;
    // insert our promise into the hash table
    std::tie(std::ignore, inserted) =
        request_map_.emplace(id, std::move(promise));
    assert(inserted);
  }
  // Construct and send the ping message
  auto buf = MakeUniqueIOBuf(sizeof(uint32_t));
  auto dp = buf->GetMutDataPointer();
  dp.Get<uint32_t>() = id + 1; // Ping messages are odd
  for (auto &nid : node_map_) {
    SendMessage(nid, std::move(buf));
  }
  return ret;
}
#endif

void GlobalCounterRoot::Register(Messenger::NetworkId nid) {
  kprintf("GlobalCounterRoot Registed called!\n");
#ifdef __ebbrt__
  // Send an empty ACK to the leader to register the node
  SendMessage(nid, MakeUniqueIOBuf(1, true));
#endif
  node_map_.push_back(nid);
  return;
}

void GlobalCounterRoot::ReceiveMessage(ebbrt::Messenger::NetworkId nid,
                                       std::unique_ptr<ebbrt::IOBuf> &&buffer) {
// registration ACK when len=1
// kprintf("message size: %d\n", buffer->ComputeChainDataLength());
#ifndef __ebbrt__
  if (buffer->ComputeChainDataLength() == 1) {
    Register(nid);
    return;
  }
#endif
  auto dp = buffer->GetDataPointer();
  auto id = dp.Get<uint32_t>();
  // Get requests id + 1, so they are always odd
  if (id & 1) {
  kprintf("Receive Get Request\n");
    // Received Ping
    auto buf = MakeUniqueIOBuf(sizeof(uint32_t) + sizeof(uint64_t));
    auto dp = buf->GetMutDataPointer();
    dp.Get<uint32_t>() = id - 1;     // Send back with the original id
#ifndef __ebbrt__
    dp.Get<uint64_t>() = Get(); // Send back global count
#else
    dp.Get<uint64_t>() = GetLocal(); // Send back local count
#endif
    SendMessage(nid, std::move(buf));
  } else {
  kprintf("Receive Get Reply\n");
    // Received Get response, lookup in the hash table for our promise and set
    auto val = dp.Get<uint64_t>();
    std::lock_guard<std::mutex> guard(m_);
    auto it = request_map_.find(id);
    assert(it != request_map_.end());
    it->second.SetValue(val);
    request_map_.erase(it);
  }
};

// LocalCounterRoot
//
uint64_t LocalCounterRoot::GetLocal() {
  kprintf("LocalCounterRoot GetLocal called\n");
  auto sum = 0;
  for (auto i : local_reps_) {
    sum += i.second->GetLocal();
  }
  return sum;
}
uint64_t LocalCounterRoot::Get()  { return root_->Get(); }

// Counter
//
uint64_t Counter::Get() { return root_->Get(); }
