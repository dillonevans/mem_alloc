CC=gcc
CFLAGS=-Wall

all: demo clean

demo.o: demo.c ../include/mem_alloc.h ../include/free_list.h
free_list.o: free_list.c ../include/free_list.h
mem_alloc.o: mem_alloc.c ../include/mem_alloc.h ../include/free_list.h

demo: free_list.o mem_alloc.o demo.o
	$(CC) -o demo mem_alloc.o free_list.o demo.o

clean: 
	rm -f *.o
