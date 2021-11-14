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

/**
 * @brief Allocates a full page of memory on the heap
 */
void init_mem_alloc();

/**
 * @brief Adds a block to the beginning of the free list
 * 
 * @param block The block to add
 */
void push_front(block_t* block);

/**
 * @brief Inserts a block into the free list based upon
 * the address of the block. This maintains the ascending order 
 * of the list and allows for easy coalescing
 * 
 * @param block The block to insert
 */
void insert_with_ordering(block_t* block);

/**
 * @brief Removes the specified block from the list
 * 
 * @param block The block to remove
 */
void remove_from_list(block_t* block);

/**
 * @brief Print the contents of each block in the free list, including
 * the number of bytes allocated and the address
 */
void dump_free_list();

/**
 * @brief Splits a block into two halves. The first half 
 * is the block allocated, the second half is the free block containing
 * the remaining bytes
 * 
 * @param block The block to split
 * @param bytes The number of bytes needed
 * @return The free block
 */
block_t* split_block(block_t* block, size_t bytes);

/**
 * @brief Performs a linear search on the list to find the first block 
 * that can be allocated
 * @param bytes The number of bytes to be allocated
 * @return The allocated block if there exists a large enough block, NULL if not
 */
block_t* find_first_fit(size_t bytes);

/**
 * @brief Attempts to allocate the specified amount of bytes on the heap.
 * @param bytes The amount of bytes to allocate
 * @return The allocated memory upon success, or NULL if more memory cannot be allocated.
 */
void* mem_alloc(size_t bytes);

/**
 * @brief Copies n bytes starting at the source address into
 * the destination 
 * 
 * @param src The pointer contents to copy
 * @param destination The pointer to copy to
 * @param bytes The number of bytes to copy
 */
void mem_copy(void* src, void* destination, size_t bytes);

/**
 * @brief Deallocates the memory for the specified pointer
 * 
 * @param ptr The pointer to deallocate memory for
 */
void mem_free(void* ptr);

/**
 * @brief Allocates an array in a contiguous block of memory of size n * size bytes
 * 
 * @param n The number of elements
 * @param size The size of the element type
 */
void* mem_calloc(size_t n, size_t size);

/**
 *  Merges two contiguous blocks into one continuous block 
 */
void merge(block_t* block, block_t* neighbor);

/**
 * Coalesces contiguous free blocks into one continuous block
 * of memory
 */
void coalesce_free_blocks();

#endif // __MEM_ALLOC_H__