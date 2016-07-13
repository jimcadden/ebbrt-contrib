MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

NETPIPE_SRC_DIR := $(abspath $(MYDIR)../ext/NetPIPE-3.7.2/src)
netpipe_c_sources := $(shell find $(NETPIPE_SRC_DIR) -type f -name '*.c') 
netpipe_c_objects := $(patsubst $(NETPIPE_SRC_DIR)/%.c, %.o, $(netpipe_c_sources))

EBBRT_APP_INCLUDES := -I $(NETPIPE_SRC_DIR)

EBBRT_TARGET := netpipe
EBBRT_APP_OBJECTS := Netpipe.o Printer.o 
EBBRT_APP_VPATH := $(abspath $(MYDIR)../src) $(abspath $(MYDIR)../ext/NetPIPE-3.7.2/src)
EBBRT_CONFIG := $(abspath $(MYDIR)../src/ebbrtcfg.h)
EBBRT_APP_CFLAGS := -DTCP -Wno-unused-variable
EBBRT_APP_CXXFLAGS := -DTCP

include $(abspath $(EBBRT_LD_LIBRARY_PATH)/socket/build.mk)
EBBRT_APP_OBJECTS += $(netpipe_c_objects) 
include $(abspath $(EBBRT_SRCDIR)/apps/ebbrtbaremetal.mk)
