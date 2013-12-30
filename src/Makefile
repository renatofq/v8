VPATH = .
V8_ROOT = ..

include $(V8_ROOT)/defs.make

SRCS = \
	buffer.c \
	config.c \
	cookie.c \
	list.c \
	log.c \
	lua.c \
	map.c \
	request.c \
	response.c \
	scgi.c \
	strmap.c \
	v8.c \
	dispatcher.c \
	view.c

TARGET = libV8.so

ifeq ($(CFG),debug)
CFLAGS += -DV8_DEBUG
endif

CFLAGS += -fPIC
LDFLAGS += -shared
LIBS += -lm -L$(V8_ROOT)/vendor/dist/lib -llua


all: target

include $(V8_ROOT)/rules.make
