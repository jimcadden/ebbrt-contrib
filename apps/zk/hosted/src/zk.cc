//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <thread>
#include <arpa/inet.h> 
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h> 
#include <unistd.h>


#include <boost/filesystem.hpp>

#include <ebbrt/Context.h>
#include <ebbrt/ContextActivation.h>
#include <ebbrt/GlobalIdMap.h>
#include <ebbrt/NodeAllocator.h>
#include <ebbrt/Runtime.h>
#include <ebbrt/StaticIds.h>

#include "Printer.h"
#include "ZooKeeper.h"

  ebbrt::Runtime runtime;
  ebbrt::Context c(runtime);

using namespace std;
ebbrt::Messenger::NetworkId net_id;

class PrinterWatcher : public ebbrt::ZooKeeper::Watcher {
  public:
    void OnConnected() override { printf("watch alert: Session Connected.\n"); }
    void OnConnecting() override { printf("watch alert: Session Conneting.\n"); }
    void OnSessionExpired() override { printf("watch alert: Session Expired.\n"); }
    void OnCreated(const char* path) override { printf("watch alert: Created!\n"); }
    void OnDeleted(const char* path) override { printf("watch alert: Deleted !\n"); }
    void OnChanged(const char* path) override { printf("watch alert: Changed: !\n"); }
    void OnChildChanged(const char* path) override { printf("watch alert: Child Changed.\n"); }
    void OnNotWatching(const char* path) override { printf("watch alert: Not Wathcing.\n"); }
};

void cmdr () {
        cout << "Enter a command for zookeeper:  " << endl;
    ebbrt::ContextActivation activation(c);
        string str;
        while(str != "quit"){
          cout << ">   ";
          getline (cin, str);
          printer->Print(net_id, str.c_str());
        }
}


int main(int argc, char **argv) {
  auto bindir = boost::filesystem::system_complete(argv[0]).parent_path() /
                "/bm/zk.elf32";

  boost::asio::signal_set sig(c.io_service_, SIGINT);
  {
    ebbrt::ContextActivation activation(c);


    // ensure clean quit on ctrl-c
    sig.async_wait([](const boost::system::error_code &ec,
                        int signal_number) { c.io_service_.stop(); });
    //Printer::Init().Then([bindir](ebbrt::Future<void> f) {
    //  f.Get();
    //  auto ns = ebbrt::node_allocator->AllocateNode(bindir.string(), 1,1);
    //  ns.NetworkId().Then([](ebbrt::Future<ebbrt::Messenger::NetworkId> net_if){
    //    net_id = net_if.Get();
    //    std::thread ts(cmdr);
    //    ts.detach();
    //  }); });


    auto *mw = new PrinterWatcher();
    ebbrt::EbbRef<ebbrt::ZooKeeper> zk =
      ebbrt::ZooKeeper::Create(ebbrt::ebb_allocator->Allocate(), "172.17.0.4:2181", mw);
    
        string str;
        while(str != "quit"){
          cout << ">   ";
          getline (cin, str);
          zk->CLI(const_cast<char*>(str.c_str()));
        }
  }
  c.Run();

  return 0;
}
