//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <signal.h>

#include <boost/filesystem.hpp>

#include <ebbrt/Cpu.h>
#include <ebbrt/Debug.h>
#include <ebbrt/SpinBarrier.h>
#include <ebbrt/GlobalIdMap.h>
#include <ebbrt/EbbRef.h>
#include <ebbrt/Runtime.h>
#include <ebbrt/StaticIds.h>
#include <ebbrt/hosted/Context.h>
#include <ebbrt/hosted/ContextActivation.h>
#include <ebbrt/hosted/NodeAllocator.h>

#include "../Counter.h"
#include "Printer.h"

int main(int argc, char** argv) {
  auto bindir = boost::filesystem::system_complete(argv[0]).parent_path() /
                "/bm/multicore.elf32";

  ebbrt::Runtime runtime;
  ebbrt::Context c(runtime);
  boost::asio::signal_set sig(c.io_service_, SIGINT);
  {
    ebbrt::ContextActivation activation(c);

    // ensure clean quit on ctrl-c
    sig.async_wait([&c](const boost::system::error_code& ec,
                        int signal_number) { c.io_service_.stop(); });


///    ebbrt::EbbRef<Counter> counter;
///    counter->Up();
///    ebbrt::kprintf("Sum: %d\n", counter->Get());



    Printer::Init().Then([bindir](ebbrt::Future<void> f) {
      f.Get();
      ebbrt::global_id_map->Set(ebbrt::GCounter::GlobalCounterId, "220").Then([bindir](auto f){
          ebbrt::node_allocator->AllocateNode(bindir.string());
        });
    });
  }
  c.Run();

  return 0;
}
