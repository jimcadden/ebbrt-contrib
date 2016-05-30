MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

app_sources := \
	iocmd.cc \
	Printer.cc

target := iocmd

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrthosted.mk)
