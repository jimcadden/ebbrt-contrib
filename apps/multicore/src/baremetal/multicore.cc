//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ebbrt/Cpu.h>
#include <ebbrt/Debug.h>
#include <ebbrt/SpinBarrier.h>
#include <ebbrt/EbbRef.h>
#include "Printer.h"
#include "../Counter.h"

void AppMain() { printer->Print("MULTICORE BACKEND UP.\n"); 

    auto cid = ebbrt::ebb_allocator->Allocate();
    auto counter = ebbrt::EbbRef<ebbrt::Counter>(cid);
    auto barrier = new ebbrt::SpinBarrier(ebbrt::Cpu::Count());
    for (size_t core = 1; core < ebbrt::Cpu::Count(); ++core) {
      ebbrt::event_manager->SpawnRemote(
          [counter, &barrier, core]() {
            counter->Up();
            barrier->Wait();
          },
          core);
    }
    counter->Up();
    barrier->Wait();
    if((size_t)ebbrt::Cpu::GetMine() == 0){
        ebbrt::kprintf("Sum: %d\n", counter->Get());
    }
}
