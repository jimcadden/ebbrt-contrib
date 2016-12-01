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

    ebbrt::EbbRef<ebbrt::Counter> counter(ebbrt::ebb_allocator->Allocate());
    auto custom_counter = ebbrt::Counter::Create(7);
    auto barrier = new ebbrt::SpinBarrier(ebbrt::Cpu::Count());
    for (size_t core = 1; core < ebbrt::Cpu::Count(); ++core) {
      ebbrt::event_manager->SpawnRemote(
          [counter, &barrier, custom_counter]() {
            counter->Up();
            custom_counter->Up();
            barrier->Wait();
          },
          core);
    }
    counter->Up();
    custom_counter->Up();
    barrier->Wait();
    if((size_t)ebbrt::Cpu::GetMine() == 0){
        char buff[100];
        sprintf(buff, "Sums \n\tcounter:%d \n\tcustom:%d\n", (int)counter->Get(),(int)custom_counter->Get());
        printer->Print(buff);
    }
}
