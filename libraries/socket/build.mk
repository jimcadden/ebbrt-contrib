# Makefile for EbbRT socket library
SOCKET_LIB := $(EBBRT_LD_LIBRARY_PATH)/socket

EBBRT_APP_INCLUDES += -I $(SOCKET_LIB)/include \
											-I $(SOCKET_LIB)/ext/lwip/include \
											-I $(SOCKET_LIB)/ext/lwip/include/posix \
											-I $(SOCKET_LIB)/ext/gnu/include/ \
											-iquote $(SOCKET_LIB)/ext/lwip/include/ipv4/ \
											-iquote $(SOCKET_LIB)/ext/lwip/etc \
											-iquote $(SOCKET_LIB)/include

socket_cpp_sources := $(shell find $(SOCKET_LIB)/src -type f -name '*.cc') 
socket_cpp_objects := $(patsubst $(SOCKET_LIB)/src/%.cc, %.o, $(socket_cpp_sources))

lwip_api_sources := $(shell find $(SOCKET_LIB)/ext/lwip/api -type f -name '*.c') 
lwip_core_sources := $(shell find $(SOCKET_LIB)/ext/lwip/core -type f -name '*.c') 
lwip_objects := $(patsubst $(SOCKET_LIB)/ext/lwip/api/%.c, %.o, $(lwip_api_sources)) \
	$(patsubst $(SOCKET_LIB)/ext/lwip/core/%.c, %.o, $(lwip_core_sources)) 

EBBRT_APP_OBJECTS += $(socket_cpp_objects) $(lwip_objects)
EBBRT_APP_VPATH += $(abspath $(SOCKET_LIB)/src) $(abspath $(SOCKET_LIB)/ext/lwip/api) \
									 $(abspath $(SOCKET_LIB)/ext/lwip/core)
