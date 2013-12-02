V8_ROOT=../..
VPATH=.

include $(V8_ROOT)/defs.make

SRCS = map_test.c

TARGET = map_test

INCFLAGS += -I../
LIBS += -L$(BUILD_ROOT) -lV8

all: target

include $(V8_ROOT)/rules.make
