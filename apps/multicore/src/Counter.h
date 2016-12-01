//          Copyright Boston University SESA Group 2013 - 2016.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef EBBRT_ZKCOUNTER_H_
#define EBBRT_ZKCOUNTER_H_

#include "MulticoreEbb.h"

namespace ebbrt {
class Counter;

class CounterRoot : public ebbrt::MulticoreEbbRootz<CounterRoot, Counter> {
public:
  CounterRoot();
  uint64_t Get();
private:
  friend class ebbrt::MulticoreEbbz<Counter,CounterRoot>;
  Counter* get_rep_(size_t core){ 
    ebbrt::kprintf("overload! %d\n", id_);
    return ebbrt::MulticoreEbbRootz<CounterRoot, Counter>::get_rep_(core);
  };
};

class Counter : public ebbrt::MulticoreEbbz<Counter, CounterRoot> {
public:
  Counter();
  void Up();
  void Down();
  uint64_t GetLocal();
  uint64_t Get();

private:
  uint64_t count_ = 0;
};

}; // end namespace
#endif // EBBRT_ZKGLOBALIDMAP_H_
