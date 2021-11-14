CC=gcc
CFLAGS=-Wall
DEPS = mem_alloc.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mem_alloc: mem_alloc.o 
	$(CC) -o mem_alloc mem_alloc.o 

clean:
	rm -f **.o 