//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <array>

#include <ebbrt/Debug.h> 
#include <ebbrt/UniqueIOBuf.h> 
#include <ebbrt/native/Net.h> 
#include <ebbrt/native/NetIpAddress.h> 

#include "LoEth.h"

typedef struct {
  uint32_t magic = 0xd00df00d;
  uint32_t key;
} test_response;

void AppMain() { ebbrt::kprintf_force("LOOPBACK BACKEND UP.\n");
  ebbrt::LoopbackDriver::Create();
  
  auto udp_pcb = new ebbrt::NetworkManager::UdpPcb();
  udp_pcb->Bind(80);

  std::array<uint8_t, 4> foo = {{169, 254, 1, 0}};
  ebbrt::Ipv4Address lo_dest(foo);

  auto buf = ebbrt::MakeUniqueIOBuf(sizeof(test_response), true);
  auto res = reinterpret_cast<test_response *>(buf->MutData());
  res->key = 69;
ebbrt::kprintf_force("Sending...\n");
  udp_pcb->SendTo(lo_dest, 80, std::move(buf));
ebbrt::kprintf_force("Finished Send.\n");
}
