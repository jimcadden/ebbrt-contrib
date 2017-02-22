//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ebbrt/Debug.h>
#include "../ZKGlobalIdMap.h"

void AppMain() { 
  
  ebbrt::kprintf("WEAKSYM BACKEND UP.\n"); 
  ebbrt::global_id_map->Init().Then([](auto connected) {

    ebbrt::kbugon(connected.Get() == false);
    ebbrt::kprintf("getting secret\n");
    auto val = ebbrt::global_id_map->Get(42).Block().Get();
    ebbrt::kprintf("The secret value is %s\n", val.c_str());

    ebbrt::global_id_map->SetWatcher(
        42, new ebbrt::ZKGlobalIdMap::WatchEvent(ZOO_CHANGED_EVENT, [val]() {
          auto val = ebbrt::global_id_map->Get(42).Block().Get();
          ebbrt::kprintf("The new secret value is %s\n", val.c_str());
          ebbrt::kprintf("setting new secrets\n");
          ebbrt::global_id_map->Set(42, "Abc").Block();
          ebbrt::global_id_map->Set(42, "123").Block();
          ebbrt::global_id_map->Set(42, "you and me").Block();
          ebbrt::kprintf("done\n");
        }));
    ebbrt::kprintf("setting new secret\n");
    ebbrt::global_id_map->Set(42, "abab rezah").Block();
  });

}
