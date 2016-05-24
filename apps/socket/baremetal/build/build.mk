MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

EBBRT_TARGET := socket
EBBRT_APP_INCLUDES += -I $(MYDIR)../ext/lwip/include \
											-iquote $(MYDIR)../ext/lwip/include/ipv4/ \
											-I $(MYDIR)../ext/lwip/include/posix \
											-I $(MYDIR)../ext/zookeeper/include \
											-I $(MYDIR)/../src/zookeeper-cpp

zk_sources := $(shell find $(MYDIR)../ext/zookeeper -type f -name '*.c') 
zk_objects := $(patsubst $(MYDIR)../ext/zookeeper/%.c, %.o, $(zk_sources))
zk_cpp_sources := $(shell find $(MYDIR)../ext/zookeeper -type f -name '*.cc') 
zk_cpp_objects := $(patsubst $(MYDIR)../ext/zookeeper/%.cc, %.o, $(zk_cpp_sources))

EBBRT_APP_OBJECTS := socket.o Printer.o SocketManager.o Vfs.o Socket.o ip_addr.o $(zk_objects) $(zk_cpp_objects)
EBBRT_APP_VPATH := $(abspath $(MYDIR)../src) $(abspath $(MYDIR)../ext/zookeeper)
EBBRT_CONFIG := $(abspath $(MYDIR)../src/ebbrtcfg.h) 
EBBRT_APP_CPPFLAGS := -Wno-unused-function -Wno-unused-variable -Wno-write-strings -Wno-sign-compare  -Wno-error
EBBRT_APP_CXXFLAGS := -fpermissive

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrtbaremetal.mk)
