//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef APPS_HOSTEDTMR_BAREMETAL_SRC_H_
#define APPS_HOSTEDTMR_BAREMETAL_SRC_H_

#include <string>
#include <ebbrt/Timer.h>
#include <ebbrt/Debug.h>

class MyTimer : public ebbrt::Timer::Hook {
 public:
  explicit MyTimer(){
    ebbrt::kprintf("Starting timer%c\n"," ");
    ebbrt::timer->Start(*this, std::chrono::seconds(1), true);
  };
  void Foo() {
    ebbrt::kprintf("Hello World%c\n", ' ');
    return;
  }
  void Fire() override { 
    
    ebbrt::kprintf("Fire!%c\n", ' ');
    return; }
  //static Printer& HandleFault(ebbrt::EbbId id);

  //void Print(const char* string);
  //void ReceiveMessage(ebbrt::Messenger::NetworkId nid,
  //                    std::unique_ptr<ebbrt::IOBuf>&& buffer);
 private:
};

//constexpr auto printer = ebbrt::EbbRef<Printer>(kPrinterEbbId);

#endif  // APPS_HOSTEDTMR_BAREMETAL_SRC_H_
