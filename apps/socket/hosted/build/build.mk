MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

app_sources := \
	socket.cc \
	Printer.cc

target := socket

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrthosted.mk)
