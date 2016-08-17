#include <ebbrt/Debug.h>
#include <ebbrt/EbbAllocator.h>
#include <ebbrt/Net.h>
#include <ebbrt/NetTcpHandler.h>
#include <ebbrt/SharedIOBufRef.h>
#include <ebbrt/UniqueIOBuf.h>

#define PORT 5201

class iPerf : public StaticSharedEbb<iPerf>, public CacheAligned {
public:
  iPerf();
  void Start(uint16_t port){
    listening_pcb_.Bind(port, [this](NetworkManager::TcpPcb pcb) {
      // new connection callback
      static std::atomic<size_t> cpu_index{0};
      auto index = cpu_index.fetch_add(1) % ebbrt::Cpu::Count();
      pcb.BindCpu(index);
      auto connection = new TcpSession(this, std::move(pcb));
      connection->Install();
    });
  }
  
private:
  class TcpSession : public ebbrt::TcpHandler {
  public:
    TcpSession(iPerf *iperf, ebbrt::NetworkManager::TcpPcb pcb)
      : ebbrt::TcpHandler(std::move(pcb)), mcd_(mcd) {}
    void Close(){}
    void Abort(){}
    void Receive(std::unique_ptr<MutIOBuf> b){
      b.release();
      return;
    }
  private:
    iPerf *iperf;
  };
  NetworkManager::ListeningTcpPcb listening_pcb_;
};


void AppMain()
{
  auto mc = ebbrt::EbbRef<ebbrt::Memcached>(ebb_allocator->AllocateLocal());
  mc->Start(PORT);
  ebbrt::kprintf("iPerf server listening on port %d\n", MCDPORT);
}

