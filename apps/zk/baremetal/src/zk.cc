//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ebbrt/Debug.h>
#include <ebbrt/EbbAllocator.h>

#include "Printer.h"
#include "ZooKeeper.h"


class PrinterWatcher : public ebbrt::ZooKeeper::Watcher {
  public:
    void OnConnected() override { printer->Print("watch alert: Session Connected.\n"); }
    void OnConnecting() override { printer->Print("watch alert: Session Conneting.\n"); }
    void OnSessionExpired() override { printer->Print("watch alert: Session Expired.\n"); }
    void OnCreated(const char* path) override { printer->Print("watch alert: Created!\n"); }
    void OnDeleted(const char* path) override { printer->Print("watch alert: Deleted !\n"); }
    void OnChanged(const char* path) override { printer->Print("watch alert: Changed: !\n"); }
    void OnChildChanged(const char* path) override { printer->Print("watch alert: Child Changed.\n"); }
    void OnNotWatching(const char* path) override { printer->Print("watch alert: Not Wathcing.\n"); }
};

auto *mw = new PrinterWatcher();
ebbrt::EbbRef<ebbrt::ZooKeeper> zk;

void AppMain() {
  zk = ebbrt::ZooKeeper::Create(ebbrt::ebb_allocator->Allocate(), static_cast<ebbrt::ZooKeeper::Watcher*>(mw));
//  auto val = zk->Get("/secret").Block().Get();
 // printf("The secret value is %s\n", val.value.c_str());
 printf("Backend up\n");
}
