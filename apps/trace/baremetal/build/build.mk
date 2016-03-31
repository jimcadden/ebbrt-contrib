MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

EBBRT_TARGET := trace
EBBRT_APP_OBJECTS := trace.o Printer.o
EBBRT_APP_VPATH := $(abspath $(MYDIR)../src)
EBBRT_CONFIG := $(abspath $(MYDIR)../src/ebbrtcfg.h)

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrtbaremetal.mk)
