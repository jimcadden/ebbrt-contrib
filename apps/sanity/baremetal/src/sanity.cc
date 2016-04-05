//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define RUNS 100
#define ITERATIONS 1000

#include <ebbrt/Acpi.h>
#include <ebbrt/EventManager.h>
#include "Printer.h"
#include "Perf.h"

void AppMain() { 
  printer->Print("SANITY BACKEND UP.\n"); 

  ebbrt::perf::PerfCounter c{ebbrt::perf::PerfEvent::fixed_cycles};

  c.Start();
  for( int i = 1; i<ITERATIONS; i++){
    asm volatile(""); 
    ebbrt::kprintf(".");
  }
  printer->Print("GATHERING NUMBERS.\n"); 
  c.Stop();

  auto count = c.Read();
  ebbrt::kprintf("\nRead %llu cycles\n", count);
  //c.clear();
  //auto ofl = c.overflow();

  printer->Print("SANITY BACKEND FINISHED.\n"); 
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::kprintf("Powering off...\n"); });
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::acpi::PowerOff(); });
 }

