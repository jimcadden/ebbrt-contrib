MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

app_sources := \
	mapreduce.cc \
	Printer.cc

target := mapreduce

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrthosted.mk)
