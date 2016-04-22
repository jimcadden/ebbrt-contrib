//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <chrono>
#include <cstdio>
#include <ebbrt/Clock.h>
#include <string.h>
#include <thread>

#include "Printer.h"
#include "zookeeper.hpp"

void AppMain() {

  printer->Print("ZK BACKEND UP.\n");
  ebbrt::kprintf("Connecting\n");

  zookeeper::ZooKeeper zk("localhost:2181");
  auto t1 = ebbrt::clock::Wall::Now();
  while ((ebbrt::clock::Wall::Now() - t1) < std::chrono::seconds(1)) {
  }

  if (zk.is_connected()) {
    ebbrt::kprintf("We are connected\n");
  } else {
    ebbrt::kprintf("Connection failed\n");
  }
  return;
}
