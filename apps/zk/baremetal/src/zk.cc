//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <chrono>
#include <cstdio>
//#include <ebbrt/Clock.h>
#include <ebbrt/Debug.h>
//#include <string.h>
//#include <thread>

#include "Zookeeper.h"


  ebbrt::Zookeeper *zk;

void AppMain() {

  zk = new ebbrt::Zookeeper("172.17.0.4:2181");
  ebbrt::kprintf("End of App.\n");
  return;
}
