//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef APP_SRC_INCLUDE_EBBRT_CPUASM_H_
#define APP_SRC_INCLUDE_EBBRT_CPUASM_H_

namespace ebbrt {
inline uintptr_t ReadCr3() {
  uintptr_t cr3;
  asm volatile("mov %%cr3, %[cr3]" : [cr3] "=r"(cr3));
  return cr3;
}

inline uint64_t rdpmc(uint64_t reg) {
  uint32_t lo, hi;
  __asm__ __volatile__("rdpmc" : "=a"(lo), "=d"(hi) : "c"(reg));
  return ((uint64_t)lo) | (((uint64_t)hi) << 32);
}
inline void cpuid(uint32_t eax, uint32_t ecx, uint32_t* a, uint32_t* b,
                  uint32_t* c, uint32_t* d) {
  __asm__ __volatile__("cpuid"
                       : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                       : "a"(eax), "c"(ecx));
}

inline uint64_t rdtsc() {
  uint64_t tsc;
  asm volatile("rdtsc;"
               "shl $32,%%rdx;"
               "or %%rdx,%%rax"
               : "=a"(tsc)
               :
               : "%rcx", "%rdx");
  return tsc;
}

inline uint64_t rdtscp() {
  uint64_t tsc;
  asm volatile("rdtscp;"
               "shl $32,%%rdx;"
               "or %%rdx,%%rax"
               : "=a"(tsc)
               :
               : "%rcx", "%rdx");
  return tsc;
}

inline void wrmsr(uint64_t val, uint32_t msr){
    __asm__ __volatile__("wrmsr"
                         :
                         : "a"(val & 0xFFFFFFFF),
                           "d"(val >> 32), "c"(msr));
}

inline uint64_t rdmsr(uint32_t msr){
  uint64_t val;
  asm volatile("rdmsr;" : "=A"(val) : "c" (msr));
  return val;
}

}

#endif  // BAREMETAL_SRC_INCLUDE_EBBRT_CPUASM_H_
