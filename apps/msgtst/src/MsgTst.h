//          Copyright Boston University SESA Group 2013 - 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef APPS_MSGTST_SRC_MSGTST_H_
#define APPS_MSGTST_SRC_MSGTST_H_

#include <unordered_map>
#include <iostream>
#include <iterator>
#include <vector>
#include <random>
#include <algorithm>

#include <ebbrt/EbbAllocator.h>
#include <ebbrt/Future.h>
#include <ebbrt/Message.h>

/* The MsgTst Ebb allows both hosted and native implementations to send "PING"
 * messages via the messenger to another node (machine). The remote node
 * responds back and the initiator fulfills its Promise. */

class MsgTst : public ebbrt::Messagable<MsgTst> {
 public:
  static ebbrt::EbbRef<MsgTst>
  Create(ebbrt::EbbId id = ebbrt::ebb_allocator->Allocate());

  std::unique_ptr<IOBuf> RandomMsg(uint16_t bytes);
  static MsgTst& HandleFault(ebbrt::EbbId id);

  MsgTst(ebbrt::EbbId ebbid);

  ebbrt::Future<void> Ping(ebbrt::Messenger::NetworkId nid);

  void ReceiveMessage(ebbrt::Messenger::NetworkId nid,
                      std::unique_ptr<ebbrt::IOBuf>&& buffer);

 private:
  const char alphanum[] = "0 .. 1A .. Za.. z";

  std::mutex m_;
  std::unordered_map<uint32_t, ebbrt::Promise<void>> promise_map_;
  uint32_t id_{0};
};

#endif  // APPS_MSGTST_SRC_MSGTST_H_
