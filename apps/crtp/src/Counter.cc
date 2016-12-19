// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include "Counter.h"

uint64_t LocalCounterRoot::Get() {
  auto sum = 0;
  for (auto i : local_reps_) {
    sum += i.second->GetLocal();
  }
  return sum;
};

uint64_t Counter::Get() { return root_->Get(); }
