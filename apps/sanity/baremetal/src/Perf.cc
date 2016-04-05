//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <ctime>
#include <cstdint>
#include <ebbrt/Debug.h>
#include "CpuAsm.h"
#include "Perf.h"

namespace {
};

ebbrt::perf::PerfCounter::PerfCounter(ebbrt::perf::PerfEvent evt) : is_running_{false}, evt_{evt} {

  uint32_t a,b,c,d;
  ebbrt::cpuid(PERF_CPUID_LEAF,0,&a, &b, &c, &d);
  pmc_version_ = a & 0xFF;
  pmc_gp_count_ = (a>>8) & 0xFF; 
  pmc_events_ = (a>>16) & 0xFF; 

  kassert( pmc_version_  );
  kassert( pmc_gp_count_ );

  evt_ = evt;
  //ia32_fixed_ctr_ctrl_t fixed_ctrl =0;
  //fixed_ctrl.val = 0;
  switch(evt_){
    case PerfEvent::fixed_cycles:
      break;
    case PerfEvent::fixed_instructions: 
      break;
    case PerfEvent::fixed_reference_cycles: 
      break;
    default:
      ebbrt::kbugon("PMC v1 not yet supported");
  };
  //  ebbrt::kassert( (pmc_events_ & PERF_EVENT_BITMASK(evt)) > 0 );
}


