//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ebbrt/Debug.h>

#define SOCK_CLI 0
#define SOCK_SRV 0
#define SOCK_ALL 0
#define ZK_CLI 1

/*******************************************************************/
/*******************************************************************/
#if SOCK_SRV 

#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <string>


void AppMain() {
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    std::string sendBuff ("Hello World!\n");
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    //if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    //{
    //  ebbrt::kabort("\n inet_pton error occured\n");
    //    return;
    //} 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
        ebbrt::kprintf("Listening for a connection....\n");
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        //ticks = time(NULL);
        //snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));

        write(connfd, sendBuff.c_str(), sendBuff.size()); 

        close(connfd);
        //sleep(1);
     }
}
#endif // SOCK_SRV
/*******************************************************************/
/*******************************************************************/

#if SOCK_CLI

#include <cstring>
#include <sys/socket.h>
#include <errno.h>

void AppMain() {

  int sockfd = 0, n = 0;
  char recvBuff[1024];
  struct sockaddr_in serv_addr;
  char ip[15] = "0.0.0.0"; 
  
  ebbrt::kprintf("The time is: \n");
  memset(recvBuff, '0', sizeof(recvBuff));

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    ebbrt::kprintf("\n Error : Could not create socket (%d)\n", errno);
    return;
  }
  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5000); 
  
  if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
    ebbrt::kprintf("\n inet_on error occured (%d)\n");

    return;
  }
  
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    ebbrt::kprintf("Error : Connect Failed \n");
    return;
  }

  while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
    recvBuff[n] = '\0';
    ebbrt::kprintf("> %s \n", recvBuff);
  }
  close(sockfd);
}
#endif // SOCK_CLI


/*******************************************************************/
/*******************************************************************/
#if SOCK_ALL
#include <sys/socket.h>

void AppMain() {
  int fd;
  const struct sockaddr *saddr = nullptr;
  socklen_t namelen = {};

  fd = socket(AF_INET, SOCK_STREAM, 0);
  fd = recv(fd, nullptr, 0, 0);

  // ALL
  // socket.h
  fd = accept(0, nullptr, nullptr);      
  fd = bind(0, nullptr, namelen);      
  fd = connect(0, saddr, namelen);      
  fd = closesocket(fd); // WIN32
  fd = getpeername(0, nullptr, nullptr);      
  fd = getsockname(0, nullptr, nullptr);      
  fd = getsockopt(0, SOL_SOCKET, SO_ERROR, nullptr, nullptr);
  fd = ioctlsocket(0,0,nullptr); // WIN32
  fd = listen(0,0);      
  fd = recv(0, nullptr, 0, 0);      
  fd = recvfrom(0, nullptr, 0, 0, nullptr, nullptr);      
  fd = send(0, nullptr, 0, 0);      
  fd = sendto(0, nullptr, 0, 0, nullptr, namelen);      
  fd = setsockopt(0, SOL_SOCKET, SO_ERROR, saddr, namelen);
  fd = shutdown(0,0);
  fd = socket(0,0,0);

  // fcntl.h
  fd = read(0,nullptr,0);
  fd = write(0,nullptr,0);
  fd = close(0);
  fd = fcntl(0,0,0);

  // newlib
  //fd = fstat();
}
#endif // SOCK_ALL

/*******************************************************************/
/*******************************************************************/
#if ZK_CLI

#include "zk_config.h"
#include <zookeeper.h>
#include <proto.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>
#include <unistd.h>
//#include <sys/select.h> 

#include <time.h>
#include <errno.h>
#include <assert.h>

#include <sys/types.h>

#define _LL_CAST_ (long long)

static zhandle_t *zh;
static clientid_t myid;
static const char *clientIdFile = 0;
struct timeval startTime;
static char cmd[1024];
static int batchMode=0;

static int to_send = 0;
static int sent = 0;
static int recvd = 0;

