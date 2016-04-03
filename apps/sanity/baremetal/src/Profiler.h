#include <chrono>
#include <cstdint>
#include <ebbrt/Clock.h>

#define IF_INTEL 1

namespace ebbrt {
namespace profiler {

typedef std::chrono::nanoseconds ns;
typedef std::chrono::seconds s;

const constexpr uint32_t reg =
    1 << 30; // Intel fixed-purpose-register config flag

inline uint64_t rdtsc(void) {
  uint32_t lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)lo) | (((uint64_t)hi) << 32);
}
inline uint64_t rdpmc(int reg) {
  uint32_t lo, hi;
  __asm__ __volatile__("rdpmc" : "=a"(lo), "=d"(hi) : "c"(reg));
  return ((uint64_t)lo) | (((uint64_t)hi) << 32);
  return 0;
}
inline void cpuid(uint32_t eax, uint32_t ecx, uint32_t *a, uint32_t *b,
                  uint32_t *c, uint32_t *d) {
  __asm__ __volatile__("cpuid"
                       : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                       : "a"(eax), "c"(ecx));
}

typedef struct {
  ns tsc;
  uint32_t wct;
  uint32_t cycles;
  uint32_t instructions;
} Duration;

class CycleCounter {
public:
  void tick() __attribute__((always_inline)) {
#if IS_INTEL
    cycles_low_ = ebbrt::profiler::rdpmc((reg+1))
#endif
    return;
  };
  void tock() __attribute__((always_inline)) {
#if IS_INTEL
    cycles_high_ = ebbrt::profiler::rdpmc((reg+1))
#endif
    return;
  };
  uint32_t get() __attribute__((always_inline)) { 
    
#if IS_INTEL
    return cycles_high_ - cycles_low_;
#else
    return 0;
#endif
  };

private:
  uint32_t cycles_low_;
  uint32_t cycles_high_;
};

class InstructionCounter {
public:
  void tick() __attribute__((always_inline)) {
#if IS_INTEL
    inst_low_ = ebbrt::profiler::rdpmc((reg+1))
#endif
    return;
  };
  void tock() __attribute__((always_inline)) {
#if IS_INTEL
    inst_high_ = ebbrt::profiler::rdpmc((reg+1))
#endif
    return;
  };
  uint32_t get() __attribute__((always_inline)) { 
    
#if IS_INTEL
    return inst_high_ - inst_low_;
#else
    return 0;
#endif
  };
private:
  uint32_t inst_low_;
  uint32_t inst_high_;
};

// on "print" get the following:
// time duration
class Profiler {
public:
  void tick() __attribute__((always_inline)) {
    cc_.tick();
    ic_.tick();
    hrt_.tick();
    return;
  };
  void tock() __attribute__((always_inline)) {
    time_ = hrt_.tock();
    cc_.tock();
    ic_.tock();
    return;
  };
  Duration get() __attribute__((always_inline)) { 
  Duration d;
    d.tsc = time_;
    d.wct = 0;
    d.instructions = ic_.get();
    d.cycles = cc_.get();
    return d;
  }

private:
  ebbrt::clock::HighResTimer hrt_;
  ns time_;
  CycleCounter cc_;
  InstructionCounter ic_;
};
}
}
