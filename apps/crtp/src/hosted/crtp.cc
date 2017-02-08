//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <signal.h>

#include <boost/filesystem.hpp>

#include <ebbrt/hosted/Context.h>
#include <ebbrt/hosted/ContextActivation.h>
#include <ebbrt/GlobalIdMap.h>
#include <ebbrt/StaticIds.h>
#include <ebbrt/hosted/NodeAllocator.h>
#include <ebbrt/Runtime.h>

#include "../CRTP.h"
#include "../Counter.h"
#include "Printer.h"

using namespace ebbrt;

class Ebby;
class Ebby1;
class Ebby2;
class Ebby3;

// STAND ALONE EBBs
struct Ebbo : Ebb<Ebbo> {
  using Ebb<Ebbo>::Ebb;
  void bar(int x){
    std::cout << "Ebbo bar! #" << id_ << "~" << x << std::endl;
  }
};

struct Ebbx : SharedLocalEbb<Ebbx> {
  using SharedLocalEbb<Ebbx>::SharedLocalEbb;
  void bar(int x){
    std::cout << "YES EbbX bar! #" << id_ << "~" << x << std::endl;
  }
};

// EBB STACK
// LAYER 3
struct Ebby3 : EbbShard<Ebby3, Ebby2> {
  Ebby3(EbbId id) : EbbShard(id) { 
    std::cout << "top["<< id_ << "]-" ; 
  }
};

// LAYER 2
struct Ebby2 : EbbShard<Ebby2, Ebby1, Ebby3> {
  Ebby2(EbbId id) : EbbShard(id) { 
    std::cout << "head["<< id_ << "]-" ; 
  }
};
// LAYER 1
struct Ebby1 : EbbShard<Ebby1, Ebby, Ebby2> {
  Ebby1(EbbId id) : EbbShard(id) { 
    std::cout << "shard["<< id_ << "]-" ; 
  }
};

// LAYER 0
struct Ebby : SharedLocalEbb<Ebby, Ebby1>
{
  Ebby(EbbId id) : SharedLocalEbb(id) { 
    std::cout << "base["<< id_ << "]" << std::endl;
  }
  void foo(int x){
    std::cout << "Ebby foo! #" << id_ << "~" << x << std::endl;
  }
};

int main(int argc, char **argv) {

  auto bindir = boost::filesystem::system_complete(argv[0]).parent_path() /
                "/bm/crtp.elf32";

  ebbrt::Runtime runtime;
  ebbrt::Context c(runtime);
  boost::asio::signal_set sig(c.io_service_, SIGINT);
  {
    ebbrt::ContextActivation activation(c);

    std::cout << "Start" << std::endl;

    // ensure clean quit on ctrl-c
    sig.async_wait([&c](const boost::system::error_code& ec,
                        int signal_number) { c.io_service_.stop(); });
    Printer::Init().Then([bindir](ebbrt::Future<void> f) {
      f.Get();
      ebbrt::node_allocator->AllocateNode(bindir.string());
    //auto cnt = ebbrt::EbbRef<Counter>(4);
    //cnt->Up();
    //std::cout << "Sum: " << cnt->Get() << std::endl;

    // auto e = ebbrt::EbbRef<Ebby>(43);
    // e->foo(2);

    // auto ee = ebbrt::EbbRef<Ebbo>(44);
    // ee->bar(3);

    // auto eee = ebbrt::EbbRef<Ebbx>(400);
    // eee->bar(4);

    // e->foo(2);
    // std::cout << "End" << std::endl;
    });
  }
  c.Run();

  return 0;
}
