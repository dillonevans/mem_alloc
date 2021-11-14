#ifndef __BLOCK_T_H__
#define __BLOCK_T_H__

#include <stdio.h>

typedef unsigned char byte;
typedef struct block_t block_t;

struct block_t
{
    block_t* next, * prev;
    size_t size;
};

#define BLOCK_SIZE ((sizeof(block_t) + 7 ) & (-8))
#define NEIGHBOR(block) ((block_t*)((byte*)block + BLOCK_SIZE + block->size))

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
 *  Merges two contiguous blocks into one continuous block 
 */
void merge(block_t* block, block_t* neighbor);

/**
 * Coalesces contiguous free blocks into one continuous block
 * of memory
 */
void coalesce_free_blocks();

#endif // __BLOCK_T_H__