MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

app_sources := \
	netpipe.cc \
	Printer.cc

target := netpipe

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrthosted.mk)