static int shutdownThisThing=0;

static __attribute__ ((unused)) void 
printProfileInfo(struct timeval start, struct timeval end, int thres,
                 const char* msg)
{
  int delay=(end.tv_sec*1000+end.tv_usec/1000)-
    (start.tv_sec*1000+start.tv_usec/1000);
  if(delay>thres)
    ebbrt::kprintf("%s: execution time=%dms\n",msg,delay);
}

static const char* state2String(int state){
  if (state == 0)
    return "CLOSED_STATE";
  if (state == ZOO_CONNECTING_STATE)
    return "CONNECTING_STATE";
  if (state == ZOO_ASSOCIATING_STATE)
    return "ASSOCIATING_STATE";
  if (state == ZOO_CONNECTED_STATE)
    return "CONNECTED_STATE";
  if (state == ZOO_EXPIRED_SESSION_STATE)
    return "EXPIRED_SESSION_STATE";
  if (state == ZOO_AUTH_FAILED_STATE)
    return "AUTH_FAILED_STATE";

  return "INVALID_STATE";
}

static const char* type2String(int state){
  if (state == ZOO_CREATED_EVENT)
    return "CREATED_EVENT";
  if (state == ZOO_DELETED_EVENT)
    return "DELETED_EVENT";
  if (state == ZOO_CHANGED_EVENT)
    return "CHANGED_EVENT";
  if (state == ZOO_CHILD_EVENT)
    return "CHILD_EVENT";
  if (state == ZOO_SESSION_EVENT)
    return "SESSION_EVENT";
  if (state == ZOO_NOTWATCHING_EVENT)
    return "NOTWATCHING_EVENT";

  return "UNKNOWN_EVENT_TYPE";
}

void watcher(zhandle_t *zzh, int type, int state, const char *path,
             void* context)
{
    /* Be careful using zh here rather than zzh - as this may be mt code
     * the client lib may call the watcher before zookeeper_init returns */

  ebbrt::kprintf("Watcher %s state = %s", type2String(type), state2String(state));
    if (path && strlen(path) > 0) {
      ebbrt::kprintf(" for path %s", path);
    }
    ebbrt::kprintf("\n");

    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
            const clientid_t *id = zoo_client_id(zzh);
            if (myid.client_id == 0 || myid.client_id != id->client_id) {
                myid = *id;
                ebbrt::kprintf("Got a new session id: 0x%llx\n",
                        _LL_CAST_ myid.client_id);
                if (clientIdFile) {
                    FILE *fh = fopen(clientIdFile, "w");
                    if (!fh) {
                        perror(clientIdFile);
                    } else {
                        int rc = fwrite(&myid, sizeof(myid), 1, fh);
                        if (rc != sizeof(myid)) {
                            perror("writing client id");
                        }
                        fclose(fh);
                    }
                }
            }
        } else if (state == ZOO_AUTH_FAILED_STATE) {
          ebbrt::kprintf("Authentication failure. Shutting down...\n");
            zookeeper_close(zzh);
            shutdownThisThing=1;
            zh=0;
        } else if (state == ZOO_EXPIRED_SESSION_STATE) {
          ebbrt::kprintf("Session expired. Shutting down...\n");
            zookeeper_close(zzh);
            shutdownThisThing=1;
            zh=0;
        }
    }
}

void dumpStat(const struct Stat *stat) {
    char tctimes[40];
    char tmtimes[40];
    time_t tctime;
    time_t tmtime;

    if (!stat) {
      ebbrt::kprintf("null\n");
        return;
    }
    tctime = stat->ctime/1000;
    tmtime = stat->mtime/1000;
       
    ctime_r(&tmtime, tmtimes);
    ctime_r(&tctime, tctimes);
       
    ebbrt::kprintf("\tctime = %s\tczxid=%llx\n"
    "\tmtime=%s\tmzxid=%llx\n"
    "\tversion=%x\taversion=%x\n"
    "\tephemeralOwner = %llx\n",
     tctimes, _LL_CAST_ stat->czxid, tmtimes,
    _LL_CAST_ stat->mzxid,
    (unsigned int)stat->version, (unsigned int)stat->aversion,
    _LL_CAST_ stat->ephemeralOwner);
}

