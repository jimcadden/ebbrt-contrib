//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define ITERATIONS 100000

#include "Printer.h"
#include <ebbrt/Acpi.h>
#include <ebbrt/EventManager.h>
#include <ebbrt/Trace.h>

void AppMain() { 
  printer->Print("SANITY BACKEND UP.\n"); 
  ebbrt::trace::Init();
  ebbrt::trace::Enable();

  ebbrt::trace::AddNote("NULL Test");
  ebbrt::trace::AddTracepoint(0);
  ebbrt::trace::AddTimestamp(0);
  // sanity tests
  for( int i = 0; i<ITERATIONS; i++){
   asm volatile(""); 
  }
  ebbrt::trace::AddTimestamp(1);
  ebbrt::trace::AddTracepoint(1);

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
