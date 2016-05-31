MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

EBBRT_TARGET := socket
EBBRT_APP_INCLUDES += -I $(MYDIR)../ext/lwip/include \
											-I $(MYDIR)../ext/lwip/include/posix \
											-I $(MYDIR)../ext/gnu/include/ \
											-iquote $(MYDIR)../ext/lwip/include/ipv4/ \
											-iquote $(MYDIR)../ext/lwip/opt/ \
											-I $(MYDIR)../ext/zookeeper/include \
											-I $(MYDIR)../ext/zookeeper/generated \
											-I $(MYDIR)/../src/zookeeper-cpp

zk_sources := $(shell find $(MYDIR)../ext/zookeeper -type f -name '*.c') 
zk_objects := $(patsubst $(MYDIR)../ext/zookeeper/%.c, %.o, $(zk_sources))
lwip_api_sources := $(shell find $(MYDIR)../ext/lwip/api -type f -name '*.c') 
lwip_core_sources := $(shell find $(MYDIR)../ext/lwip/core -type f -name '*.c') 
lwip_objects := $(patsubst $(MYDIR)../ext/lwip/api/%.c, %.o, $(lwip_api_sources)) \
	$(patsubst $(MYDIR)../ext/lwip/core/%.c, %.o, $(lwip_core_sources))

EBBRT_APP_OBJECTS := socket.o Printer.o SocketManager.o Vfs.o lwip_socket_api.o ZooKeeper.o  $(zk_objects) $(lwip_objects)
EBBRT_APP_VPATH := $(abspath $(MYDIR)../src) $(abspath $(MYDIR)../ext/zookeeper) $(abspath $(MYDIR)../ext/lwip/api) $(abspath $(MYDIR)../ext/lwip/core)
EBBRT_CONFIG := $(abspath $(MYDIR)../src/ebbrtcfg.h) 
EBBRT_APP_CPPFLAGS := -Wno-unused-function -Wno-unused-variable -Wno-write-strings -Wno-sign-compare  -Wno-error 
EBBRT_APP_CXXFLAGS := 

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrtbaremetal.mk)
