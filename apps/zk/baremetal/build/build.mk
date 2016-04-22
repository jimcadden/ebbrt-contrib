MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

zk_sources := $(shell find $(MYDIR)../ext/zookeeper -type f -name '*.c') 
zk_objects := $(patsubst $(MYDIR)../ext/zookeeper/%.c, $(MYDIR)../ext/zookeeper/%.o, $(zk_sources))

zk_cpp_sources := $(shell find $(MYDIR)../ext/zookeeper -type f -name '*.cc') 
zk_cpp_objects := $(patsubst $(MYDIR)../ext/zookeeper/%.cc, $(MYDIR)../ext/zookeeper/%.o, $(zk_cpp_sources))

EBBRT_APP_INCLUDES += -I $(MYDIR)../ext/zookeeper/include -I $(MYDIR)/../src/zookeeper-cpp

EBBRT_TARGET := zk
EBBRT_APP_OBJECTS := zk.o Printer.o zookeeper.o zookeeper_error.o zookeeper_ext.o $(zk_objects) $(zk_cpp_objects)
EBBRT_APP_VPATH := $(abspath $(MYDIR)../src)
EBBRT_CONFIG := $(abspath $(MYDIR)../src/ebbrtcfg.h)
EBBRT_APP_CPPFLAGS := -Wno-unused-function -Wno-unused-variable -Wno-write-strings -Wno-sign-compare -fpermissive


include $(abspath $(EBBRT_SRCDIR)/apps/ebbrtbaremetal.mk)