void my_string_completion(int rc, const char *name, const void *data) {
  ebbrt::kprintf("[%s]: rc = %d\n", (char*)(data==0?"null":data), rc);
    if (!rc) {
      ebbrt::kprintf("\tname = %s\n", name);
    }
    if(batchMode)
      shutdownThisThing=1;
}

void my_string_completion_free_data(int rc, const char *name, const void *data) {
    my_string_completion(rc, name, data);
    free((void*)data);
}

void my_data_completion(int rc, const char *value, int value_len,
        const struct Stat *stat, const void *data) {
    struct timeval tv;
    int sec;
    int usec;
    gettimeofday(&tv, 0);
    sec = tv.tv_sec - startTime.tv_sec;
    usec = tv.tv_usec - startTime.tv_usec;
    ebbrt::kprintf("time = %d msec\n", sec*1000 + usec/1000);
    ebbrt::kprintf("%s: rc = %d\n", (char*)data, rc);
    if (value) {
      ebbrt::kprintf(" value_len = %d\n", value_len);
        assert(write(2, value, value_len) == value_len);
    }
    ebbrt::kprintf("\nStat:\n");
    dumpStat(stat);
    free((void*)data);
    if(batchMode)
      shutdownThisThing=1;
}

void my_silent_data_completion(int rc, const char *value, int value_len,
        const struct Stat *stat, const void *data) {
    recvd++;
    ebbrt::kprintf("Data completion %s rc = %d\n",(char*)data,rc);
    free((void*)data);
    if (recvd==to_send) {
      ebbrt::kprintf("Recvd %d responses for %d requests sent\n",recvd,to_send);
        if(batchMode)
          shutdownThisThing=1;
    }
}

void my_strings_completion(int rc, const struct String_vector *strings,
        const void *data) {
    struct timeval tv;
    int sec;
    int usec;
    int i;

    gettimeofday(&tv, 0);
    sec = tv.tv_sec - startTime.tv_sec;
    usec = tv.tv_usec - startTime.tv_usec;
    ebbrt::kprintf("time = %d msec\n", sec*1000 + usec/1000);
    ebbrt::kprintf("%s: rc = %d\n", (char*)data, rc);
    if (strings)
        for (i=0; i < strings->count; i++) {
          ebbrt::kprintf("\t%s\n", strings->data[i]);
        }
    free((void*)data);
    gettimeofday(&tv, 0);
    sec = tv.tv_sec - startTime.tv_sec;
    usec = tv.tv_usec - startTime.tv_usec;
    ebbrt::kprintf("time = %d msec\n", sec*1000 + usec/1000);
    if(batchMode)
      shutdownThisThing=1;
}

void my_strings_stat_completion(int rc, const struct String_vector *strings,
        const struct Stat *stat, const void *data) {
    my_strings_completion(rc, strings, data);
    dumpStat(stat);
    if(batchMode)
      shutdownThisThing=1;
}

void my_void_completion(int rc, const void *data) {
  ebbrt::kprintf("%s: rc = %d\n", (char*)data, rc);
    free((void*)data);
    if(batchMode)
      shutdownThisThing=1;
}

void my_stat_completion(int rc, const struct Stat *stat, const void *data) {
  ebbrt::kprintf("%s: rc = %d Stat:\n", (char*)data, rc);
    dumpStat(stat);
    free((void*)data);
    if(batchMode)
      shutdownThisThing=1;
}

void my_silent_stat_completion(int rc, const struct Stat *stat,
        const void *data) {
    //    ebbrt::kprintf("State completion: [%s] rc = %d\n", (char*)data, rc);
    sent++;
    free((void*)data);
}

