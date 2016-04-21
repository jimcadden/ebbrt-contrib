MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

zk_sources := $(shell find $(MYDIR)../ext/zookeeper -type f -name '*.c')
zk_objects := $(patsubst $(MYDIR)../ext/zookeeper/%.c, $(MYDIR)../ext/zookeeper/%.o, $(zk_sources))
EBBRT_APP_INCLUDES += -I $(MYDIR)../ext/zookeeper/include

EBBRT_TARGET := zk
EBBRT_APP_OBJECTS := zk.o Printer.o $(zk_objects)
EBBRT_APP_VPATH := $(abspath $(MYDIR)../src)
EBBRT_CONFIG := $(abspath $(MYDIR)../src/ebbrtcfg.h)


include $(abspath $(EBBRT_SRCDIR)/apps/ebbrtbaremetal.mk)
