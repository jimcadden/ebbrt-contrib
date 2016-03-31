MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

app_sources := \
	trace.cc \
	Printer.cc

target := trace

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrthosted.mk)
