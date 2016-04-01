MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

app_sources := \
	sanity.cc \
	Printer.cc

target := sanity

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrthosted.mk)
