//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define RUNS 100
#define ITERATIONS 100000

const unsigned int reg = 1 << 30;  // Intel fixed-purpose-register config flag

#include "Printer.h"
#include "Profiler.h"
#include <ebbrt/Acpi.h>
#include <ebbrt/EventManager.h>
#include <ebbrt/Trace.h>

void AppMain() { 
  printer->Print("SANITY BACKEND UP.\n"); 
  printer->Print("SANITY BACKEND POWERING OFF.\n"); 
  ebbrt::profiler::Duration d;

  ebbrt::profiler::Profiler prof;
  prof.tick();
  for( int i = 0; i<ITERATIONS; i++){
   asm volatile(""); 
  }
  prof.tock();
  d = prof.get();
  ebbrt::kprintf("1 %llu %llu %llu %llu \n", d.tsc,
    d.wct, d.instructions, d.cycles);
  ////
  //prof.tick();
  //for( int i = 0; i<ITERATIONS*100; i++){
  // asm volatile(""); 
  //}
  //prof.tock();
  //ebbrt::kprintf("2 %llu\n", prof.get());
  ////
  //prof.tick();
  //for( int i = 0; i<ITERATIONS*1000; i++){
  // asm volatile(""); 
  //}
  //prof.tock();
  //ebbrt::kprintf("3 %llu\n", prof.get());

  //for( int i = 0; i<ITERATIONS*10000; i++){
  // asm volatile(""); 
  //}

  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::kprintf("Powering off...\n"); });
  ebbrt::event_manager->SpawnLocal([=]() { ebbrt::acpi::PowerOff(); });
//  ebbrt::kprintf("ITERATIONS:%llu\n",ITERATIONS);
//  ebbrt::kprintf("[ status, func, n/a, time, cycles, instructions ]\n");
//  ebbrt::trace::Init();
//  ebbrt::trace::Enable();
//
//  // OVERHEADS
//  {
//    for( int i = 0; i<RUNS; i++){
// //     auto start = ebbrt::trace::rdtsc();
// //     auto start = ebbrt::trace::rdpmc((reg));
//      auto start = ebbrt::trace::rdpmc((reg+1));
//      ebbrt::trace::AddTracepoint(0);
//      auto end = ebbrt::trace::rdpmc((reg+1));
// //     auto end = ebbrt::trace::rdpmc((reg));
// //     auto end  = ebbrt::trace::rdtsc();
//      ebbrt::kprintf("TP:%llu - %llu = %llu\n",end,start,(end-start)); 
//    }
//  }
//  {
//    for( int i = 0; i<RUNS; i++){
// //     auto start = ebbrt::trace::rdtsc();
// //     auto start = ebbrt::trace::rdpmc((reg));
//      auto start = ebbrt::trace::rdpmc((reg+1));
//      ebbrt::trace::AddTimestamp(0);
//      auto end = ebbrt::trace::rdpmc((reg+1));
// //     auto end = ebbrt::trace::rdpmc((reg));
// //     auto end  = ebbrt::trace::rdtsc();
//      ebbrt::kprintf("TS:%llu - %llu = %llu\n",end,start,(end-start)); 
//    }
//  }
//  {
//    for( int i = 0; i<RUNS; i++){
//  //    auto start = ebbrt::trace::rdtsc();
// //     auto start = ebbrt::trace::rdpmc((reg));
//      auto start = ebbrt::trace::rdpmc((reg+1));
//      ebbrt::trace::AddNote("aaa");
//      auto end = ebbrt::trace::rdpmc((reg+1));
// //     auto end = ebbrt::trace::rdpmc((reg));
//  //    auto end  = ebbrt::trace::rdtsc();
//      ebbrt::kprintf("NT:%llu - %llu = %llu\n",end,start,(end-start)); 
//    }
//  }
//  ebbrt::trace::AddNote("000");
//  ebbrt::trace::AddTracepoint(0);
//  // sanity tests
//  for( int i = 0; i<ITERATIONS; i++){
//   asm volatile(""); 
//  }
//  ebbrt::trace::AddTracepoint(1);
//
//  ebbrt::trace::AddNote("001");
//  ebbrt::trace::AddTimestamp(0);
//  // sanity tests
//  for( int i = 0; i<ITERATIONS; i++){
//   asm volatile(""); 
//  }
//  ebbrt::trace::AddTimestamp(1);
//
//  ebbrt::trace::Disable();
//  ebbrt::trace::Dump();
 }

/*
  is = ebbrt::trace::rdpmc((reg));
  cs = ebbrt::trace::rdpmc((reg + 1));
  ts = ebbrt::trace::rdtsc();
  for (auto i = 0; i < ITERATIONS; i++) { ebbrt::trace::AddTracepoint(0);}
  for (auto i = 0; i < ITERATIONS; i++) { ebbrt::trace::AddNote("overhead");}
 *
 */
