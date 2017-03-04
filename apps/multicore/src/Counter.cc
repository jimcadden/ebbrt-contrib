//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "Counter.h"

uint64_t GlobalCounter::Get() {
  return 100;
}

uint64_t NodeCounter::Get() {
  auto sum = root_->Get();
  for (auto i : local_reps_) {
    sum += i.second->GetLocal();
  }
  return sum;
};

uint64_t Counter::Get() { return root_->Get(); }
