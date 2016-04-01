//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define RUNS 100000
#define ITERATIONS 100000

#include "Printer.h"
#include <ebbrt/Acpi.h>
#include <ebbrt/EventManager.h>
#include <ebbrt/Trace.h>

void AppMain() { 
  printer->Print("SANITY BACKEND UP.\n"); 
  ebbrt::kprintf("ITERATIONS:%llu\n",ITERATIONS);
  ebbrt::kprintf("[ status, func, n/a, time, cycles, instructions ]\n");
  ebbrt::trace::Init();
  ebbrt::trace::Enable();

  // OVERHEADS
  {
    for( int i = 0; i<RUNS; i++){
      auto start = ebbrt::trace::rdtsc();
      ebbrt::trace::AddTracepoint(0);
      auto end  = ebbrt::trace::rdtsc();
      ebbrt::kprintf("TP:%llu - %llu = %llu\n",end,start,(end-start)); 
    }
  }
  {
    for( int i = 0; i<RUNS; i++){
      auto start = ebbrt::trace::rdtsc();
      ebbrt::trace::AddTimestamp(0);
      auto end  = ebbrt::trace::rdtsc();
      ebbrt::kprintf("TP:%llu - %llu = %llu\n",end,start,(end-start)); 
    }
  }
  {
    for( int i = 0; i<RUNS; i++){
      auto start = ebbrt::trace::rdtsc();
      ebbrt::trace::AddNote("aaa");
      auto end  = ebbrt::trace::rdtsc();
      ebbrt::kprintf("TP:%llu - %llu = %llu\n",end,start,(end-start)); 
    }
  }
  ebbrt::trace::AddNote("000");
  ebbrt::trace::AddTracepoint(0);
  // sanity tests
  for( int i = 0; i<ITERATIONS; i++){
   asm volatile(""); 
  }
  ebbrt::trace::AddTracepoint(1);

  ebbrt::trace::AddNote("001");
  ebbrt::trace::AddTimestamp(0);
  // sanity tests
  for( int i = 0; i<ITERATIONS; i++){
   asm volatile(""); 
  }
  ebbrt::trace::AddTimestamp(1);

  ebbrt::trace::Disable();
  ebbrt::trace::Dump();
  printer->Print("SANITY BACKEND POWERING OFF.\n"); 
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::acpi::PowerOff(); });
 }

/*
  is = ebbrt::trace::rdpmc((reg));
  cs = ebbrt::trace::rdpmc((reg + 1));
  ts = ebbrt::trace::rdtsc();
  for (auto i = 0; i < ITERATIONS; i++) { ebbrt::trace::AddTracepoint(0);}
  for (auto i = 0; i < ITERATIONS; i++) { ebbrt::trace::AddNote("overhead");}
 *
 */
