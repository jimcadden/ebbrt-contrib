MYDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
MKROOT ?= $(abspath $(CURDIR))
THISFILE := $(MYDIR)/Makefile
-include config.mk

CD ?= cd
CMAKE ?= cmake
MAKE ?= make
MKDIR ?= mkdir
RM ?= -rm
TEST ?= test

EBBRT_SRCDIR ?= $(abspath $(HOME)/EbbRT)
EBBRT_SRC ?= $(abspath $(EBBRT_SRCDIR)/src)
EBBRT_MAKEFILE ?= $(abspath $(EBBRT_SRCDIR)/toolchain/Makefile)

PREFIX ?= $(abspath $(CURDIR))
INSTALL_ROOT ?= $(PREFIX)
BUILD_ROOT ?= $(PREFIX)
SYSROOT ?= $(INSTALL_ROOT)/sysroot
HOSTED ?= $(INSTALL_ROOT)/hosted

SEED := $(shell bash -c 'echo $$RANDOM')
BUILD_PREFIX ?= _build
HOSTED_BUILD_DIR_PREFIX ?= $(BUILD_PREFIX)$(SEED)_hosted
NATIVE_BUILD_DIR_PREFIX ?= $(BUILD_PREFIX)$(SEED)_native
HOSTED_BUILD_DIR ?= $(BUILD_ROOT)/$(HOSTED_BUILD_DIR_PREFIX)
NATIVE_BUILD_DIR ?= $(BUILD_ROOT)/$(NATIVE_BUILD_DIR_PREFIX)

ifdef CLEAN
CLEANUP ?= $(RM) -rf  
else
CLEANUP ?= $(TEST)
endif

ifdef DEBUG
CMAKE_OPT ?= -DCMAKE_BUILD_TYPE=Debug
else
CMAKE_OPT ?= -DCMAKE_BUILD_TYPE=Release
endif

all: hosted native

clean:
	$(CD) $(BUILD_ROOT) && \
	$(RM) -rf $(BUILD_PREFIX)*

hosted: | $(EBBRT_SRC)
	$(MKDIR) -p $(HOSTED_BUILD_DIR) && $(CD) $(HOSTED_BUILD_DIR) && \
	$(CMAKE) -DCMAKE_INSTALL_PREFIX=$(HOSTED) $(CMAKE_OPT) $(EBBRT_SRC) && \
	$(MAKE) install && \
	$(CD) - && \
	$(CLEANUP) $(HOSTED_BUILD_DIR)
	
native: | $(EBBRT_MAKEFILE)
	$(MKDIR) -p $(NATIVE_BUILD_DIR) && $(CD) $(NATIVE_BUILD_DIR) && \
	$(MAKE) -f $(EBBRT_MAKEFILE) SYSROOT=$(SYSROOT) CMAKE_OPT=$(CMAKE_OPT) && \
	$(CD) - && \
	$(CLEANUP) $(NATIVE_BUILD_DIR)

.PHONY: all hosted native 


