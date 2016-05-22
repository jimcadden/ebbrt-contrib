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
#include <ebbrt/EventManager.h>
#include <ebbrt/LocalIdMap.h>
#include <ebbrt/Net.h>
#include <ebbrt/NetTcpHandler.h>
#include <ebbrt/SharedIOBufRef.h>
#include <ebbrt/SpinLock.h>
#include <ebbrt/StaticSharedEbb.h>
#include <ebbrt/UniqueIOBuf.h>

namespace ebbrt {

class SocketManager : public ebbrt::StaticSharedEbb<SocketManager>,
                      public CacheAligned {
public:
  class SocketFd : public Vfs::Fd,
                       public ebbrt::Timer::Hook {
  public:
    class TcpSession : public ebbrt::TcpHandler {
    public:
      TcpSession(SocketFd *fd, ebbrt::NetworkManager::TcpPcb pcb)
          : ebbrt::TcpHandler(std::move(pcb)), fd_(fd) {}
      void Connected() override {
        fd_->Connected();
      }
      void Receive(std::unique_ptr<ebbrt::MutIOBuf> buf) override;

      void Close() override {
        Shutdown();
      }

      void Abort() override {}

    private:
      friend class SocketFd;
      SocketFd *fd_;
      ebbrt::NetworkManager::TcpPcb pcb_;
      //std::unique_ptr<ebbrt::MutIOBuf> received_buf_;
      Promise<std::unique_ptr<ebbrt::MutSharedIOBufRef>> in_;
      ebbrt::SpinLock buf_lock_;
    };

    void Fire() override;
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


    std::unique_ptr<ebbrt::MutSharedIOBufRef> Read(size_t len) override;
    ebbrt::Future<bool> Connect(ebbrt::NetworkManager::TcpPcb pcb);
    void Connected();

  private:
    TcpSession *tcp_session_;
      enum states {
        DISCONNECTED,
        CONNECTING,
        READY,
        READ_BOCKED,
      };
      enum states state_;
      Promise<bool> connected_;
  };
  explicit SocketManager(){};
  int NewIpv4Socket();
};

constexpr auto socket_manager = EbbRef<SocketManager>(kSocketManagerEbbId);

} // namespace ebbrt
#endif // APPS_SOCKET_BAREMETAL_SRC_SOCKETMANAGER_H_
