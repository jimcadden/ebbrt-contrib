MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

app_sources := \
	iperfpv.cc \
	Printer.cc

target := iperfpv

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrthosted.mk)
