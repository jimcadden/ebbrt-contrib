# Makefile for EbbRT ZooKeeper library
ZK_LIB := $(EBBRT_LD_LIBRARY_PATH)/zookeeper

EBBRT_APP_INCLUDES += -I $(ZK_LIB)/include \
											-I $(ZK_LIB)/ext/zookeeper/include \
											-I $(ZK_LIB)/ext/zookeeper/generated 

zk_cpp_sources := $(shell find $(ZK_LIB)/src -type f -name '*.cc') 
zk_cpp_objects := $(patsubst $(ZK_LIB)/src/%.cc, %.o, $(zk_cpp_sources))

zk_c_sources := $(shell find $(ZK_LIB)/ext/zookeeper -type f -name '*.c') 
zk_c_objects := $(patsubst $(ZK_LIB)/ext/zookeeper/%.c, %.o, $(zk_c_sources))

EBBRT_APP_OBJECTS += $(zk_cpp_objects) $(zk_c_objects) 
EBBRT_APP_VPATH += $(abspath $(ZK_LIB)/src) $(abspath $(ZK_LIB)/ext/zookeeper) 
EBBRT_APP_CFLAGS += -Wno-unused-variable -Wno-unused-function
