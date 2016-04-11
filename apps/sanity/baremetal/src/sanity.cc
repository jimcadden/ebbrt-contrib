//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define PRINT_ITERATIONS 100
#define NIL_ITERATIONS 10000

#include <ebbrt/Acpi.h>
#include <ebbrt/EventManager.h>
#include "Printer.h"
#include "Perf.h"

void AppMain() { 
  printer->Print("SANITY BACKEND UP.\n"); 

  ebbrt::perf::PerfCounter c{ebbrt::perf::PerfEvent::llc_misses};
  ebbrt::perf::PerfCounter i{ebbrt::perf::PerfEvent::branch_misses};
  ebbrt::perf::PerfCounter m{ebbrt::perf::PerfEvent::reference_cycles};
  // llc_references
  // reference_cycles

  auto count = c.Read();
  auto count2 = i.Read();
  auto count3 = m.Read();
  ebbrt::kprintf("New: %llu %llu %llu \n", count, count2, count3);

  c.Start();
  i.Start();
  m.Start();
  for( int i = 1; i<NIL_ITERATIONS; i++){
    asm volatile(""); 
  }
  c.Stop();
  i.Stop();
  m.Stop();

  count = c.Read();
  count2 = i.Read();
  count3 = m.Read();
  ebbrt::kprintf("Nil: %llu %llu %llu \n", count, count2, count3);

  /// this should not be counted
  for( int i = 1; i<NIL_ITERATIONS*100; i++){
    asm volatile(""); 
  }
  //// 
  count = c.Read();
  count2 = i.Read();
  count3 = m.Read();
  ebbrt::kprintf("Stopped: %llu %llu %llu \n", count, count2, count3);
  
  c.Start();
  i.Start();
  m.Start();
  for( int i = 1; i<NIL_ITERATIONS; i++){
    asm volatile(""); 
  }
  c.Stop();
  i.Stop();
  m.Stop();

  count = c.Read();
  count2 = i.Read();
  count3 = m.Read();
  ebbrt::kprintf("Nil 2: %llu %llu %llu \n", count, count2, count3);

  c.Clear();
  i.Clear();
  m.Clear();
  count = c.Read();
  count2 = i.Read();
  count3 = m.Read();
  ebbrt::kprintf("Clear: %llu %llu %llu \n", count, count2, count3);


  c.Start();
  i.Start();
  m.Start();
  for( int i = 1; i<PRINT_ITERATIONS; i++){
    ebbrt::kprintf(".");
  }
  c.Stop();
  i.Stop();
  m.Stop();

  count = c.Read();
  count2 = i.Read();
  count3 = m.Read();
  ebbrt::kprintf("\nPrint: %llu %llu %llu \n", count, count2, count3);



  printer->Print("SANITY BACKEND FINISHED.\n"); 
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::kprintf("Shuting Down...\n"); });
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::acpi::PowerOff(); });
 }

