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

void ebbrt::SocketManager::SocketFd::TcpSession::Fire() {
  // TODO: check state
  auto state = connected_.GetFuture();
  if (!state.Ready()) {
    connected_.SetValue(-1);
  }
}

void ebbrt::SocketManager::SocketFd::TcpSession::Connected() {
  connected_.SetValue(true);
  return;
}


void ebbrt::SocketManager::SocketFd::TcpSession::check_read() {

  std::lock_guard<ebbrt::SpinLock> guard(buf_lock_);
  if (!inbuf_ || !read_.second || read_.first.GetFuture().Ready()) {
    return;
  }

  std::unique_ptr<MutIOBuf> rbuf(nullptr);
  auto len = read_.second;
  auto chain_len = inbuf_->ComputeChainDataLength();

  if (len >= chain_len) {
    read_.first.SetValue(std::move(inbuf_));
  } else {
    // we have more data then whats being requested
    ebbrt::kabort("splitting buffer not yet supported");
  }
  return;
}

void
ebbrt::SocketManager::SocketFd::TcpSession::Close() {
    read_.first.SetValue(ebbrt::MakeUniqueIOBuf(0));
    Shutdown();
    disconnected_.SetValue(0);
  return;
}

void ebbrt::SocketManager::SocketFd::TcpSession::Abort() {
  ebbrt::kabort("TCP Session Abort.\n");
  return;
}

ebbrt::Future<std::unique_ptr<ebbrt::IOBuf>>
ebbrt::SocketManager::SocketFd::Read(size_t len) {

  // TODO: locking out concurrent receives
  // std::lock_guard<ebbrt::SpinLock> guard(tcpbuf_lock_);
  Promise<std::unique_ptr<ebbrt::IOBuf>> p;
  auto f = p.GetFuture();

  tcp_session_->read_ = std::make_pair(std::move(p), len);
  tcp_session_->check_read();
  return std::move(f);
}

ebbrt::Future<uint8_t> 
ebbrt::SocketManager::SocketFd::Close(){
  tcp_session_->Shutdown();
  return tcp_session_->disconnected_.GetFuture();
}

ebbrt::Future<uint8_t>
ebbrt::SocketManager::SocketFd::Connect(ebbrt::NetworkManager::TcpPcb pcb) {
  // TODO: check state
  tcp_session_ = new TcpSession(this, std::move(pcb));
  tcp_session_->Install();
  // XXX: is timeout nessessary, does't TCP close or abort?
  timer->Start(*tcp_session_, std::chrono::seconds(5), /* repeat */ false);
  return tcp_session_->connected_.GetFuture();
}

