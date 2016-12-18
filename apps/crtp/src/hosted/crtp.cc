//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <signal.h>

#include <boost/filesystem.hpp>

#include <ebbrt/EbbId.h>
#include <ebbrt/GlobalIdMap.h>
#include <ebbrt/Runtime.h>
#include <ebbrt/StaticIds.h>
#include <ebbrt/hosted/Context.h>
#include <ebbrt/hosted/ContextActivation.h>
#include <ebbrt/hosted/NodeAllocator.h>

#include "Printer.h"
#include "../CRTP.h"
#include "../Factory.h"

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
struct Ebby : Ebb<Ebby, Ebby1>
{
  Ebby(EbbId id) : Ebb(id) { 
    std::cout << "tail["<< id_ << "]" << std::endl;
  }
  void foo(int x){
    std::cout << "Ebby foo! #" << id_ << "~" << x << std::endl;
  }
};

int main(int argc, char** argv) {

  ebbrt::Runtime runtime;
  ebbrt::Context c(runtime);
  boost::asio::signal_set sig(c.io_service_, SIGINT);
  {
    ebbrt::ContextActivation activation(c);

    // ensure clean quit on ctrl-c
    sig.async_wait([&c](const boost::system::error_code& ec,
                        int signal_number) { c.io_service_.stop(); });
  std::cout << "Start" << std::endl;

  auto e = ebbrt::EbbRef<Ebby>(4);
  e->foo(2);

  auto ee = ebbrt::EbbRef<Ebbo>(44);
  ee->bar(3);

  e->foo(2);
  std::cout << "End" << std::endl;
  }
  c.Run();

  return 0;
}
