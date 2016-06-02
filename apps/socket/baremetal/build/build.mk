MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

EBBRT_TARGET := socket
EBBRT_APP_INCLUDES =
EBBRT_APP_OBJECTS := socket.o 
EBBRT_APP_VPATH := $(abspath $(MYDIR)../src) 
EBBRT_CONFIG := $(abspath $(MYDIR)../src/ebbrtcfg.h) 
EBBRT_APP_CPPFLAGS := 
EBBRT_APP_CXXFLAGS := 

include $(abspath $(EBBRT_LD_LIBRARY_PATH)/socket/build.mk)
include $(abspath $(EBBRT_LD_LIBRARY_PATH)/zookeeper/build.mk)
include $(abspath $(EBBRT_SRCDIR)/apps/ebbrtbaremetal.mk)
