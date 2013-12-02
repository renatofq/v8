# The directories containing the source files, separated by ':'
VPATH=src

# To make "debug" the default configuration if invoked with just "make":
#
ifeq ($(CFG),)
CFG=debug
endif

# The source files: regardless of where they reside in the source tree,
# VPATH will locate them...
V8_SRC = \
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

# Build a Dependency list and an Object list, by replacing the .c
# extension to .d for dependency files, and .o for object files.
V8_DEP = $(patsubst %.c, build/$(CFG)/out/%.d, ${V8_SRC})
V8_OBJ = $(patsubst %.c, build/$(CFG)/out/%.o, ${V8_SRC})

# Your final binary
TARGET=libV8.so

# What compiler to use
CC = gcc

# What include flags to pass to the compiler
INCLUDEFLAGS = -Iinclude -Ivendor/dist/include

# Common flags for all configurations
CFLAGS += -Wall -Werror -fPIC ${INCLUDEFLAGS}
LDFLAGS += -shared -lm -Lvendor/dist/lib -llua


# Separate compile options per configuration
ifeq ($(CFG),debug)
CFLAGS += -ggdb -D_DEBUG -fPIC
LDFLAGS += -Wl,--incremental
else
CFLAGS += -O2
endif


all:	inform build/$(CFG)/${TARGET}

inform:
ifneq ($(CFG),release)
ifneq ($(CFG),debug)
	@echo "Invalid configuration "$(CFG)" specified."
	@echo "You must specify a configuration when running make, e.g."
	@echo  "make CFG=debug"
	@echo
	@echo  "Possible choices for configuration are 'release' and 'debug'"
	@exit 1
endif
endif
	@echo "Configuration "$(CFG)
	@echo "------------------------"

lua:
	@make -C vendor/lua-5.2.2/ install

build/$(CFG)/${TARGET}: ${V8_OBJ} lua | inform
	@mkdir -p $(dir $@)
	$(CC) -g -o $@ $^ ${LDFLAGS}

build/$(CFG)/out/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

build/$(CFG)/out/%.d: %.c
	@mkdir -p $(dir $@)
	@echo Generating dependencies for $<
	@set -e ; $(CC) -MM -MP $(INCLUDEFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,objs.$(CFG)\/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	@rm -rf build
	@make -C vendor/lua-5.2.2/ clean

# Unless "make clean" is called, include the dependency files
# which are auto-generated. Don't fail if they are missing
# (-include), since they will be missing in the first invocation!
ifneq ($(MAKECMDGOALS),clean)
-include ${V8_DEP}
endif
