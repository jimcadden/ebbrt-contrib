//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <signal.h>
#include <iostream>
#include <string>

#include <thread>
#include <boost/filesystem.hpp>

#include <ebbrt/Context.h>
#include <ebbrt/ContextActivation.h>
#include <ebbrt/GlobalIdMap.h>
#include <ebbrt/StaticIds.h>
#include <ebbrt/NodeAllocator.h>
#include <ebbrt/Runtime.h>

#include "Printer.h"

  ebbrt::Runtime runtime;
  ebbrt::Context c(runtime);

using namespace std;
ebbrt::Messenger::NetworkId net_id;

void cmdr () {
    ebbrt::ContextActivation activation(c);
        string str;
        while(str != "quit"){
          cout << "cmd:  ";
          getline (cin, str);
          printer->Print(net_id, str.c_str());
        }
}

int main(int argc, char** argv) {
  auto bindir = boost::filesystem::system_complete(argv[0]).parent_path() /
                "/bm/iocmd.elf32";

  boost::asio::signal_set sig(c.io_service_, SIGINT);
  {
    ebbrt::ContextActivation activation(c);

    // ensure clean quit on ctrl-c
    sig.async_wait([](const boost::system::error_code& ec,
                        int signal_number) { c.io_service_.stop(); });
    Printer::Init().Then([bindir](ebbrt::Future<void> f) {
      f.Get();
      auto ns = ebbrt::node_allocator->AllocateNode(bindir.string(), 1,1);
      ns.NetworkId().Then([](ebbrt::Future<ebbrt::Messenger::NetworkId> net_if){
        net_id = net_if.Get();
        std::thread ts(cmdr);
        ts.detach();
      }); });
  }
  c.Run();

  return 0;
}
