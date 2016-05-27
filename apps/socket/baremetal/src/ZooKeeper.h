//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef APPS_SOCKET_BAREMETAL_SRC_ZOOKEEPER_H_
#define APPS_SOCKET_BAREMETAL_SRC_ZOOKEEPER_H_


#include <cstdio>
#include <string>
#include <ebbrt/Debug.h>
#include <ebbrt/SharedIOBufRef.h>
#include <ebbrt/SharedEbb.h>
#include <ebbrt/Timer.h>
#include <ebbrt/SpinLock.h>

#include "zk_config.h"
#include <zookeeper.h>

namespace ebbrt {

class ZooWatcher {
public:
  virtual ~ZooWatcher() {}

  virtual void OnConnected() = 0;
  virtual void OnConnecting() = 0;
  virtual void OnSessionExpired() = 0;

  virtual void OnCreated(const char* path) = 0;
  virtual void OnDeleted(const char* path) = 0;
  virtual void OnChanged(const char* path) = 0;
  virtual void OnChildChanged(const char* path) = 0;
  virtual void OnNotWatching(const char* path) = 0;
};

typedef Stat NodeStat;

class ZooKeeper : public ebbrt::Timer::Hook {
public:
  void Fire() override;
  ZooKeeper(const std::string& server_hosts,
            void* global_watcher = nullptr,
            int timeout_ms = 5 * 1000);
  ~ZooKeeper();
  // disable copy
  ZooKeeper(const ZooKeeper&) = delete;
  ZooKeeper& operator=(const ZooKeeper&) = delete;

  bool is_connected();
  bool is_expired();

  //bool Exists(const std::string& path, bool watch = false, NodeStat* = nullptr);

  //NodeStat Stat(const std::string& path);

  //std::string Create(const std::string& path,
  //                   const std::string& value = std::string(),
  //                   int flag = 0);

  //std::string CreateIfNotExists(const std::string& path,
  //                              const std::string& value = std::string(),
  //                              int flag = 0);

  //void Delete(const std::string& path);

  //void DeleteIfExists(const std::string& path);

  //void Set(const std::string&path, const std::string& value);

  //std::string Get(const std::string& path, bool watch = false);

  //std::vector<std::string> GetChildren(const std::string& parent_path, bool watch = false);

private:
  zhandle_t* zk_ = nullptr;
  SpinLock lock_;

  //ZooWatcher* global_watcher_ = nullptr;

  //void WatchHandler(int type, int state, const char* path);

  //static void GlobalWatchFunc(zhandle_t*, int type, int state,
 //                             const char* path, void* ctx);
};

}

#endif
