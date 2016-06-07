MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

app_sources := \
	hostedtmr.cc \
	Printer.cc

target := hostedtmr

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrthosted.mk)
