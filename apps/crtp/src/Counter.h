//          Copyright Boston University SESA Group 2013 - 2016.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef EBBRT_CRTP_COUNTER_H_
#define EBBRT_CRTP_COUNTER_H_

#include "CRTP.h"

class Counter;
class GlobalCounterRoot;

class GlobalCounterRoot : public EbbShard<GlobalCounterRoot, LocalCounterRoot>{
  using EbbShard<GlobalCounterRoot, LocalCounterRoot>::EbbShard;
public:
  uint64_t Get();
}

class LocalCounterRoot : public EbbShard<LocalCounterRoot, Counter, GlobalCounterRoot> {
  using EbbShard<LocalCounterRoot, Counter, GlobalCounterRoot>::EbbShard;
public:
  uint64_t Get();
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
