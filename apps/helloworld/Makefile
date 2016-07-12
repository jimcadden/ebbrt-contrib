MYDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

APP_TARGET ?= helloworld

CD ?= cd
CMAKE ?= cmake
CP ?= cp
ECHO ?= echo
MAKE ?= make
MKDIR ?= mkdir

EBBRTSRCDIR ?= $(abspath $(EBBRT_SRCDIR))# required variable
EBBRTSYSROOT ?= $(abspath $(EBBRT_SYSROOT))# required variable

EBBRT_HOSTED_SRC_PATH ?= $(EBBRTSRCDIR)/hosted
EBBRT_BUILD_PATH ?= $(MYDIR)/ebbrt
EBBRT_HOSTED_BUILD_PATH ?= $(EBBRT_BUILD_PATH)/hosted
EBBRT_HOSTED_INSTALL_PATH ?= $(MYDIR)/usr
EBBRT_HOSTED_LIB_PATH ?= $(EBBRT_HOSTED_INSTALL_PATH)/lib
EBBRT_HOSTED_CMAKE_CONFIG_PATH ?= $(EBBRT_HOSTED_LIB_PATH)/cmake/EbbRT

CMAKE_TOOLCHAIN_FILE ?= $(EBBRTSRCDIR)/baremetal/cmake/ebbrt.cmake
CMAKE_PACKAGE_CONFIG_FILE ?= $(EBBRT_HOSTED_CMAKE_CONFIG_PATH)/EbbRTConfig.cmake

BUILD_PATH ?= $(MYDIR)
DEBUG_PATH ?= $(BUILD_PATH)/Debug
RELEASE_PATH ?= $(BUILD_PATH)/Release

BAREMETAL_DEBUG_DIR ?= $(DEBUG_PATH)/bm
BAREMETAL_RELEASE_DIR ?= $(RELEASE_PATH)/bm
HOSTED_DEBUG_DIR ?= $(DEBUG_PATH)
HOSTED_RELEASE_DIR ?= $(RELEASE_PATH)

all: Debug Release
Debug: baremetal-debug hosted-debug
Release: baremetal-release baremetal-debug

$(BUILD_PATH): 
	$(MKDIR) $@

$(DEBUG_PATH): | $(BUILD_PATH)
	$(MKDIR) $@

$(RELEASE_PATH): | $(BUILD_PATH)
	$(MKDIR) $@

$(BAREMETAL_DEBUG_DIR): | $(DEBUG_PATH)
	$(MKDIR) $@

$(BAREMETAL_RELEASE_DIR): | $(RELEASE_PATH)
	$(MKDIR) $@

$(HOSTED_DEBUG_DIR): | $(DEBUG_PATH)
	$(MKDIR) $@

$(HOSTED_RELEASE_DIR): | $(RELEASE_PATH)
	$(MKDIR) $@

check-ebbrt-src:
ifndef EBBRT_SRCDIR
	$(error EBBRT_SRCDIR is undefined)
endif

check-ebbrt-sysroot:
ifndef EBBRT_SYSROOT
	$(error EBBRT_SYSROOT is undefined)
endif

baremetal-debug: | check-ebbrt-src check-ebbrt-sysroot $(BAREMETAL_DEBUG_DIR) 
	$(CD) $(BAREMETAL_DEBUG_DIR) && \
		EBBRT_SYSROOT=$(EBBRTSYSROOT) $(CMAKE) -DCMAKE_TOOLCHAIN_FILE=$(CMAKE_TOOLCHAIN_FILE) $(MYDIR) && $(MAKE)

baremetal-release: | check-ebbrt-src check-ebbrt-sysroot $(BAREMETAL_RELEASE_DIR) 
	$(CD) $(BAREMETAL_RELEASE_DIR) && \
		EBBRT_SYSROOT=$(EBBRTSYSROOT) $(CMAKE) -DCMAKE_TOOLCHAIN_FILE=$(CMAKE_TOOLCHAIN_FILE) $(MYDIR) && $(MAKE)

hosted-debug:

hosted-release:

.PHONY: Debug Release all check-src baremetal baremetal-debug baremetal-release hosted hosted-debug hosted-release 
