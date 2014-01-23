V8_ROOT=.

include defs.make

.PHONY: all inform clean build_dir lua v8 example tests

.SILENT:

default: inform build_dir lua v8

all: inform build_dir lua v8 install example tests

inform:
ifneq ($(CFG),release)
ifneq ($(CFG),debug)
	$(ECHO) "Invalid configuration "$(CFG)" specified."
	$(ECHO) "You must specify a configuration when running make, e.g."
	$(ECHO) "make CFG=debug"
	$(ECHO)
	$(ECHO) "Possible choices for configuration are 'release' and 'debug'. Defaults to 'debug'"
	$(EXIT) 1
endif
endif
	$(ECHO) "------------------------------"
	$(ECHO) "Configuration: "$(CFG)
	$(ECHO) "Compiler: "$(CC)
	$(ECHO) "------------------------------"

clean:
	$(ECHO) "Cleanup"
	$(ECHO) "------------------------------"
	$(RM) -rf build
	$(MAKE) -C vendor/lua-5.2.2/ clean

build_dir:
	$(MKDIR) -p $(BUILD_ROOT)/out/

lua: build_dir
	$(ECHO) "Building lua: "$(LUA)
	$(ECHO) "------------------------------"
	$(MAKE) -C vendor/$(LUA)/
	$(MAKE) -C vendor/$(LUA)/ install

v8: lua
	$(ECHO) "Building V8"
	$(ECHO) "------------------------------"
	$(MAKE) -C src/

install: v8
	$(ECHO) "Installing V8"
	$(ECHO) "------------------------------"
	$(INSTALL) -p -s -m 0755 build/$(CFG)/libV8.so $(INSTALL_ROOT)/lib
	$(MKDIR) -p $(INSTALL_ROOT)/include/v8
	$(INSTALL) -p -m 0644 include/v8/* $(INSTALL_ROOT)/include/v8

example: v8
	$(ECHO) "Building example"
	$(ECHO) "------------------------------"
	$(INSTALL) -p -m 0664 example/v8.conf $(BUILD_ROOT)
	$(INSTALL) -p -m 0664 example/*.html $(BUILD_ROOT)
	$(MAKE) -C example/

tests: v8
	$(ECHO) "Building tests"
	$(ECHO) "------------------------------"
	$(MAKE) -C tests/
