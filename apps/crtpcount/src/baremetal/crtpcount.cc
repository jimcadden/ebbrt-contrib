//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ebbrt/Cpu.h>
#include <ebbrt/Debug.h>
#include <ebbrt/SpinBarrier.h>
#include "Printer.h"
#include "../Counter.h"
#include "../StaticEbbIds.h"

void AppMain() { printer->Print("CRTPCOUNT BACKEND UP.\n"); 

   auto counter = ebbrt::EbbRef<Counter>(kCounterEbbId);
   auto barrier = new ebbrt::SpinBarrier(ebbrt::Cpu::Count());
  counter->Up();

  for (size_t core = 1; core < ebbrt::Cpu::Count(); ++core) {
    ebbrt::event_manager->SpawnRemote(
        [barrier, counter] {
          counter->Up();
          barrier->Wait();
        },
        core);
  }
  barrier->Wait();
  ebbrt::event_manager->SpawnLocal(
      [&]() { ebbrt::kprintf("Global sum: %llu\n", counter->Get()); });
}
