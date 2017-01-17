include ../Make.inc

CFLAGS-add += -I../src -DNDEBUG
LDFLAGS-add += -lm

all: test

test.o: test.c
	$(CC) $(CPPFLAGS) $(CFLAGS-add) $(CFLAGS) $(fPIC) -c $< -o $@

nj.o: ../src/nj.c
	$(CC) $(CPPFLAGS) $(CFLAGS-add) $(CFLAGS) $(fPIC) -c $< -o $@

test: test.o nj.o
	$(CC) $(LDFLAGS)  -o $@ $^ $(LDFLAGS-add)

clean:
	rm -f *.o test
