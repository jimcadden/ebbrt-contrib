//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
//
#include <ebbrt/Timer.h>
#include "ZooKeeper.h"
#include <zookeeper.h>
#include <poll.h>

void ebbrt::ZooKeeper::GlobalWatchFunc(zhandle_t* h, int type, int state, const char* path, void* ctx) {
  auto self = static_cast<ZooKeeper*>(ctx);
  self->WatchHandler(type, state, path);
}

ebbrt::ZooKeeper::ZooKeeper(const std::string &server_hosts, ebbrt::ZooKeeper::ZooWatcher* global_watcher, int timeout_ms) : global_watcher_(global_watcher) {

  // create zk object
  zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
  zoo_deterministic_conn_order(1); // enable deterministic order
  zk_ = zookeeper_init(server_hosts.c_str(),
                               GlobalWatchFunc,
                               timeout_ms,
                               nullptr, // client id
                               this, // overload the 
                               0);
  timer->Start(*this, std::chrono::milliseconds(750), true);
  return;
}

ebbrt::ZooKeeper::~ZooKeeper() {
  if (zk_) {
    auto ret = zookeeper_close(zk_);
    if (ret != ZOK) {
    }
  }
}

/*
void *do_io(void *v)
{
    zhandle_t *zh = (zhandle_t*)v;
    struct pollfd fds[2];
    struct adaptor_threads *adaptor_threads = zh->adaptor_priv;

    api_prolog(zh);
    notify_thread_ready(zh);
    LOG_DEBUG(("started IO thread"));
    fds[0].fd=adaptor_threads->self_pipe[0];
    fds[0].events=POLLIN;
    while(!zh->close_requested) {
        struct timeval tv;
        int fd;
        int interest;
        int timeout;
        int maxfd=1;
        int rc;
        
        zookeeper_interest(zh, &fd, &interest, &tv);
        if (fd != -1) {
            fds[1].fd=fd;
            fds[1].events=(interest&ZOOKEEPER_READ)?POLLIN:0;
            fds[1].events|=(interest&ZOOKEEPER_WRITE)?POLLOUT:0;
            maxfd=2;
        }
        timeout=tv.tv_sec * 1000 + (tv.tv_usec/1000);
        
        poll(fds,maxfd,timeout);
        if (fd != -1) {
            interest=(fds[1].revents&POLLIN)?ZOOKEEPER_READ:0;
            interest|=((fds[1].revents&POLLOUT)||(fds[1].revents&POLLHUP))?ZOOKEEPER_WRITE:0;
        }
        if(fds[0].revents&POLLIN){
            // flush the pipe
            char b[128];
            while(read(adaptor_threads->self_pipe[0],b,sizeof(b))==sizeof(b)){}
        }        
        // dispatch zookeeper events
        rc = zookeeper_process(zh, interest);
        // check the current state of the zhandle and terminate 
        // if it is_unrecoverable()
        if(is_unrecoverable(zh))
            break;
    }
    api_epilog(zh, 0);    
    LOG_DEBUG(("IO thread terminated"));
    return 0;
}


*/

void ebbrt::ZooKeeper::Fire() {
  std::lock_guard<ebbrt::SpinLock> guard(lock_);
    struct pollfd fds[1];
    //struct adaptor_threads *adaptor_threads = zk_->adaptor_priv;

    //api_prolog(zk_);
    //notify_thread_ready(zk_);
    //LOG_DEBUG(("started IO thread"));
    //fds[0].fd=adaptor_threads->self_pipe[0];
    //fds[0].events=POLLIN;
    if(zk_) {
        struct timeval tv;
        int fd;
        int interest;
        int timeout;
        int maxfd=1;
        int rc;
        
        rc = zookeeper_interest(zk_, &fd, &interest, &tv);
        if( rc != ZOK){
          ebbrt::kabort("zookeeper_interest error");
        }
        if (fd != -1) {
            fds[0].fd=fd;
            fds[0].events=(interest&ZOOKEEPER_READ)?POLLIN:0;
            fds[0].events|=(interest&ZOOKEEPER_WRITE)?POLLOUT:0;
            maxfd=1;
            fds[0].revents = 0;
        }
        timeout=tv.tv_sec * 1000 + (tv.tv_usec/1000);
        
        poll(fds,maxfd,timeout);
        if (fd != -1) {
          interest = (fds[0].revents & POLLIN) ? ZOOKEEPER_READ : 0;
          interest |= ((fds[0].revents & POLLOUT) || (fds[0].revents & POLLHUP))
                          ? ZOOKEEPER_WRITE
                          : 0;
        } else{
          ebbrt::kabort("zookeeper_process fd=-1: %d", fd);
        }
        // dispatch zookeeper events
        zookeeper_process(zk_, interest);
        if( rc != ZOK){
          ebbrt::kabort("zookeeper_process error");
        }
        // check the current state of the zhandle and terminate 
        // if it is_unrecoverable()
        if(is_unrecoverable(zk_)){
        //  api_epilog(zk_, 0);    
          ebbrt::kabort(("IO thread terminated"));
        }
    }
    return;
}

bool ebbrt::ZooKeeper::is_connected() {
  return zoo_state(zk_) == ZOO_CONNECTED_STATE;
}

bool ebbrt::ZooKeeper::is_expired() {
  return zoo_state(zk_) == ZOO_EXPIRED_SESSION_STATE;
}

void ebbrt::ZooKeeper::WatchHandler(int type, int state, const char* path) {
  // call global watcher
  if (!global_watcher_) return;

  if (type == ZOO_SESSION_EVENT) {
    if (state == ZOO_EXPIRED_SESSION_STATE) {
      global_watcher_->OnSessionExpired();
    } else if (state == ZOO_CONNECTED_STATE) {
      global_watcher_->OnConnected();
    } else if (state == ZOO_CONNECTING_STATE) {
      global_watcher_->OnConnecting();
    } else {
      // TODO:
      assert(0 && "don't know how to process other session event yet");
    }
  } else if (type == ZOO_CREATED_EVENT) {
    global_watcher_->OnCreated(path);
  } else if (type == ZOO_DELETED_EVENT) {
    global_watcher_->OnDeleted(path);
  } else if (type == ZOO_CHANGED_EVENT) {
    global_watcher_->OnChanged(path);
  } else if (type == ZOO_CHILD_EVENT) {
    global_watcher_->OnChildChanged(path);
  } else if (type == ZOO_NOTWATCHING_EVENT) {
    global_watcher_->OnNotWatching(path);
  } else {
    assert(false && "unknown zookeeper event type");
  }
}

ebbrt::NodeStat ebbrt::ZooKeeper::Stat(const std::string& path) {
  ebbrt::NodeStat stat;
  auto zoo_code = zoo_exists(zk_, path.c_str(), false, &stat);
  
  ebbrt::kbugon(zoo_code != ZOK);


  return stat;
}

std::string ebbrt::ZooKeeper::Get(const std::string& path, bool watch) {
  std::string value_buffer;

  auto node_stat = Stat(path);

  value_buffer.resize(node_stat.dataLength);

  int buffer_len = value_buffer.size();
  auto zoo_code = zoo_get(zk_,
                          path.c_str(),
                          watch,
                          const_cast<char*>(value_buffer.data()),
                          &buffer_len,
                          &node_stat);

  ebbrt::kbugon(zoo_code != ZOK);

  value_buffer.resize(buffer_len);
  return value_buffer;
}
