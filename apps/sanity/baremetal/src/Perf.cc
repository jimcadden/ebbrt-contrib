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

ebbrt::perf::PerfCounter::PerfCounter(ebbrt::perf::PerfEvent evt) : evt_{evt} {

  uint32_t a,b,c,d;
  ebbrt::cpuid(PERF_CPUID_LEAF,0,&a, &b, &c, &d);
  pmc_version_ = a & 0xFF;
  pmc_count_ = (a>>8) & 0xFF; 
  pmc_width_ = (a>>16) & 0xFF; 
  pmc_events_ = (b) ;//& 0xFF; 
  pmc_fixed_count_ = (d) & 0xF; 
  pmc_fixed_width_ = (d>>4) & 0xFF; 
  evt_num_ = static_cast<uint8_t>(evt_);
  pmc_num_ = next_pmc;
  next_pmc++;

  //TODO(jmc): disable counter, avoid abort
  kassert( pmc_version_  >= 1 );
  kprintf("PMC version: %d\n", pmc_version_ );
  kassert( pmc_num_ < pmc_count_ );
  kprintf("PMC count: %d\n", pmc_count_ );
  kprintf("PMC event: %d\n", evt_num_ );
  kprintf("PMC events mask: %d\n", pmc_events_ );
  // fixed_event counters require PMC version >= 2
  if( evt_num_ >= FIXED_EVT_OFFSET(0) )
    kassert( pmc_version_  >= 2 );
  else{
    auto not_valid = (pmc_events_ >> evt_num_) & 0x1;
    kassert(not_valid == 0);
  }

  ia32_fixed_ctr_ctrl_t fixed_ctrl;
  ia32_perfevtsel_t perfevtsel;
  perfevtsel.val = 0;
  fixed_ctrl.val = 0; 

  switch(evt_){
    case PerfEvent::cycles:
      perfevtsel.event = PERFEVTSEL_EVT_CYCLES;
      perfevtsel.umask = PERFEVTSEL_UMASK_CYCLES;
      break;
    case PerfEvent::instructions:
      perfevtsel.event = PERFEVTSEL_EVT_INSTRUCTIONS;
      perfevtsel.umask = PERFEVTSEL_UMASK_INSTRUCTIONS;
      break;
    case PerfEvent::reference_cycles:
      perfevtsel.event = PERFEVTSEL_EVT_CYCLES_REF;
      perfevtsel.umask = PERFEVTSEL_UMASK_CYCLES_REF;
      break;
    case PerfEvent::llc_references:
      perfevtsel.event = PERFEVTSEL_EVT_LLC_REF;
      perfevtsel.umask = PERFEVTSEL_UMASK_LLC_REF;
      break;
    case PerfEvent::llc_misses:
      perfevtsel.event = PERFEVTSEL_EVT_LLC_MISSES;
      perfevtsel.umask = PERFEVTSEL_UMASK_LLC_MISSES;
      break;
    case PerfEvent::branch_instructions:
      perfevtsel.event = PERFEVTSEL_EVT_BRANCH_INSTRUCTIONS;
      perfevtsel.umask = PERFEVTSEL_UMASK_BRANCH_INSTRUCTIONS;
      break;
    case PerfEvent::branch_misses:
      perfevtsel.event = PERFEVTSEL_EVT_BRANCH_MISSES;
      perfevtsel.umask = PERFEVTSEL_UMASK_BRANCH_MISSES;
      break;
    case PerfEvent::fixed_instructions: 
      fixed_ctrl.ctr0_enable = FIXED_CTL_RING_LVL_ALL;
      counter_offset_ = ebbrt::rdmsr(IA32_FXD_PMC(0));
      pmc_num_ = FIXED_EVT_OFFSET(0);
      next_pmc--;
      break;
    case PerfEvent::fixed_cycles:
      fixed_ctrl.ctr1_enable = FIXED_CTL_RING_LVL_ALL;
      counter_offset_ = ebbrt::rdmsr(IA32_FXD_PMC(1));
      pmc_num_ = FIXED_EVT_OFFSET(1);
      next_pmc--;
      break;
    case PerfEvent::fixed_reference_cycles: 
      fixed_ctrl.ctr2_enable = FIXED_CTL_RING_LVL_ALL;
      counter_offset_ = ebbrt::rdmsr(IA32_FXD_PMC(2));
      pmc_num_ = FIXED_EVT_OFFSET(2);
      next_pmc--;
      break;
    default:
      kprintf("Warning: unknown performance monitor counter \n"); 
      break;
  };

  if( fixed_ctrl.val != 0 ){
    fixed_ctrl.val |= ebbrt::rdmsr(IA32_FIXED_CTR_CTRL_MSR); 
    ebbrt::wrmsr(fixed_ctrl.val, IA32_FIXED_CTR_CTRL_MSR);
    return;
  }
  
  if ( perfevtsel.val != 0 ){
    perfevtsel.usermode = 1;
    perfevtsel.osmode = 1;
    perfevtsel.en = 1;
    ebbrt::wrmsr(perfevtsel.val, IA32_PERFEVTSEL_MSR(pmc_num_));
    counter_offset_ = ebbrt::rdmsr(IA32_PMC(pmc_num_));
    return;
  }
    
  return;
}

