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
      std::cout << "Begin Messenger Tests:\n";

      std::cout << "1. single small message (4 bytes)\n";
      // block on first msg is a require work-around to ensure the initial then
      // has been completed.
      msgtst_ebb->SendMessages(nid, 1, 4)[0].Block();
      std::cout << "success!\n";

      std::cout << "2. many small message (1000 x 4 bytes)\n";
      auto v2 = msgtst_ebb->SendMessages(nid, 1000, 4);
      when_all(v2).Block().Then([](Future<std::vector<uint32_t> > vf) {
        std::cout << "success!\n";
      });

      std::cout << "3. single medium message (1 x  4000 bytes)\n";
      auto v3 = msgtst_ebb->SendMessages(nid, 1, 4000);
      when_all(v3).Then([](Future<std::vector<uint32_t> > vf) {
        vf.Block();
        std::cout << "success!\n";
      });

      std::cout << "4. many medium message (100000 x  4000 bytes)\n";
      auto v4 = msgtst_ebb->SendMessages(nid, 100000, 4000);
      when_all(v4).Then([](Future<std::vector<uint32_t> > vf) {
        vf.Block();
        std::cout << "success!\n";
      });

      std::cout << "4. one large message (1 x  4000000 bytes)\n";
      auto v5 = msgtst_ebb->SendMessages(nid, 1, 4000000);
      when_all(v5).Then([](Future<std::vector<uint32_t> > vf) {
        vf.Block();
        std::cout << "success!\n";
      });
      std::cout << "\nAll tests finished. Ctl+c to exit. \n";
      return;
    });
  }
  c.Run();
  return 0;
}
