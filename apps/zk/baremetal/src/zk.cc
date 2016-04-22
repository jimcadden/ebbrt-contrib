//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "Printer.h"
#include <zookeeper.h>

void AppMain() { 
  
  printer->Print("ZK BACKEND UP.\n"); 

  zhandle_t* zoo_handle_ = nullptr;
  zoo_handle_ = zookeeper_init("localhost:2181",
                               nullptr,
                               5000,
                               nullptr, // client id
                               nullptr,
                               0);

  if( zoo_state(zoo_handle_) == ZOO_CONNECTED_STATE ){
      ebbrt::kprintf("We are connected");
  }
  return;
}
