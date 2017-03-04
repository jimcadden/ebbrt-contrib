//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../Counter.h"
#include "Printer.h"
#include <ebbrt/Cpu.h>
#include <ebbrt/Debug.h>
#include <ebbrt/EbbRef.h>
#include <ebbrt/SpinBarrier.h>

void AppMain() {
  printer->Print("MULTICORE BACKEND UP.\n");

  ebbrt::EbbRef<ebbrt::Counter> counter(ebbrt::ebb_allocator->Allocate());
  auto barrier = new ebbrt::SpinBarrier(ebbrt::Cpu::Count());
  for (size_t core = 1; core < ebbrt::Cpu::Count(); ++core) {
    ebbrt::event_manager->SpawnRemote(
        //[counter, &barrier, init_count, g_count]() {
        [counter, &barrier]() {
        kprintf("up!\n");
          counter->Up();
          barrier->Wait();
        },
        core);
  }
  counter->Up();
        kprintf("up!\n");
  barrier->Wait();
  if ((size_t)ebbrt::Cpu::GetMine() == 0) {
    char buff[100];
    sprintf(buff, "Sums:\tcounter(%d)\n",
            (int)counter->Get());
    printer->Print(buff);
    counter->Up();
    sprintf(buff, "Sums:\tcounter(%d)\n",
            (int)counter->Get());
    printer->Print(buff);
  }
}
