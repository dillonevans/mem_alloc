#ifndef __MEM_ALLOC_H__
#define __MEM_ALLOC_H__

#include <stdio.h>
#include "block_t.h"
#include <stdbool.h>

#define PAGE_SIZE getpagesize()
#define UNSUCCESSFUL (void*)(-1)
#define SCALE_FACTOR 2
#define ALIGN(bytes) (bytes % 8 == 0 ? bytes : ((bytes + 7) & (-8)))
#define BLOCK(ptr) ((block_t*)((byte*)ptr - BLOCK_SIZE))
#define MEM(block) ((void*)((byte*)block + BLOCK_SIZE))

/**
 * @brief Allocates a full page of memory on the heap
 */
void init_mem_alloc();

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

#endif // __MEM_ALLOC_H__