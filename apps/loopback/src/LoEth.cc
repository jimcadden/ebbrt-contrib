//          Copyright Boston University SESA Group 2013 - 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ebbrt/Debug.h> 
#include "LoEth.h"

ebbrt::LoopbackDriver::LoopbackDriver()
    : mac_addr_({{0x06, 0xfe, 0x22, 0x22, 0x22, 0x22}}),
      itf_(network_manager->NewLoopback(*this)) {

  std::array<uint8_t, 4> address = {{169, 254, 0, 1}};
  std::array<uint8_t, 4> netmask = {{255, 255, 0, 0}};
  std::array<uint8_t, 4> gateway = {{169, 254, 0, 1}};
  auto addr = new ebbrt::NetworkManager::Interface::ItfAddress();
  addr->address = address;
  addr->netmask = netmask;
  addr->gateway = gateway;
  itf_.SetAddress(
      std::unique_ptr<ebbrt::NetworkManager::Interface::ItfAddress>(addr));
}

void ebbrt::LoopbackDriver::Create() {
  auto lo_dev = new LoopbackDriver();
  lo_dev->ebb_ = LoopbackRep::Create(lo_dev, ebb_allocator->AllocateLocal());
}

void ebbrt::LoopbackDriver::Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo) {
  ebb_->Send(std::move(buf), std::move(pinfo));
}

const ebbrt::EthernetAddress& ebbrt::LoopbackDriver::GetMacAddress() {
 ebbrt::kprintf_force("Loopback Get Mac Addr\n"); 
  return mac_addr_;
}

ebbrt::LoopbackRep::LoopbackRep(const LoopbackDriver &root) : root_(root) {
 ebbrt::kprintf_force("Loopback Rep Constructor\n"); 
}

void ebbrt::LoopbackRep::ReceivePoll() {
  // Receive data
  // root_.itf_.Receive(std::move(b));
 ebbrt::kprintf_force("Loopback RECV.\n"); 
}

void ebbrt::LoopbackRep::Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo) {
  // Send data
 ebbrt::kprintf_force("Loopback SEND PACKET len=%d chain_len=%d\n", buf->ComputeChainDataLength(),buf->CountChainElements() ); 
  auto dp = buf->GetDataPointer();
  auto eh = dp.Get<ebbrt::EthernetHeader>();
  ebbrt::kprintf_force("Ethertype= 0x%x\n", ebbrt::ntohs(eh.type));
  auto ap = dp.Get<ebbrt::ArpPacket>();
  ebbrt::kprintf_force("arp hardware type 0x%x\n", ebbrt::ntohs(ap.htype));
  ebbrt::kprintf_force("arp proto type 0x%x\n", ebbrt::ntohs(ap.ptype));
  ebbrt::kprintf_force("arp opcode 0x%x\n", ebbrt::ntohs(ap.oper));

  ebbrt::kprintf_force("arp sender mac %x:%x:%x:%x:%x:%x\n", ap.sha[0], ap.sha[1],ap.sha[2],ap.sha[3],ap.sha[4],ap.sha[5]);
  ebbrt::kprintf_force("arp target mac %x:%x:%x:%x:%x:%x\n", ap.tha[0], ap.tha[1],ap.tha[2],ap.tha[3],ap.tha[4],ap.tha[5]);
  auto spa = ap.spa.toArray();
  ebbrt::kprintf_force("arp sender ip %hhd.%hhd.%hhd.%hhd \n", spa[0], spa[1],spa[2],spa[3]);
  spa = ap.tpa.toArray();
  ebbrt::kprintf_force("arp target ip %hhd.%hhd.%hhd.%hhd \n", spa[0], spa[1],spa[2],spa[3]);
}

