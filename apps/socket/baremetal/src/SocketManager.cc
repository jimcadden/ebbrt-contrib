//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include "SocketManager.h"
#include <ebbrt/Timer.h>

int ebbrt::SocketManager::NewIpv4Socket() {
  auto sfd = ebbrt::SocketManager::SocketFd::Create();
  return ebbrt::root_vfs->RegisterFd(sfd);
}

void ebbrt::SocketManager::SocketFd::TcpSession::Receive(
    std::unique_ptr<ebbrt::MutIOBuf> b) {
  {
    ebbrt::kprintf("received data");
    std::lock_guard<ebbrt::SpinLock> guard(buf_lock_);
    if (inbuf_) {
      inbuf_->PrependChain(std::move(b));
    } else {
      inbuf_ = std::move(b);
    }
  } // unlocked
  check_read();
  return;
}

void ebbrt::SocketManager::SocketFd::TcpSession::check_read() {

  std::lock_guard<ebbrt::SpinLock> guard(buf_lock_);
  if (!inbuf_ || !read_.second || read_.first.GetFuture().Ready()) {
    return;
  }
    ebbrt::kprintf("requesting %d bytes\n", read_.second);

  std::unique_ptr<MutIOBuf> rbuf(nullptr);
  auto len = read_.second;
  auto chain_len = inbuf_->ComputeChainDataLength();

  if (len >= chain_len) {
    read_.first.SetValue(std::move(inbuf_));
  } else {
    // we have more data then whats being requested
    ebbrt::kabort("split buffer not yet supported");
  }
  return;
}

void ebbrt::SocketManager::SocketFd::TcpSession::Close() {
  Shutdown();
  return;
}

void ebbrt::SocketManager::SocketFd::TcpSession::Abort() {
  Shutdown();
  return;
}

ebbrt::Future<std::unique_ptr<ebbrt::IOBuf>>
ebbrt::SocketManager::SocketFd::Read(size_t len) {

  // std::lock_guard<ebbrt::SpinLock> guard(tcpbuf_lock_);
  // check state
  // TODO: locking out concurrent receives
  Promise<std::unique_ptr<ebbrt::IOBuf>> p;
  auto f = p.GetFuture();

  tcp_session_->read_ = std::make_pair(std::move(p), len);
  tcp_session_->check_read();
  return std::move(f);
}

void ebbrt::SocketManager::SocketFd::TcpSession::Fire() {
  // TODO: check state
  // if connection timeout
  auto state = connected_.GetFuture();
  if (!state.Ready()) {
    connected_.SetValue(false);
  }
}

ebbrt::Future<bool>
ebbrt::SocketManager::SocketFd::Connect(ebbrt::NetworkManager::TcpPcb pcb) {
  // TODO: check state
  tcp_session_ = new TcpSession(this, std::move(pcb));
  tcp_session_->Install();
  // XXX: is timeout nessessary, does't TCP close or abort?
  timer->Start(*tcp_session_, std::chrono::seconds(5), /* repeat */ false);
  return tcp_session_->connected_.GetFuture();
}

void ebbrt::SocketManager::SocketFd::TcpSession::Connected() {
  connected_.SetValue(true);
  return;
}
