#ifndef SANITY_SRC_INCLUDE_EBBRT_PERF_H_
#define SANITY_SRC_INCLUDE_EBBRT_PERF_H_

#include <atomic>
#include <cstdint>
#include <string>
#include "CpuAsm.h"

namespace ebbrt {
namespace perf {
#define PERF_CPUID_LEAF                       0x0A
#define FAST_READ                             1 << 30

#define IA32_FIXED_CTR_CTRL                   0x38D
#define IA32_PERF_GLOBAL_CTRL                 0x38F
#define IA32_PERF_GLOBAL_STATUS               0x38E
#define IA32_PERF_GLOBAL_OVF_CTRL             0x390
#define IA32_PERFEVTSEL(X)                    (0x186+(X))
#define IA32_FXD_CTR(X)                       (0x309+(X))
#define IA32_GP_CTR(X)                        (0x4C1+(X))
#define PERF_EVENT_BITMASK(X)                 1 << X

#define PERFEVT_UMASK_CYCLES                  0x00
#define PERFEVT_UMASK_CYCLES_REF              0x00
#define PERFEVT_UMASK_INSTRUCTIONS            0x01
#define PERFEVT_UMASK_LLC_REF                 0x4f
#define PERFEVT_UMASK_LLC_MISSES              0x41
#define PERFEVT_UMASK_BRANCH_INSTRUCTIONS     0x00
#define PERFEVT_UMASK_BRANCH_MISSES           0x00

#define PERFEVT_SEL_CYCLES                    0x3C
#define PERFEVT_SEL_CYCLES_REF                0xC0
#define PERFEVT_SEL_INSTRUCTIONS              0x3C
#define PERFEVT_SEL_LLC_REF                   0x2E
#define PERFEVT_SEL_LLC_MISSES                0x2E
#define PERFEVT_SEL_BRANCH_INSTRUCTIONS       0xC4
#define PERFEVT_SEL_BRANCH_MISSES             0xC5
#define FIXED_CTR_OFFSET                      0x20

enum class PerfEvent : char {
    cycles = 0x0,
    instructions, 
    reference_cycles,
    llc_references,
    llc_misses,
    branch_instructions,
    branch_misses,
    fixed_cycles = FIXED_CTR_OFFSET,
    fixed_instructions,
    fixed_reference_cycles
};

typedef struct {
  union {
    uint64_t val;
    struct {
      uint64_t eventselect_7_0 : 8;
      uint64_t unitmask : 8;
      uint64_t usermode : 1;
      uint64_t osmode : 1;
      uint64_t edge : 1;
      uint64_t pin : 1;
      uint64_t enint : 1;
      uint64_t reserved2 : 1;
      uint64_t en : 1;
      uint64_t inv : 1;
      uint64_t cntmask : 8;
      uint64_t reserved0 : 32;
    } __attribute__((packed));
  };
} ia32_perfevtsel_t;

typedef struct {
  union {
    uint64_t val;
    struct {
      uint64_t pci0 : 1;
      uint64_t pci1 : 1;
      uint64_t pci2 : 1;
      uint64_t pci3 : 1;
      uint64_t pci4 : 1;
      uint64_t pci5 : 1;
      uint64_t pci6 : 1;
      uint64_t pci7 : 1;
      uint64_t reserved0 : 24;
      uint64_t ctr0 : 1;
      uint64_t ctr1 : 1;
      uint64_t ctr2 : 1;
      uint64_t reserved1 : 27;
      uint64_t ovfbuf : 1;
      uint64_t conchd : 1;
    } __attribute__((packed));
  };
} ia32_perf_global_t;


typedef struct {
  union {
    uint64_t val;
    struct {
      uint64_t ctr0_enable : 2;
      uint64_t ctr0_reserved : 1;
      uint64_t ctr0_pmi : 1;
      uint64_t ctr1_enable : 2;
      uint64_t ctr1_reserved : 1;
      uint64_t ctr1_pmi : 1;
      uint64_t ctr2_enable : 2;
      uint64_t ctr2_reserved : 1;
      uint64_t ctr2_pmi : 1;
      uint64_t reserved : 52;
    } __attribute__((packed));
  };
} ia32_fixed_ctr_ctrl_t;


class PerfCounter {
public:
  PerfCounter(PerfEvent e);
  void clear();
  bool overflow();
  uint64_t read();
  void start();
  void stop();
private:
  std::atomic<bool> is_running_;
  PerfEvent evt_;
  uint8_t pmc_gp_count_;
  uint8_t pmc_version_;
  uint8_t pmc_events_;
};

}
}  // namespace ebbrt
#endif
