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
thread_local uint64_t perf_global_ctrl{0};
thread_local uint8_t  next_pmc{0};
};

ebbrt::perf::PerfCounter::PerfCounter(ebbrt::perf::PerfEvent evt) : evt_{evt}, is_running_{false} {

  uint32_t a,b,c,d;
  ebbrt::cpuid(PERF_CPUID_LEAF,0,&a, &b, &c, &d);
  pmc_version_ = a & 0xFF;
  pmc_count_ = (a>>8) & 0xFF; 
  pmc_width_ = (a>>16) & 0xFF; 
  pmc_events_ = (b) & 0xFF; 
  pmc_fixed_count_ = (d) & 0xF; 
  pmc_fixed_width_ = (d>>4) & 0xFF; 

  kassert( pmc_version_  >= 1 );
  kassert( pmc_count_ > 0 );
  if( (uint8_t)evt_ >= FIXED_EVT_OFFSET )
    kassert( pmc_version_  >= 2 );
  else{
    auto not_valid = (pmc_events_ >> (uint8_t)evt_);
    kassert( not_valid == 0);
  }

  ia32_fixed_ctr_ctrl_t fixed_ctrl;
  ia32_perfevtsel_t perfevtsel;
  fixed_ctrl.val = ebbrt::rdmsr(IA32_FIXED_CTR_CTRL_MSR); 
  perfevtsel.val = 0;
  perfevtsel.usermode = 1;
  perfevtsel.osmode = 1;
  //perfevtsel.en = 1;
  switch(evt_){
    case PerfEvent::cycles:
      perfevtsel.event = PERFEVTSEL_EVT_CYCLES;
      perfevtsel.umask = PERFEVTSEL_UMASK_CYCLES;
      ebbrt::wrmsr(perfevtsel.val, IA32_PERFEVTSEL_MSR(next_pmc));
      next_pmc++;
      break;
    case PerfEvent::instructions:
      perfevtsel.event = PERFEVTSEL_EVT_INSTRUCTIONS;
      perfevtsel.umask = PERFEVTSEL_UMASK_INSTRUCTIONS;
      ebbrt::wrmsr(perfevtsel.val, IA32_PERFEVTSEL_MSR(next_pmc));
      next_pmc++;
      break;
    case PerfEvent::reference_cycles:
      perfevtsel.event = PERFEVTSEL_EVT_CYCLES_REF;
      perfevtsel.umask = PERFEVTSEL_UMASK_CYCLES_REF;
      ebbrt::wrmsr(perfevtsel.val, IA32_PERFEVTSEL_MSR(next_pmc));
      next_pmc++;
      break;
    case PerfEvent::llc_references:
      perfevtsel.event = PERFEVTSEL_EVT_LLC_REF;
      perfevtsel.umask = PERFEVTSEL_UMASK_LLC_REF;
      ebbrt::wrmsr(perfevtsel.val, IA32_PERFEVTSEL_MSR(next_pmc));
      next_pmc++;
      break;
    case PerfEvent::llc_misses:
      perfevtsel.event = PERFEVTSEL_EVT_LLC_MISSES;
      perfevtsel.umask = PERFEVTSEL_UMASK_LLC_MISSES;
      ebbrt::wrmsr(perfevtsel.val, IA32_PERFEVTSEL_MSR(next_pmc));
      next_pmc++;
      break;
    case PerfEvent::branch_instructions:
      perfevtsel.event = PERFEVTSEL_EVT_BRANCH_INSTRUCTIONS;
      perfevtsel.umask = PERFEVTSEL_UMASK_BRANCH_INSTRUCTIONS;
      ebbrt::wrmsr(perfevtsel.val, IA32_PERFEVTSEL_MSR(next_pmc));
      next_pmc++;
      break;
    case PerfEvent::branch_misses:
      perfevtsel.event = PERFEVTSEL_EVT_BRANCH_MISSES;
      perfevtsel.umask = PERFEVTSEL_UMASK_BRANCH_MISSES;
      ebbrt::wrmsr(perfevtsel.val, IA32_PERFEVTSEL_MSR(next_pmc));
      next_pmc++;
      break;
    case PerfEvent::fixed_instructions: 
      fixed_ctrl.ctr0_enable = FIXED_CTL_RING_LVL_ALL;
      offset_ = ebbrt::rdpmc((FAST_READ(0)));
      ebbrt::wrmsr(fixed_ctrl.val, IA32_FIXED_CTR_CTRL_MSR);
      break;
    case PerfEvent::fixed_cycles:
      fixed_ctrl.ctr1_enable = FIXED_CTL_RING_LVL_ALL;
      offset_ = ebbrt::rdpmc((FAST_READ(1)));
      fixed_ctrl.val |= ebbrt::rdmsr(IA32_FIXED_CTR_CTRL_MSR); 
      ebbrt::wrmsr(fixed_ctrl.val, IA32_FIXED_CTR_CTRL_MSR);
      break;
    case PerfEvent::fixed_reference_cycles: 
      fixed_ctrl.ctr2_enable = FIXED_CTL_RING_LVL_ALL;
      offset_ = ebbrt::rdpmc((FAST_READ(2)));
      fixed_ctrl.val |= ebbrt::rdmsr(IA32_FIXED_CTR_CTRL_MSR); 
      ebbrt::wrmsr(fixed_ctrl.val, IA32_FIXED_CTR_CTRL_MSR);
      break;
    default:
       kbugon("Error: unknown event type");
      break;
  };
  return;
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

int8_t ebbrt::perf::PerfCounter::AvailablePMCs(){
  return (( (int8_t)pmc_count_ - (int8_t)next_pmc ));
}

void ebbrt::perf::PerfCounter::Start(){
  auto shl = static_cast<uint8_t>(evt_);
  perf_global_ctrl |= (1ull << shl); 
  ebbrt::wrmsr(perf_global_ctrl, IA32_PERF_GLOBAL_CTRL_MSR);
}

void ebbrt::perf::PerfCounter::Stop(){
  auto shl = static_cast<uint8_t>(evt_);
  perf_global_ctrl &= ~(1ull << shl); 
  ebbrt::wrmsr(perf_global_ctrl, IA32_PERF_GLOBAL_CTRL_MSR);
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
