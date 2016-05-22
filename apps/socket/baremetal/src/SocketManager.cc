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
  kassert(b->Length() != 0);
  // restore any queued buffers
  std::lock_guard<ebbrt::SpinLock> guard(buf_lock_);
  //if (received_buf_) {

  if( in_.GetFuture().Ready()){
    EBBRT_UNIMPLEMENTED();
  }
  //  received_buf_->PrependChain(std::move(b));
  //} else {
    in_.SetValue(IOBuf::Create<MutSharedIOBufRef>(SharedIOBufRef::CloneView, std::move(b)));
}

std::unique_ptr<ebbrt::MutSharedIOBufRef>
ebbrt::SocketManager::SocketFd::Read(size_t message_len) {
  kprintf("called SocketFd::Read()\n");

  auto pbuf = tcp_session_->in_.GetFuture().Block();

  kprintf("Read data!\n");
  std::lock_guard<ebbrt::SpinLock> guard(tcp_session_->buf_lock_);
  // FIXME: spin lock
  std::unique_ptr<MutIOBuf> read_in = nullptr;
  auto buf = std::move(pbuf.Get());
  kassert(buf);
  
  auto chain_len = buf->ComputeChainDataLength();

  if (likely(chain_len <= message_len)) {
    // we have exactly the amount of data to read
  } else if (chain_len > message_len) {
    // we have additional data to read so we will need to split the
    // buffer and return only the amount we need
    EBBRT_UNIMPLEMENTED();
  }
  return std::move(buf);
}

void ebbrt::SocketManager::SocketFd::Fire() {
  // connection timeout
  auto state = connected_.GetFuture();
  if (!state.Ready()) {
    connected_.SetValue(false);
  }
}

ebbrt::Future<bool>
ebbrt::SocketManager::SocketFd::Connect(ebbrt::NetworkManager::TcpPcb pcb) {
  // TODO: prevent twice install
  tcp_session_ = new TcpSession(this, std::move(pcb));
  tcp_session_->Install();
  // connection timeout
  timer->Start(*this, std::chrono::seconds(16), /* repeat */ false);
  return connected_.GetFuture();
}

void ebbrt::SocketManager::SocketFd::Connected() {
        ebbrt::kprintf(" TCP connected.\n");
  connected_.SetValue(true);
  return;
}
