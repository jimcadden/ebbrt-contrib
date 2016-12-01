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
    ebbrt::EbbRef<ebbrt::GCounter> g_count(ebbrt::GCounter::GlobalCounterId);
    auto init_count = ebbrt::Counter::Create(20);
    auto barrier = new ebbrt::SpinBarrier(ebbrt::Cpu::Count());
    for (size_t core = 1; core < ebbrt::Cpu::Count(); ++core) {
      ebbrt::event_manager->SpawnRemote(
          [counter, &barrier, init_count, g_count]() {
            counter->Up();
            init_count->Up();
            g_count->Up();
            barrier->Wait();
            },
          core);
    }
    counter->Up();
    init_count->Up();
    g_count->Up();
    barrier->Wait();
    if((size_t)ebbrt::Cpu::GetMine() == 0){
        char buff[100];
        sprintf(buff, "Sums:\tcounter(%d)\tinit(%d)\tglobal(%d)\n", (int)counter->Get(),(int)init_count->Get(),(int)g_count->Get());
        printer->Print(buff);
    }
}
