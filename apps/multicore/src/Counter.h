//          Copyright Boston University SESA Group 2013 - 2016.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef EBBRT_ZKCOUNTER_H_
#define EBBRT_ZKCOUNTER_H_

#include "../../crtp/src/CRTP.h"
#include <ebbrt/LocalIdMap.h>

namespace ebbrt {

class Counter;
class NodeCounter;
class GlobalCounter;

class GlobalCounter : public EbbShard<GlobalCounter, NodeCounter>{
  using EbbShard::EbbShard;
public:
  uint64_t Get();
};

class NodeCounter : public EbbShard<NodeCounter, Counter, GlobalCounter> {
  using EbbShard::EbbShard;
public:
  uint64_t Get();
};

class Counter : public Ebb<Counter, NodeCounter> {
  using Ebb::Ebb;
public:
  void Up() { count_++; };
  void Down() { count_--; };
  uint64_t GetLocal() { return count_; };
  uint64_t Get();

private:
  uint64_t count_ = 0;
};

};     // end namespace
#endif // EBBRT_ZKGLOBALIDMAP_H_
