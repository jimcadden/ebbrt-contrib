//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "Counter.h"
#include <ebbrt/Cpu.h>
#include <ebbrt/Debug.h>

void ebbrt::Counter::Up() { ++count_; };
void ebbrt::Counter::Down() { --count_; };
uint64_t ebbrt::Counter::GetLocal() { return count_; };
uint64_t ebbrt::Counter::Get() { return root_->Get(); };

uint64_t ebbrt::CounterRoot::Get() {
  auto sum = init_;
  for (size_t core = 0; core < ebbrt::Cpu::Count(); ++core) {
    auto it = reps_.find(core);
    if (it != reps_.end()) {
      sum += it->second->GetLocal();
    } else {
      ebbrt::kprintf("core skipped!\n");
    }
  }
  return sum;
};
