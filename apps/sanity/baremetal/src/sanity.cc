//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define PAGES                 4
#define ELEMENTS           ((1<<17)) 
#define M_LEN    PAGES*ELEMENTS
#define NIL_ITERATIONS  1000000

#define MATRIX_TEST     0
#define NIL_LOOP        1

#include <unordered_map>
#include <ebbrt/Acpi.h>
#include <ebbrt/EventManager.h>
#include <ebbrt/Perf.h>
#include "Printer.h"

void AppMain() { 

  auto data = new uint64_t[M_LEN];
  for( auto i=0ull; i<(M_LEN); i++){
    data[i] = i;
  }

  std::unordered_map<std::string, ebbrt::perf::PerfCounter> counters;

 // counters.emplace(std::make_pair(std::string("cycles      "), 
 //       std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::fixed_cycles})));
  counters.emplace(std::make_pair(std::string("instructions"), 
        std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::fixed_instructions})));
 // counters.emplace(std::make_pair(std::string("tlb_load_miss"), 
 //       std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::tlb_load_misses})));
 // counters.emplace(std::make_pair(std::string("tlb_store_miss"), 
 //       std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::tlb_store_misses})));
 // counters.emplace(std::make_pair(std::string("llc_misses"), 
 //       std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::llc_misses})));
 // counters.emplace(std::make_pair(std::string("llc_references"), 
 //       std::move(ebbrt::perf::PerfCounter{ebbrt::perf::PerfEvent::llc_references})));

#if NIL_LOOP
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
#endif

#if MATRIX_TEST
  ebbrt::kprintf("\n Sum matrix counter w/ clflush:\n");
  for( auto& i : counters ){
    i.second.Start();
    asm volatile ("cpuid" : : : "eax", "ebx", "ecx", "edx");
    auto sum = 0ull;
    for( auto j=0ull; j<(M_LEN); j++){
      auto addr=&data[j];
      asm volatile("clflush %[addr]" : [addr] "+m" (addr)); 
      //asm volatile("invlpg %[addr]" : [addr] "+m" (addr) : : "memory"); 
      sum += data[j]; 
    }
    i.second.Stop();
    ebbrt::kprintf("%s\t\t%llu\t\t%llu\n", i.first.c_str(), i.second.Read(), sum);
    i.second.Clear();
  }

  ebbrt::kprintf("\n Sum matrix counter w/ clear cr3:\n");
  for( auto& i : counters ){
      unsigned long val;
      asm volatile("mov %%cr3, %[val]" : [val] "=r" (val));
      asm volatile("mov %[val], %%cr3" : : [val] "r" (val));
    i.second.Start();
    asm volatile ("cpuid" : : : "eax", "ebx", "ecx", "edx");
    auto sum = 0ull;
    for( auto j=0ull; j<(M_LEN); j++){
      //auto addr=&data[j];
      ////asm volatile("clflush %[addr]" : [addr] "+m" (addr)); 
      //asm volatile("invlpg %[addr]" : [addr] "+m" (addr) : : "memory"); 
      sum += data[j]; 
    }
    i.second.Stop();
    ebbrt::kprintf("%s\t\t%llu\t\t%llu\n", i.first.c_str(), i.second.Read(), sum);
    i.second.Clear();
  }

  ebbrt::kprintf("\n Write matrix counter w/ clear cr3:\n");
  for( auto& i : counters ){
      unsigned long val;
      asm volatile("mov %%cr3, %[val]" : [val] "=r" (val));
      asm volatile("mov %[val], %%cr3" : : [val] "r" (val));
    i.second.Start();
    asm volatile ("cpuid" : : : "eax", "ebx", "ecx", "edx");
    auto sum = 0ull;
    for( auto j=0ull; j<(M_LEN); j++){
      //auto addr=&data[j];
      ////asm volatile("clflush %[addr]" : [addr] "+m" (addr)); 
      //asm volatile("invlpg %[addr]" : [addr] "+m" (addr) : : "memory"); 
      data[j]=0; 
    }
    i.second.Stop();
    ebbrt::kprintf("%s\t\t%llu\t\t%llu\n", i.first.c_str(), i.second.Read(), sum);
    i.second.Clear();
  }
#endif

  delete[] data;

  printer->Print("BACKEND FINISHED.\n"); 
//  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::kprintf("Shuting Down...\n"); });
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::acpi::PowerOff(); });
 }

