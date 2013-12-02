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
	view.c

TARGET = libV8.so

CFLAGS += -fPIC
LDFLAGS += -shared
LIBS += -pthread -lm -L$(V8_ROOT)/vendor/dist/lib -llua
INCFLAGS +=

all: target

include $(V8_ROOT)/rules.make
