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
  ebbrt::perf::PerfCounter i{ebbrt::perf::PerfEvent::fixed_reference_cycles};

  for( int i = 1; i<ITERATIONS; i++){
    asm volatile(""); 
  }
  auto count = c.Read();
  auto count2 = i.Read();
  ebbrt::kprintf("Counters: c:%llu  i:%llu\n", count, count2);

  c.Start();
  for( int i = 1; i<ITERATIONS; i++){
    asm volatile(""); 
  }
  c.Stop();
  i.Start();
  for( int i = 1; i<ITERATIONS; i++){
    asm volatile(""); 
  }
  i.Stop();

  count = c.Read();
  count2 = i.Read();
  ebbrt::kprintf("Counters: c:%llu  i:%llu\n", count, count2);

  /// this should not be counted
  for( int i = 1; i<ITERATIONS*100; i++){
    asm volatile(""); 
  }
  //// 
  
  c.Start();
  i.Start();
  for( int i = 1; i<ITERATIONS; i++){
    asm volatile(""); 
  }
  i.Stop();
  for( int i = 1; i<ITERATIONS; i++){
    asm volatile(""); 
  }
  c.Stop();

  count = c.Read();
  count2 = i.Read();
  ebbrt::kprintf("Counters: c:%llu  i:%llu\n", count, count2);

  c.Clear();
  i.Clear();
  count = c.Read();
  count2 = i.Read();
  ebbrt::kprintf("Counters: c:%llu  i:%llu\n", count, count2);
  //c.clear();
  //auto ofl = c.overflow();

  printer->Print("SANITY BACKEND FINISHED.\n"); 
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::kprintf("Powering off...\n"); });
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::acpi::PowerOff(); });
 }

