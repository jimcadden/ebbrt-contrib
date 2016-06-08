# Makefile for EbbRT ZooKeeper library
ZK_LIB := $(EBBRT_LD_LIBRARY_PATH)/zookeeper

EBBRT_APP_INCLUDES += -I $(ZK_LIB)/src \
											-I $(HOME)/usr/local/include/zookeeper

zk_cpp_sources := $(shell find $(ZK_LIB)/src -type f -name '*.cc') 
zk_cpp_objects := $(patsubst $(ZK_LIB)/src/%.cc, %.o, $(zk_cpp_sources))


EBBRT_APP_OBJECTS += $(zk_cpp_objects) 
EBBRT_APP_VPATH += $(abspath $(ZK_LIB)/src) 
EBBRT_APP_CXXFLAGS += -lzookeeper_st 
EBBRT_APP_LDFLAGS += -L$(LD_LIBRARY_PATH) -lzookeeper_st

