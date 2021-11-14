CC=gcc
CFLAGS=-Wall

all: block_t.o mem_alloc.o demo.o demo clean ./demo

demo.o: demo.c ../include/mem_alloc.h ../include/block_t.h

block_t.o: block_t.c ../include/block_t.h
mem_alloc.o: mem_alloc.c ../include/mem_alloc.h ../include/block_t.h

demo: block_t.o mem_alloc.o demo.o
	$(CC) -o demo mem_alloc.o block_t.o demo.o

clean: 
	rm -f *.o
