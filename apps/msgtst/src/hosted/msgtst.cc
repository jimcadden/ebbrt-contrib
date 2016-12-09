//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <signal.h>

#include <boost/filesystem.hpp>

#include <ebbrt/GlobalIdMap.h>
#include <ebbrt/Runtime.h>
#include <ebbrt/StaticIds.h>
#include <ebbrt/Future.h>
#include <ebbrt/hosted/Context.h>
#include <ebbrt/hosted/ContextActivation.h>
#include <ebbrt/hosted/NodeAllocator.h>

#include "../MsgTst.h"

using namespace ebbrt;

int main(int argc, char** argv) {
  auto bindir = boost::filesystem::system_complete(argv[0]).parent_path() /
                "/bm/msgtst.elf32";

  ebbrt::Runtime runtime;
  ebbrt::Context c(runtime);
  boost::asio::signal_set sig(c.io_service_, SIGINT);
  {
    ebbrt::ContextActivation activation(c);

    // ensure clean quit on ctrl-c
    sig.async_wait([&c](const boost::system::error_code& ec,
                        int signal_number) { c.io_service_.stop(); });
    auto msgtst_ebb = MsgTst::Create();
    auto node_desc = node_allocator->AllocateNode(bindir.string(), 2, 2, 2);
    node_desc.NetworkId().Then([msgtst_ebb](Future<Messenger::NetworkId> f) {
      auto nid = f.Get();
      std::cout << "Begin Messenger Tests" << std::endl;

      std::cout << "1. single small message (4 bytes)... " << std::endl;
      // block on first msg is a require work-around to ensure the initial then
      // has been completed.
      msgtst_ebb->SendMessages(nid, 1, 4)[0].Block();
      std::cout << "success" << std::endl;

      std::cout << "2. many small message (1000 x 4 bytes)... " << std::endl;
      auto v2 = msgtst_ebb->SendMessages(nid, 1000, 4);
      when_all(v2).Block().Then([](Future<std::vector<uint32_t> > vf) {
        std::cout << "success!" << std::endl;
      });

      std::cout << "3. single medium message (1 x  4000 bytes)... "<< std::endl;
      auto v3 = msgtst_ebb->SendMessages(nid, 1, 4000);
      when_all(v3).Block().Then([](Future<std::vector<uint32_t> > vf) {
        std::cout << "success!"<< std::endl;
      });

      std::cout << "4. many medium message (10000 x  4000 bytes)... "<< std::endl;
      auto v4 = msgtst_ebb->SendMessages(nid, 10000, 4000);
      when_all(v4).Block().Then([](Future<std::vector<uint32_t> > vf) {
        std::cout << "success!"<< std::endl;
      });

      std::cout << "5. one large message (1 x  32000000 bytes)... "<< std::endl;
      auto v5 = msgtst_ebb->SendMessages(nid, 1, 32000000);
      when_all(v5).Block().Then([](Future<std::vector<uint32_t> > vf) {
        std::cout << "success!"<< std::endl;
      });
      std::cout << "All tests finished. Ctl+c to exit." << std::endl;
      return;
    });
  }
  c.Run();

  return 0;
}
