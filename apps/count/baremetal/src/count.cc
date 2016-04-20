//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "Printer.h"
#include <boost/container/flat_map.hpp>
#include <ebbrt/EventManager.h>
#include <ebbrt/LocalIdMap.h>
#include <ebbrt/MulticoreEbb.h>
#include <ebbrt/SpinBarrier.h>

class Counter
    : public ebbrt::MulticoreEbb<Counter, ebbrt::MulticoreEbbRoot<Counter>> {
public:
  void Up() { ++count_; };
  void Down() { --count_; };
  uint64_t GetLocal() { return count_; };
  uint64_t Get() {
    auto sum = 0ull;
    auto root = this->GetRoot();
    auto refs = root->GetReps();
    for (size_t core = 0; core < ebbrt::Cpu::Count(); ++core) {
      auto it = refs->find(core);
      if (it != refs->end()) {
        sum += it->second->GetLocal();
      }
    }
    return sum;
  };

private:
  uint64_t count_ = 0;
};

namespace {
ebbrt::MulticoreEbbRoot<Counter> counter_root;
ebbrt::EbbRef<Counter> counter = Counter::Create(&counter_root);
auto barrier = new ebbrt::SpinBarrier(ebbrt::Cpu::Count());
}

void AppMain() {
  printer->Print("COUNT BACKEND UP.\n");
  for (size_t core = 1; core < ebbrt::Cpu::Count(); ++core) {
    ebbrt::event_manager->SpawnRemote(
        [core]() {
          counter->Up();
          barrier->Wait();
        },
        core);
  }
  counter->Up();
  barrier->Wait();
  ebbrt::event_manager->SpawnLocal(
      [=]() { ebbrt::kprintf("Sum: %llu\n", counter->Get()); });
}
