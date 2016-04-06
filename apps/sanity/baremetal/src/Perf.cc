//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <ctime>
#include <cstdint>
#include <string.h>
#include <ebbrt/Debug.h>
#include "CpuAsm.h"
#include "Perf.h"

namespace {
thread_local uint64_t  perf_global_ctrl{0};
};

ebbrt::perf::PerfCounter::PerfCounter(ebbrt::perf::PerfEvent evt) : evt_{evt}, is_running_{false} {

  uint32_t a,b;
  ebbrt::cpuid(PERF_CPUID_LEAF,0,&a, &b, &b, &b);
  pmc_version_ = a & 0xFF;
  pmc_gp_count_ = (a>>8) & 0xFF; 
  pmc_events_ = (a>>16) & 0xFF; 
  kprintf("PMC: v%d #%d %x\n", pmc_version_, pmc_gp_count_, pmc_events_);
  kassert( pmc_gp_count_ > 0 );

  kprintf("EVT: %x\n", evt_);
  ia32_fixed_ctr_ctrl_t fixed_ctrl;
  fixed_ctrl.val = 0;
  //fixed_ctrl.val = ebbrt::rdmsr(IA32_FIXED_CTR_CTRL);
  switch(evt_){
    case PerfEvent::fixed_instructions: 
      kassert( pmc_version_  >= 2 );
      fixed_ctrl.ctr0_enable = 3;
      offset_ = ebbrt::rdpmc((FAST_READ(0)));
      break;
    case PerfEvent::fixed_cycles:
      kassert( pmc_version_  >= 2 );
      fixed_ctrl.ctr1_enable = 3;
      offset_ = ebbrt::rdpmc((FAST_READ(1)));
      break;
    case PerfEvent::fixed_reference_cycles: 
      kassert( pmc_version_  >= 2 );
      fixed_ctrl.ctr2_enable = 3;
      offset_ = ebbrt::rdpmc((FAST_READ(2)));
      break;
    default:
      ebbrt::kbugon("PMCv.1 not yet supported");
      break;
      //ebbrt::perf::ia32_perf_global_t ctrl;
      // ebbrt::wrmsr(ctrl.val, IA32_PERF_GLOBAL_CTRL);
  };

  if( fixed_ctrl.val != 0 ){
    auto tmp = ebbrt::rdmsr(IA32_FIXED_CTR_CTRL);
    fixed_ctrl.val |= tmp; 
    ebbrt::wrmsr(fixed_ctrl.val, IA32_FIXED_CTR_CTRL);
  }
}

void ebbrt::perf::PerfCounter::Clear(){
  switch(evt_){
    case PerfEvent::fixed_instructions: 
      offset_ = ebbrt::rdpmc((FAST_READ(0)));
      break;
    case PerfEvent::fixed_cycles:
      offset_ = ebbrt::rdpmc((FAST_READ(1)));
      break;
    case PerfEvent::fixed_reference_cycles: 
      offset_ = ebbrt::rdpmc((FAST_READ(2)));
      break;
    default:
      break;
  };
}

void ebbrt::perf::PerfCounter::Start(){
  auto shl = static_cast<uint8_t>(evt_);
  perf_global_ctrl |= (1ull << shl); 
  ebbrt::wrmsr(perf_global_ctrl, IA32_PERF_GLOBAL_CTRL);
}

void ebbrt::perf::PerfCounter::Stop(){
  auto shl = static_cast<uint8_t>(evt_);
  perf_global_ctrl &= ~(1ull << shl); 
  ebbrt::wrmsr(perf_global_ctrl, IA32_PERF_GLOBAL_CTRL);
}
uint64_t ebbrt::perf::PerfCounter::Read(){
  switch(evt_){
    case PerfEvent::fixed_instructions: 
      return ebbrt::rdpmc((FAST_READ(0))) - offset_;
    case PerfEvent::fixed_cycles:
      return ebbrt::rdpmc((FAST_READ(1))) - offset_;
    case PerfEvent::fixed_reference_cycles: 
      return ebbrt::rdpmc((FAST_READ(2))) - offset_;
      break;
    default:
      break;
  };
  return 0;
}