static void sendRequest(const char* data) {
    zoo_aset(zh, "/od", data, strlen(data), -1, my_silent_stat_completion,
            strdup("/od"));
    zoo_aget(zh, "/od", 1, my_silent_data_completion, strdup("/od"));
}

void od_completion(int rc, const struct Stat *stat, const void *data) {
    int i;
    ebbrt::kprintf("od command response: rc = %d Stat:\n", rc);
    dumpStat(stat);
    // send a whole bunch of requests
    recvd=0;
    sent=0;
    to_send=200;
    for (i=0; i<to_send; i++) {
        char buf[4096*16];
        memset(buf, -1, sizeof(buf)-1);
        buf[sizeof(buf)-1]=0;
        sendRequest(buf);
    }
}

int startsWith(const char *line, const char *prefix) {
    int len = strlen(prefix);
    return strncmp(line, prefix, len) == 0;
}

static const char *hostPort;
static int verbose = 0;

void processline(char *line) {
    int rc;
    int async = ((line[0] == 'a') && !(startsWith(line, "addauth ")));
    if (async) {
        line++;
    }
    if (startsWith(line, "help")) {
      ebbrt::kprintf("    create [+[e|s]] <path>\n");
      ebbrt::kprintf("    delete <path>\n");
      ebbrt::kprintf("    set <path> <data>\n");
      ebbrt::kprintf("    get <path>\n");
      ebbrt::kprintf("    ls <path>\n");
      ebbrt::kprintf("    ls2 <path>\n");
      ebbrt::kprintf("    sync <path>\n");
      ebbrt::kprintf("    exists <path>\n");
      ebbrt::kprintf("    wexists <path>\n");
      ebbrt::kprintf("    myid\n");
      ebbrt::kprintf("    verbose\n");
      ebbrt::kprintf("    addauth <id> <scheme>\n");
      ebbrt::kprintf("    quit\n");
      ebbrt::kprintf("\n");
      ebbrt::kprintf("    prefix the command with the character 'a' to run the command asynchronously.\n");
      ebbrt::kprintf("    run the 'verbose' command to toggle verbose logging.\n");
      ebbrt::kprintf("    i.e. 'aget /foo' to get /foo asynchronously\n");
    } else if (startsWith(line, "verbose")) {
      if (verbose) {
        verbose = 0;
        zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
        ebbrt::kprintf("logging level set to WARN\n");
      } else {
        verbose = 1;
        zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
        ebbrt::kprintf("logging level set to DEBUG\n");
      }
    } else if (startsWith(line, "get ")) {
        line += 4;
        if (line[0] != '/') {
          ebbrt::kprintf("Path must start with /, found: %s\n", line);
            return;
        }
               
        rc = zoo_aget(zh, line, 1, my_data_completion, strdup(line));
        if (rc) {
          ebbrt::kprintf("Error %d for %s\n", rc, line);
        }
    } else if (startsWith(line, "set ")) {
        char *ptr;
        line += 4;
        if (line[0] != '/') {
          ebbrt::kprintf("Path must start with /, found: %s\n", line);
            return;
        }
        ptr = strchr(line, ' ');
        if (!ptr) {
          ebbrt::kprintf("No data found after path\n");
            return;
        }
        *ptr = '\0';
        ptr++;
        if (async) {
            rc = zoo_aset(zh, line, ptr, strlen(ptr), -1, my_stat_completion,
                    strdup(line));
        } else {
            struct Stat stat;
            rc = zoo_set2(zh, line, ptr, strlen(ptr), -1, &stat);
        }
        if (rc) {
          ebbrt::kprintf("Error %d for %s\n", rc, line);
        }
    } else if (startsWith(line, "ls ")) {
        line += 3;
        if (line[0] != '/') {
          ebbrt::kprintf("Path must start with /, found: %s\n", line);
            return;
        }
        gettimeofday(&startTime, 0);
        rc= zoo_aget_children(zh, line, 1, my_strings_completion, strdup(line));
        if (rc) {
          ebbrt::kprintf("Error %d for %s\n", rc, line);
        }
    } else if (startsWith(line, "ls2 ")) {
        line += 4;
        if (line[0] != '/') {
          ebbrt::kprintf("Path must start with /, found: %s\n", line);
            return;
        }
        gettimeofday(&startTime, 0);
        rc= zoo_aget_children2(zh, line, 1, my_strings_stat_completion, strdup(line));
        if (rc) {
          ebbrt::kprintf("Error %d for %s\n", rc, line);
        }
    } else if (startsWith(line, "create ")) {
        int flags = 0;
        line += 7;
        if (line[0] == '+') {
            line++;
            if (line[0] == 'e') {
                flags |= ZOO_EPHEMERAL;
                line++;
            }
            if (line[0] == 's') {
                flags |= ZOO_SEQUENCE;
                line++;
            }
            line++;
        }
        if (line[0] != '/') {
          ebbrt::kprintf("Path must start with /, found: %s\n", line);
            return;
        }
        ebbrt::kprintf("Creating [%s] node\n", line);
//        {
//            struct ACL _CREATE_ONLY_ACL_ACL[] = {{ZOO_PERM_CREATE, ZOO_ANYONE_ID_UNSAFE}};
//            struct ACL_vector CREATE_ONLY_ACL = {1,_CREATE_ONLY_ACL_ACL};
//            rc = zoo_acreate(zh, line, "new", 3, &CREATE_ONLY_ACL, flags,
//                    my_string_completion, strdup(line));
//        }
        rc = zoo_acreate(zh, line, "new", 3, &ZOO_OPEN_ACL_UNSAFE, flags,
                my_string_completion_free_data, strdup(line));
        if (rc) {
          ebbrt::kprintf("Error %d for %s\n", rc, line);
        }
    } else if (startsWith(line, "delete ")) {
        line += 7;
        if (line[0] != '/') {
          ebbrt::kprintf("Path must start with /, found: %s\n", line);
            return;
        }
        if (async) {
            rc = zoo_adelete(zh, line, -1, my_void_completion, strdup(line));
        } else {
            rc = zoo_delete(zh, line, -1);
        }
        if (rc) {
          ebbrt::kprintf("Error %d for %s\n", rc, line);
        }
    } else if (startsWith(line, "sync ")) {
        line += 5;
        if (line[0] != '/') {
          ebbrt::kprintf("Path must start with /, found: %s\n", line);
            return;
        }
        rc = zoo_async(zh, line, my_string_completion_free_data, strdup(line));
        if (rc) {
          ebbrt::kprintf("Error %d for %s\n", rc, line);
        }
    } else if (startsWith(line, "wexists ")) {
#ifdef THREADED
        struct Stat stat;
#endif
        line += 8;
        if (line[0] != '/') {
          ebbrt::kprintf("Path must start with /, found: %s\n", line);
            return;
        }
#ifndef THREADED
        rc = zoo_awexists(zh, line, watcher, (void*) 0, my_stat_completion, strdup(line));
#else
        rc = zoo_wexists(zh, line, watcher, (void*) 0, &stat);
#endif
        if (rc) {
          ebbrt::kprintf("Error %d for %s\n", rc, line);
        }
    } else if (startsWith(line, "exists ")) {
#ifdef THREADED
        struct Stat stat;
#endif
        line += 7;
        if (line[0] != '/') {
          ebbrt::kprintf("Path must start with /, found: %s\n", line);
            return;
        }
#ifndef THREADED
        rc = zoo_aexists(zh, line, 1, my_stat_completion, strdup(line));
#else
        rc = zoo_exists(zh, line, 1, &stat);
#endif
        if (rc) {
          ebbrt::kprintf("Error %d for %s\n", rc, line);
        }
    } else if (strcmp(line, "myid") == 0) {
        printf("session Id = %llx\n", _LL_CAST_ zoo_client_id(zh)->client_id);
    } else if (strcmp(line, "reinit") == 0) {
        zookeeper_close(zh);
        // we can't send myid to the server here -- zookeeper_close() removes 
        // the session on the server. We must start anew.
        zh = zookeeper_init(hostPort, watcher, 30000, 0, 0, 0);
    } else if (startsWith(line, "quit")) {
      ebbrt::kprintf("Quitting...\n");
        shutdownThisThing=1;
    } else if (startsWith(line, "od")) {
        const char val[]="fire off";
        ebbrt::kprintf("Overdosing...\n");
        rc = zoo_aset(zh, "/od", val, sizeof(val)-1, -1, od_completion, 0);
        if (rc)
            ebbrt::kprintf("od command failed: %d\n", rc);
    } else if (startsWith(line, "addauth ")) {
      char *ptr;
      line += 8;
      ptr = strchr(line, ' ');
      if (ptr) {
        *ptr = '\0';
        ptr++;
      }
      zoo_add_auth(zh, line, ptr, ptr ? strlen(ptr) : 0, NULL, NULL);
    }
}