void ebbrt::perf::PerfCounter::Clear(){
  switch(evt_){
    case PerfEvent::cycles:
    case PerfEvent::instructions:
    case PerfEvent::reference_cycles:
    case PerfEvent::llc_references:
    case PerfEvent::llc_misses:
    case PerfEvent::branch_instructions:
    case PerfEvent::branch_misses:
      counter_offset_ = ebbrt::rdmsr(IA32_PMC(pmc_num_));
      break;
    case PerfEvent::fixed_instructions: 
      counter_offset_ = ebbrt::rdmsr(IA32_FXD_PMC(0));
      break;
    case PerfEvent::fixed_cycles:
      counter_offset_ = ebbrt::rdmsr(IA32_FXD_PMC(1));
      break;
    case PerfEvent::fixed_reference_cycles: 
      counter_offset_ = ebbrt::rdmsr(IA32_FXD_PMC(2));
      break;
    default:
      break;
  };
  return;
}

int8_t ebbrt::perf::PerfCounter::AvailablePMCs(){
  return pmc_count_ - next_pmc;
}

void ebbrt::perf::PerfCounter::Start(){
  perf_global_ctrl |= (1ull << pmc_num_); 
  ebbrt::wrmsr(perf_global_ctrl, IA32_PERF_GLOBAL_CTRL_MSR);
}

void ebbrt::perf::PerfCounter::Stop(){
  perf_global_ctrl &= ~(1ull << pmc_num_); 
  ebbrt::wrmsr(perf_global_ctrl, IA32_PERF_GLOBAL_CTRL_MSR);
}
uint64_t ebbrt::perf::PerfCounter::Read(){
  switch(evt_){
    case PerfEvent::cycles:
    case PerfEvent::instructions:
    case PerfEvent::reference_cycles:
    case PerfEvent::llc_references:
    case PerfEvent::llc_misses:
    case PerfEvent::branch_instructions:
    case PerfEvent::branch_misses:
        return ebbrt::rdmsr(IA32_PMC(pmc_num_)) - counter_offset_;
    case PerfEvent::fixed_instructions: 
        return ebbrt::rdmsr((IA32_FXD_PMC(0))) - counter_offset_;
    case PerfEvent::fixed_cycles:
        return ebbrt::rdmsr((IA32_FXD_PMC(1))) - counter_offset_;
    case PerfEvent::fixed_reference_cycles: 
        return ebbrt::rdmsr((IA32_FXD_PMC(2))) - counter_offset_;
      break;
    default:
      break;
  };
  return 0;
}
