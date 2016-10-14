//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "Printer.h"

#include <ebbrt-zookeeper/ZooKeeper.h>

ebbrt::EbbRef<ebbrt::ZooKeeper> zk;

void AppMain() { printer->Print("ZKDEMO BACKEND UP.\n"); }
