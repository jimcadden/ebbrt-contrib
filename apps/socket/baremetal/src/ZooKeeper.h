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
#include <ebbrt/Future.h>
#include <ebbrt/SharedEbb.h>
#include <ebbrt/SharedIOBufRef.h>
#include <ebbrt/SpinLock.h>
#include <ebbrt/Timer.h>

#include "zk_config.h"
#include <zookeeper.h>

namespace ebbrt {


typedef Stat NodeStat;

class ZooKeeper : public ebbrt::Timer::Hook {
public:
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
  void Fire() override;
  ZooKeeper(const std::string& server_hosts,
            ZooWatcher* global_watcher = nullptr,
            int timeout_ms = 30 * 1000);
  ~ZooKeeper();
  // disable copy
  ZooKeeper(const ZooKeeper&) = delete;
  ZooKeeper& operator=(const ZooKeeper&) = delete;

  void Input(char *line);
  bool is_connected();
  bool is_expired();
  static void dumpstat(const struct Stat *stat);
  static void my_data_completion(int rc, const char *value, int value_len, const struct Stat *stat, const void *data);
  static void my_silent_data_completion(int rc, const char *value, int value_len, const struct Stat *stat, const void *data);
  static void my_stat_completion(int rc, const struct Stat *stat, const void *data);
  static void my_string_completion(int rc, const char *name, const void *data);
  static void my_string_completion_free_data(int rc, const char *name, const void *data);
  static void my_strings_completion(int rc, const struct String_vector *strings, const void *data);
  static void my_strings_stat_completion(int rc, const struct String_vector *strings, const struct Stat *stat, const void *data);
  static void my_void_completion(int rc, const void *data);
  static void my_silent_stat_completion(int rc, const struct Stat *stat, const void *data);

  Future<NodeStat> Stat(const std::string &path);
  Future<std::string> Create(const std::string &path,
                             const std::string &value = std::string(),
                             int flag = 0);
  Future<std::string>
  CreateIfNotExists(const std::string &path,
                    const std::string &value = std::string(), int flag = 0);
  Future<void> Delete(const std::string &path);
  Future<void> DeleteIfExists(const std::string &path);
  Future<void> Set(const std::string &path, const std::string &value);
  Future<std::string> Get(const std::string &path, bool watch = false);
  Future<std::vector<std::string>> GetChildren(const std::string &parent_path,
                                               bool watch = false);

  void WatchHandler(int type, int state, const char* path);

private:
  zhandle_t* zk_ = nullptr;
  SpinLock lock_;
  int verbose = 0;
  ZooWatcher* global_watcher_ = nullptr;

  int  startsWith(const char *line, const char *prefix);
  static void GlobalWatchFunc(zhandle_t*, int type, int state,
                              const char* path, void* ctx);
};

}

#endif
