# Tools
SHELL = /bin/bash
CC = gcc
LD = gcc
MAKE = make
RM = rm
MKDIR = mkdir
ECHO = echo
EXIT = exit
SED = sed
SET = set
INSTALL = install

# Default to debug
ifeq ($(CFG),)
CFG=debug
endif

# Config
INSTALL_ROOT = /usr/local
BUILD_ROOT = $(V8_ROOT)/build/$(CFG)
LUA = lua-5.2.2


ifeq ($(CFG),debug)
CFLAGS = -ggdb -D_DEBUG
else
CFLAGS = -O2
endif

# Flags
CFLAGS += -Wall -Werror
INCFLAGS += -I$(V8_ROOT)/include -I$(V8_ROOT)/vendor/dist/include
