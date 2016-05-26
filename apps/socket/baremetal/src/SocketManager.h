//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef APPS_SOCKET_BAREMETAL_SRC_SOCKETMANAGER_H_
#define APPS_SOCKET_BAREMETAL_SRC_SOCKETMANAGER_H_

#include <utility> // std::pair

#include "../../src/StaticEbbIds.h"
#include "Vfs.h"
#include <ebbrt/CacheAligned.h>
#include <ebbrt/EbbAllocator.h>
#include <ebbrt/EventManager.h>
#include <ebbrt/LocalIdMap.h>
#include <ebbrt/Future.h>
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
  class SocketFd : public Vfs::Fd {
  public:
    class TcpSession : public ebbrt::TcpHandler,
                       public ebbrt::Timer::Hook {
    public:
      void Fire() override;
      TcpSession(SocketFd *fd, ebbrt::NetworkManager::TcpPcb pcb)
          : ebbrt::TcpHandler(std::move(pcb)), fd_(fd) {}
      void Connected() override; 
      void Receive(std::unique_ptr<ebbrt::MutIOBuf> buf) override;
      void Close() override;
      void Abort() override;

    private:
      typedef std::pair<Promise<std::unique_ptr<ebbrt::IOBuf>>, size_t> PendingRead;
      friend class SocketFd;
      SocketFd *fd_;
      ebbrt::NetworkManager::TcpPcb pcb_;
      std::unique_ptr<ebbrt::MutIOBuf> inbuf_;
      ebbrt::SpinLock buf_lock_;
      PendingRead read_;
      Promise<uint8_t> disconnected_;
      Promise<uint8_t> connected_;
      void check_read();
    };

    SocketFd() : listen_port_(0) {};
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

    int Listen();
    int Bind(uint16_t port);
    ebbrt::Future<int> Accept();
    ebbrt::Future<uint8_t> Close() override;
    ebbrt::Future<uint8_t> Connect(ebbrt::NetworkManager::TcpPcb pcb);
    ebbrt::Future<std::unique_ptr<IOBuf>> Read(size_t len) override;
    void Write(std::unique_ptr<IOBuf> buf) override;

  private:
    void install_pcb(ebbrt::NetworkManager::TcpPcb pcb);
    void check_waiting();

    TcpSession *tcp_session_;
    ebbrt::NetworkManager::ListeningTcpPcb listening_pcb_;
    bool connected_;

    // listening tcp socket
    uint16_t listen_port_;
    std::queue<ebbrt::Promise<int>> waiting_accept_;
    std::queue<ebbrt::NetworkManager::TcpPcb> waiting_pcb_;
    ebbrt::SpinLock waiting_lock_;

  };
  explicit SocketManager(){};
  int NewIpv4Socket();
};

constexpr auto socket_manager = EbbRef<SocketManager>(kSocketManagerEbbId);

} // namespace ebbrt
#endif // APPS_SOCKET_BAREMETAL_SRC_SOCKETMANAGER_H_