void AppMain() {
    ebbrt::kprintf("Zookeeper Client \n");
    fd_set rfds, wfds, efds;
    int processed=0;
    char buffer[4096];
    char p[2048];
    int bufoff = 0;
    FILE *fh;

    hostPort = "172.17.0.4:2181";
    zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
    zoo_deterministic_conn_order(1); // enable deterministic order

    zh = zookeeper_init(hostPort, watcher, 30000, &myid, 0, 0);
    if (!zh) {
      ebbrt::kabort("Zookeeper Init Failed\n");
    }

        int fd;
        int interest;
        int events;
        struct timeval tv;
        int rc;
        zookeeper_interest(zh, &fd, &interest, &tv);
        if (fd != -1) {
            if (interest&ZOOKEEPER_READ) {
                FD_SET(fd, &rfds);
            } else {
                FD_CLR(fd, &rfds);
            }
            if (interest&ZOOKEEPER_WRITE) {
                FD_SET(fd, &wfds);
            } else {
                FD_CLR(fd, &wfds);
            }
        } else {
            fd = 0;
        }
        FD_SET(0, &rfds);
        ebbrt::kprintf("SELECT..\n");
        rc = select(fd+1, &rfds, &wfds, &efds, &tv);
        events = 0;
        if (rc > 0) {
            if (FD_ISSET(fd, &rfds)) {
                events |= ZOOKEEPER_READ;
            }
            if (FD_ISSET(fd, &wfds)) {
                events |= ZOOKEEPER_WRITE;
            }
        }
        if(batchMode && processed==0){
          //batch mode
          processline(cmd);
          processed=1;
        }
        if (FD_ISSET(0, &rfds)) {
            int rc;
            int len = sizeof(buffer) - bufoff -1;
            if (len <= 0) {
              ebbrt::kprintf("Can't handle lines that long!\n");
                exit(2);
            }
            rc = read(0, buffer+bufoff, len);
            if (rc <= 0) {
              ebbrt::kprintf("bye\n");
                break;
            }
            bufoff += rc;
            buffer[bufoff] = '\0';
            while (strchr(buffer, '\n')) {
                char *ptr = strchr(buffer, '\n');
                *ptr = '\0';
                processline(buffer);
                ptr++;
                memmove(buffer, ptr, strlen(ptr)+1);
                bufoff = 0;
            }
        }
        zookeeper_process(zh, events);

    if (to_send!=0)
        ebbrt::kprintf("Recvd %d responses for %d requests sent\n",recvd,sent);
    zookeeper_close(zh);
    return ;
}

#endif // end ZK_CLI

