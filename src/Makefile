include ../Make.inc

SRCS =  nj.c

OBJS = $(SRCS:%.c=%.o)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS-add) $(CFLAGS) $(fPIC) -DNDEBUG -c $< -o $@

release debug: libnj.$(SHLIB_EXT)

libnj.$(SHLIB_EXT): $(OBJS)
	rm -rf $@
	$(CC) $(LDFLAGS) $(LDFLAGS-add) -shared -o $@ $^

clean:
	rm -f *.o *.$(SHLIB_EXT)
