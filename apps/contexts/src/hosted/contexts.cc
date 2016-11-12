//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <signal.h>
#include <thread>

#include <boost/filesystem.hpp>

#include <ebbrt/GlobalIdMap.h>
#include <ebbrt/Runtime.h>
#include <ebbrt/StaticIds.h>
#include <ebbrt/hosted/Context.h>
#include <ebbrt/hosted/ContextActivation.h>
#include <ebbrt/hosted/NodeAllocator.h>

#include "Printer.h"

static const int num_cores = 3;
boost::filesystem::path bindir;
  ebbrt::Runtime runtime;

void start_context(){

  ebbrt::Context c(runtime);
  boost::asio::signal_set sig(c.io_service_, SIGINT);
  {
    ebbrt::ContextActivation activation(c);

    // ensure clean quit on ctrl-c
    sig.async_wait([&c](const boost::system::error_code& ec,
                        int signal_number) { c.io_service_.stop(); });
    ebbrt::event_manager->Spawn([]() {
        std::cout << "inside spawn exiting" << std::endl;
    });
  }
  c.Run();
        std::cout << "spawn exited" << std::endl;
  return;
}

int main(int argc, char** argv) {

  bindir = boost::filesystem::system_complete(argv[0]).parent_path() /
                "/bm/contexts.elf32";
  std::thread t[num_cores];
  for (int i = 0; i < num_cores; ++i) {
      t[i] = std::thread(start_context );
  }

  for (int i = 0; i < num_cores; ++i) {
      t[i].join();
  }
  return 0;
}
