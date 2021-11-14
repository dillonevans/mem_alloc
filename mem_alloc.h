#ifndef __MEM_ALLOC_H__
#define __MEM_ALLOC_H__

#include <stdio.h>

typedef unsigned char byte;
typedef struct block_t block_t;

struct block_t
{
    block_t* next, * prev;
    size_t size;
};

#define PAGE_SIZE getpagesize()
#define UNSUCCESSFUL (void*)(-1)
#define SCALE_FACTOR 2
#define BLOCK_SIZE ((sizeof(block_t) + 7 ) & (-8))
#define ALIGN(bytes) (bytes % 8 == 0 ? bytes : ((bytes + 7) & (-8)))
#define BLOCK(ptr) ((block_t*)((byte*)ptr - BLOCK_SIZE))
#define MEM(block) ((void*)((byte*)block + BLOCK_SIZE))
#define NEIGHBOR(block) ((block_t*)((byte*)block + BLOCK_SIZE + block->size))

static block_t* head = NULL;

void init_mem_alloc();
void push_front(block_t* block);
void insert_with_ordering(block_t* block);
void remove_from_list(block_t* block);
void dump_free_list();

block_t* split_block(block_t* block, size_t bytes);
block_t* find_first_fit(size_t bytes);

void* mem_alloc(size_t bytes);
void mem_copy(void* src, void* destination, size_t bytes);
void merge(block_t* block, block_t* neighbor);
void coalesce_free_blocks();
void mem_free(void* ptr);


#endif // __MEM_ALLOC_H__