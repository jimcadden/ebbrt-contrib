//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef APPS_SOCKET_BAREMETAL_SRC_VFS_H_
#define APPS_SOCKET_BAREMETAL_SRC_VFS_H_

#include <ebbrt/CacheAligned.h>
#include <ebbrt/StaticSharedEbb.h>
#include "../../src/StaticEbbIds.h"

namespace ebbrt {
class Vfs : public ebbrt::StaticSharedEbb<Vfs>, public CacheAligned {
public:
  explicit Vfs(){};
};

constexpr auto vfs = EbbRef<SocketManager>(kVfsEbbId);

}  // namespace ebbrt
#endif  // APPS_SOCKET_BAREMETAL_SRC_VFS_H_
