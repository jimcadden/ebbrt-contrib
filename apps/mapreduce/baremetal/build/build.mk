MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

EBBRT_TARGET := mapreduce
EBBRT_APP_OBJECTS := mapreduce.o Printer.o
EBBRT_APP_VPATH := $(abspath $(MYDIR)../src) 
EBBRT_CONFIG := $(abspath $(MYDIR)../src/ebbrtcfg.h)
EBBRT_APP_INCLUDES += -I $(abspath $(MYDIR)../../ext/mapreduce)
EBBRT_APP_CXXFLAGS :=-Wno-deprecated -Wno-unused-local-typedefs

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrtbaremetal.mk)
