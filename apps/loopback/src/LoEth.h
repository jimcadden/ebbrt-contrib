//          Copyright Boston University SESA Group 2013 - 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ebbrt/MulticoreEbb.h> 
#include <ebbrt/native/Net.h> 

#ifndef EBBRT_LO_ETH
#define EBBRT_LO_ETH

namespace ebbrt {
class LoopbackRep;

class LoopbackDriver : public EthernetDevice {
public:
  LoopbackDriver();
  static void Create();
  void Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo) override;
  const EthernetAddress &GetMacAddress() override;

private:
  EbbRef<LoopbackRep> ebb_;
  EthernetAddress mac_addr_;
  NetworkManager::Interface &itf_;

  friend class LoopbackRep;
};

class LoopbackRep : public MulticoreEbb<LoopbackRep, LoopbackDriver> {
public:
  explicit LoopbackRep(const LoopbackDriver &root);
  void Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo);
  void Receive();
  void ReceivePoll();

private:
  const LoopbackDriver &root_;
};
} // namespace ebbrt
#endif //EBBRT_LO_ETH
