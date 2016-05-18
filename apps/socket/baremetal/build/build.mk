MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

EBBRT_TARGET := socket
EBBRT_APP_INCLUDES += -I $(MYDIR)../ext/lwip/include \
											-iquote $(MYDIR)../ext/lwip/include/ipv4/ \
											-I $(MYDIR)../ext/lwip/include/posix 
EBBRT_APP_OBJECTS := socket.o Printer.o SocketManager.o Vfs.o Socket.o
EBBRT_APP_VPATH := $(abspath $(MYDIR)../src)
EBBRT_CONFIG := $(abspath $(MYDIR)../src/ebbrtcfg.h)

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrtbaremetal.mk)
