//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <signal.h>

#include <boost/filesystem.hpp>

#include <ebbrt/Context.h>
#include <ebbrt/ContextActivation.h>
#include <ebbrt/GlobalIdMap.h>
#include <ebbrt/StaticIds.h>
#include <ebbrt/NodeAllocator.h>
#include <ebbrt/Runtime.h>

#include "../../src/MsgTst.h"

using namespace ebbrt;

int main(int argc, char **argv) {
  auto bindir = boost::filesystem::system_complete(argv[0]).parent_path() /
                "/bm/msgtst.elf32";

  Runtime runtime;
  Context c(runtime);

  boost::asio::signal_set sig(c.io_service_, SIGINT);
  {
    ContextActivation activation(c);

    // ensure clean quit on ctrl-c
    sig.async_wait([&c](const boost::system::error_code &ec,
                        int signal_number) { c.io_service_.stop(); });
    auto msgtst_ebb = MsgTst::Create();
    auto node_desc = node_allocator->AllocateNode(bindir.string());
    node_desc.NetworkId().Then([msgtst_ebb](Future<Messenger::NetworkId> f) {
      auto nid = f.Get();
      std::cout << "Sending initial messages \n";
      auto v1 = msgtst_ebb->SendMessages(nid, 1, 1);
      v1[0].Block().Then([msgtst_ebb, nid](Future<uint32_t> f) {
         printf("woo! \n");
        });
      printf("blah \n");
      auto v2 = msgtst_ebb->SendMessages(nid, 2, 1000);
      std::cout << " received?\n";
      return;
    });
  }
  c.Run();
  c.Reset();

  std::cout << "tests finished\n";
  return 0;
}
