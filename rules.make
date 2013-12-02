DEPS = $(patsubst %.c, $(V8_ROOT)/build/$(CFG)/out/%.d, $(SRCS))
OBJS = $(patsubst %.c, $(V8_ROOT)/build/$(CFG)/out/%.o, $(SRCS))

target: $(V8_ROOT)/build/$(CFG)/${TARGET}

$(V8_ROOT)/build/$(CFG)/${TARGET}: ${OBJS}
	$(ECHO) "Linking "$(TARGET)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

$(V8_ROOT)/build/$(CFG)/out/%.o: %.c
	$(ECHO) "Compiling "$<
	$(CC) -c $(CFLAGS) $(INCFLAGS) -o $@ $<

$(V8_ROOT)/build/$(CFG)/out/%.d: %.c
	$(ECHO) "Generating dependencies for "$<
	$(SET) -e ; $(CC) -MM -MP $(INCFLAGS) $< > $@.$$$$; \
	$(SED) 's,\($*\)\.o[ :]*,objs.$(CFG)\/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# Unless "make clean" is called, include the dependency files
# which are auto-generated. Don't fail if they are missing
# (-include), since they will be missing in the first invocation!
ifneq ($(MAKECMDGOALS),clean)
-include ${DEPS}
endif
