MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

app_sources := \
	MsgTst.cc \
	msgtst.cc

target := msgtst

EBBRT_APP_VPATH := $(abspath $(MYDIR)../../src)

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrthosted.mk)
