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

void 
ebbrt::SocketManager::SocketFd::Write(std::unique_ptr<IOBuf> buf){
  tcp_session_->Send(std::move(buf));
}

ebbrt::Future<uint8_t> 
ebbrt::SocketManager::SocketFd::Close(){
  tcp_session_->Shutdown();
  return tcp_session_->disconnected_.GetFuture();
}

void
ebbrt::SocketManager::SocketFd::install_pcb(ebbrt::NetworkManager::TcpPcb pcb) {
  tcp_session_ = new TcpSession(this, std::move(pcb));
  tcp_session_->Install();
  return;
}

void
ebbrt::SocketManager::SocketFd::check_waiting(){
  std::lock_guard<ebbrt::SpinLock> guard(waiting_lock_);
  while( !waiting_accept_.empty() && !waiting_pcb_.empty() ){

    auto &a = waiting_accept_.front();
    auto pcb = std::move(waiting_pcb_.front());

    waiting_accept_.pop();
    waiting_pcb_.pop();

    auto fd = ebbrt::socket_manager->NewIpv4Socket();
    static_cast<ebbrt::EbbRef<ebbrt::SocketManager::SocketFd>>(fd)->Connect(std::move(pcb));
    a.SetValue(fd);
  }
}

ebbrt::Future<int> 
ebbrt::SocketManager::SocketFd::Accept(){
  ebbrt::Promise<int> p;
  auto f = p.GetFuture();
  waiting_accept_.push(std::move(p));
  check_waiting();
  return f;
}

int
ebbrt::SocketManager::SocketFd::Bind(uint16_t port) {
  // TODO: set state / error
  listen_port_ = port;
  return 0;
}

int
ebbrt::SocketManager::SocketFd::Listen() {
  // TODO: set state
  try {
    listening_pcb_.Bind(listen_port_, [this](ebbrt::NetworkManager::TcpPcb pcb) {
    ebbrt::kprintf("New connection arrived on listening socket.\n");
    waiting_pcb_.push(std::move(pcb));
    check_waiting();
    });
  } catch (std::exception& e) {
    // TODO: set errno
    ebbrt::kprintf("Unhandled exception caught: %s\n", e.what());
    return -1;
  } catch (...) {
    ebbrt::kprintf("Unhandled exception caught \n");
    return -1;
  }
  return 0;
}


ebbrt::Future<uint8_t>
ebbrt::SocketManager::SocketFd::Connect(ebbrt::NetworkManager::TcpPcb pcb) {
  // TODO: check state
  install_pcb(std::move(pcb));
  // timeout
  timer->Start(*tcp_session_, std::chrono::seconds(5), /* repeat */ false);
  return tcp_session_->connected_.GetFuture();
}

