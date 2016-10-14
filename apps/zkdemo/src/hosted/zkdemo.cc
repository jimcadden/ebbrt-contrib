//          Copyright Boston University SESA Group 2013 - 2016.
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

#include "Printer.h"
#include <ebbrt-zookeeper/ZooKeeper.h>

using namespace std;
ebbrt::Messenger::NetworkId net_id;

class PrinterWatcher : public ebbrt::ZooKeeper::Watcher {
public:
  void OnConnected() override { printf("watch alert: Session Connected.\n"); }
  void OnConnecting() override { printf("watch alert: Session Conneting.\n"); }
  void OnSessionExpired() override {
    printf("watch alert: Session Expired.\n");
  }
  void OnCreated(const char *path) override {
    printf("watch alert: Created!\n");
  }
  void OnDeleted(const char *path) override {
    printf("watch alert: Deleted !\n");
  }
  void OnChanged(const char *path) override {
    printf("watch alert: Changed: !\n");
  }
  void OnChildChanged(const char *path) override {
    printf("watch alert: Child Changed.\n");
  }
  void OnNotWatching(const char *path) override {
    printf("watch alert: Not Wathcing.\n");
  }
};

int main(int argc, char **argv) {
  auto bindir = boost::filesystem::system_complete(argv[0]).parent_path() /
                "/bm/zk.elf32";

  ebbrt::Runtime runtime;
  ebbrt::Context c(runtime);
  boost::asio::signal_set sig(c.io_service_, SIGINT);
  {
    ebbrt::ContextActivation activation(c);

    // ensure clean quit on ctrl-c
    sig.async_wait([&c](const boost::system::error_code &ec,
                        int signal_number) { c.io_service_.stop(); });

    // begin EbbRT context
    ebbrt::event_manager->Spawn([&]() {

      Printer::Init().Then([bindir](ebbrt::Future<void> f) {
        f.Get();
        cout << "My ip is: " << ebbrt::messenger->LocalNetworkId().ToString()
             << std::endl;
        auto secret = "Hazer Baba";
        auto *mw = new PrinterWatcher();
        ebbrt::EbbRef<ebbrt::ZooKeeper> zk = ebbrt::ZooKeeper::Create(
            ebbrt::ebb_allocator->Allocate(), mw);
        zk->New("/secret").Block();
        zk->Set("/secret", secret).Block();
        // END ZOOKEEPER
        auto ns = ebbrt::node_allocator->AllocateNode(bindir.string());
        ns.NetworkId().Then(
            [](ebbrt::Future<ebbrt::Messenger::NetworkId> net_if) {
              net_id = net_if.Get();
            });
      });

    });
  }
  c.Run();

  return 0;
}
