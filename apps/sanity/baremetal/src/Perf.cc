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
};

ebbrt::perf::PerfCounter::PerfCounter(ebbrt::perf::PerfEvent evt) : is_running_{false}, evt_{evt} {

  int x;
  char info[13];
  ebbrt::cpuid(0, 0, reinterpret_cast<uint32_t*>(&x),
        reinterpret_cast<uint32_t*>(&info[0]),
        reinterpret_cast<uint32_t*>(&info[8]),
        reinterpret_cast<uint32_t*>(&info[4]));
  info[12] = '\0';
  if (strcmp(info, "GenuineIntel") != 0)
    kprintf("KVM trace virtualization not fully supported on %s\n", info);

  uint32_t a,b,c,d;
  ebbrt::cpuid(PERF_CPUID_LEAF,0,&a, &b, &c, &d);
  pmc_version_ = a & 0xFF;
  pmc_gp_count_ = (a>>8) & 0xFF; 
  pmc_events_ = (a>>16) & 0xFF; 
  kprintf("PMC: v%d #%d %x\n", pmc_version_, pmc_gp_count_, pmc_events_);

  kassert( pmc_version_  >= 2 );
  kassert( pmc_gp_count_ > 0 );

  kprintf("EVT: %x\n", evt_);
  ia32_fixed_ctr_ctrl_t fixed_ctrl;
  fixed_ctrl.val = 0;
  switch(evt_){
    case PerfEvent::fixed_cycles:
      kprintf("Fixed cycles event\n");
      fixed_ctrl.ctr0_enable = 3;
      fixed_ctrl.ctr1_enable = 3;
      fixed_ctrl.ctr2_enable = 3;
      break;
    case PerfEvent::fixed_instructions: 
      kprintf("Fixed inst event\n");
      fixed_ctrl.ctr1_enable = 3;
      break;
    case PerfEvent::fixed_reference_cycles: 
      kprintf("Fixed ref cycles event\n");
      fixed_ctrl.ctr2_enable = 3;
      break;
    default:
      ebbrt::kbugon("PMCv.1 not yet supported");
  };
  ebbrt::perf::ia32_perf_global_t ctrl;
  ctrl.val = 0;
  ebbrt::wrmsr(fixed_ctrl.val, IA32_FIXED_CTR_CTRL);
  ebbrt::wrmsr(ctrl.val, IA32_PERF_GLOBAL_CTRL);
}

void ebbrt::perf::PerfCounter::Start(){
  ebbrt::perf::ia32_perf_global_t ctrl;
  ctrl.val = 0;
  ctrl.ctr0 = 1;
  ctrl.ctr1 = 1;
  ctrl.ctr2 = 1;
  ebbrt::kprintf("ctrl.val%llu: %llx\n", evt_, ctrl.val);
  ebbrt::wrmsr(ctrl.val, IA32_PERF_GLOBAL_CTRL);
}
void ebbrt::perf::PerfCounter::Stop(){

}
uint64_t ebbrt::perf::PerfCounter::Read(){
      uint64_t pmc = 1<<30; 
  switch(evt_){
    case PerfEvent::fixed_cycles:
      ebbrt::kprintf("Reading from 0x%llx", pmc);
      return ebbrt::rdpmc(pmc);
    case PerfEvent::fixed_instructions: 
      return ebbrt::rdpmc((FAST_READ(1)));
      break;
    case PerfEvent::fixed_reference_cycles: 
      return ebbrt::rdpmc((FAST_READ(2)));
      break;
    default:
      break;
  };
  return 0;
}
