MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

app_sources := \
	count.cc \
	Printer.cc

target := count

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrthosted.mk)
