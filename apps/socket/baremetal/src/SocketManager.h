//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef APPS_SOCKET_BAREMETAL_SRC_SOCKETMANAGER_H_
#define APPS_SOCKET_BAREMETAL_SRC_SOCKETMANAGER_H_

#include <ebbrt/CacheAligned.h>
#include <ebbrt/StaticSharedEbb.h>
#include "../../src/StaticEbbIds.h"

namespace ebbrt {
class SocketManager : public ebbrt::StaticSharedEbb<SocketManager>, public CacheAligned {
public:
  explicit SocketManager(){};
};

constexpr auto socket_manager = EbbRef<SocketManager>(kSocketManagerEbbId);

}  // namespace ebbrt
#endif  // APPS_SOCKET_BAREMETAL_SRC_SOCKETMANAGER_H_
