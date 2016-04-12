//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define PAGES               10
#define NIL_ITERATIONS 1000000

#include <unordered_map>
#include <ebbrt/Acpi.h>
#include <ebbrt/EventManager.h>
#include <ebbrt/Perf.h>
#include "Printer.h"

void AppMain() { 

  printer->Print("BACKEND UP.\n"); 

  uint64_t data[64 * PAGES];
  for( auto i=0ull; i<(64*PAGES); i++){
    data[i] = i;
  }
  ebbrt::kprintf("array allocated: %d - %d\n", data[0], data[64*PAGES-1]);

  std::unordered_map<std::string, ebbrt::perf::PerfCounter> counters;
  counters.emplace(std::make_pair(std::string("ref_cycles"), 
        ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::reference_cycles}));
  counters.emplace(std::make_pair(std::string("instructions"), 
        std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::instructions})));
  counters.emplace(std::make_pair(std::string("llc_misses"), 
        std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::llc_misses})));
  counters.emplace(std::make_pair(std::string("branch_misses"), 
        std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::branch_misses})));

  ebbrt::kprintf("\nInital counter values:\n");
  for( auto& i : counters ){
    ebbrt::kprintf("%s\t\t%llu\n", i.first.c_str(), i.second.Read());
  }

  ebbrt::kprintf("\n Nil counter:\n");
  for( auto& i : counters ){
    i.second.Start();
    for( int i = 1; i<NIL_ITERATIONS; i++){
      asm volatile(""); 
    }
    i.second.Stop();
    ebbrt::kprintf("%s\t\t%llu\n", i.first.c_str(), i.second.Read());
    i.second.Clear();
  }

  ebbrt::kprintf("\n Sum matrix counter:\n");
  for( auto& i : counters ){
    i.second.Start();
    auto sum = 0ull;
    for( auto j=0ull; j<(64*PAGES); j++){
      sum += data[j];
    }
    i.second.Stop();
    ebbrt::kprintf("%s\t\t%llu\t\t%llu\n", i.first.c_str(), i.second.Read(), sum);
    i.second.Clear();
  }

  printer->Print("BACKEND FINISHED.\n"); 
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::kprintf("Shuting Down...\n"); });
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::acpi::PowerOff(); });
 }

