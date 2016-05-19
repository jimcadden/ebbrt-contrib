//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef APPS_SOCKET_BAREMETAL_SRC_SOCKETMANAGER_H_
#define APPS_SOCKET_BAREMETAL_SRC_SOCKETMANAGER_H_

#include "../../src/StaticEbbIds.h"
#include "Vfs.h"
#include <ebbrt/CacheAligned.h>
#include <ebbrt/EbbAllocator.h>
#include <ebbrt/LocalIdMap.h>
#include <ebbrt/StaticSharedEbb.h>

namespace ebbrt {

class SocketManager : public ebbrt::StaticSharedEbb<SocketManager>,
                      public CacheAligned {
public:
  class SocketFd : public Vfs::Fd {
  public:
    SocketFd(){};
    static EbbRef<SocketFd> Create(EbbId id = ebb_allocator->Allocate()) {
      auto root = new SocketFd::Root();
      local_id_map->Insert(
          std::make_pair(id, static_cast<Vfs::Fd::Root *>(root)));
      return EbbRef<SocketFd>(id);
    }
    static SocketFd &HandleFault(EbbId id) {
      return static_cast<SocketFd &>(Vfs::Fd::HandleFault(id));
    }
    class Root : public Vfs::Fd::Root {
    public:
      Vfs::Fd &HandleFault(EbbId id) override {
        auto rep = new SocketFd();
        // Cache the reference to the rep in the local translation table
        EbbRef<SocketFd>::CacheRef(id, *rep);
        return *rep;
      }
    };
    // void Close() override { kprintf("called SocketFd::Close()\n"); }
    // void IsReady() override { kprintf("called SocketFd::IsReady()\n"); };
    // void Lseek() override { kprintf("called SocketFd::Lseek()\n"); };
    void Read() override { kprintf("called SocketFd::Read()\n"); }
    // void Write() override { kprintf("called SocketFd::Write()\n"); }
  };
  explicit SocketManager(){};
  int NewIpv4Socket();
};

constexpr auto socket_manager = EbbRef<SocketManager>(kSocketManagerEbbId);

} // namespace ebbrt
#endif // APPS_SOCKET_BAREMETAL_SRC_SOCKETMANAGER_H_
