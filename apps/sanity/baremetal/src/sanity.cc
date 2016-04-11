//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define PRINT_ITERATIONS 100
#define NIL_ITERATIONS 10000

#include <unordered_map>
#include <ebbrt/Acpi.h>
#include <ebbrt/EventManager.h>
#include <ebbrt/Perf.h>
#include "Printer.h"

void AppMain() { 

  printer->Print("SANITY BACKEND UP.\n"); 
  std::unordered_map<std::string, ebbrt::perf::PerfCounter> counter;


  counter.emplace(std::make_pair(std::string("cycles"), 
        std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::cycles})));
  counter.emplace(std::make_pair(std::string("instructions"), 
        std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::instructions})));

  //counter.emplace(std::make_pair(std::string("llc_misses"), 
  //      std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::llc_misses})));
  //counter.emplace(std::make_pair(std::string("branch_misses"), 
  //      std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::branch_misses})));

  for( int i = 1; i<NIL_ITERATIONS; i++){
    asm volatile(""); 
  }

  printer->Print("SANITY BACKEND FINISHED.\n"); 
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::kprintf("Shuting Down...\n"); });
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::acpi::PowerOff(); });
 }

