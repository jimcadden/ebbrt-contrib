// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include "Counter.h"

EBBRT_PUBLISH_TYPE(, GlobalCounterRoot);

uint64_t GlobalCounterRoot::Get() {
  kprintf("GlobalCounterRoot Get!\n");
  return 100;
}
void GlobalCounterRoot::ReceiveMessage(ebbrt::Messenger::NetworkId nid,
                      std::unique_ptr<ebbrt::IOBuf>&& buffer){

  Get();
};

uint64_t LocalCounterRoot::Get() {
  auto sum = 0;
  for (auto i : local_reps_) {
    sum += i.second->GetLocal();
  }
  sum += root_->Get();
  return sum;
};

uint64_t Counter::Get() { return root_->Get(); }
