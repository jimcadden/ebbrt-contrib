MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

app_sources := \
	zk.cc \
	Printer.cc

target := zk

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrthosted.mk)
