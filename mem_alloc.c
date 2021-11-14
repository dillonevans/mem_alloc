#include "../include/mem_alloc.h"
#include <unistd.h>

static bool isInitialized = false;

void init_mem_alloc()
{
    isInitialized = true;
    block_t * block = (block_t*)(sbrk(PAGE_SIZE));
    block->size = PAGE_SIZE - BLOCK_SIZE;
    insert_with_ordering(block);
}

void* mem_alloc(size_t bytes)
{
    // Handling spurious allocation requests
    if (bytes <= 0) { return NULL; }

    //If the free list hasn't been initialized, do so.
    if (!isInitialized) { init_mem_alloc(); }

    // Align the data to be a multiple of 8 bytes
    size_t aligned = ALIGN(bytes);

    // Find the first block that has enough bytes available
    block_t* allocatedBlock = find_first_fit(bytes);

    if (allocatedBlock)
    {
        // Remove the allocated block from the free list
        remove_from_list(allocatedBlock);

        // If the allocated block is a perfect fit, simply return the memory
        if (allocatedBlock->size == aligned + BLOCK_SIZE)
        {
            return MEM(allocatedBlock);
        }
        else
        {
            // Split the allocated block into two halves, with this being the remaining free half
            block_t* freeBlock = split_block(allocatedBlock, aligned);

            // Add the block to the list of free blocks
            insert_with_ordering(freeBlock);

            // Return the memory address of the allocated block's available section
            return MEM(allocatedBlock);
        }
    }
    else
    {
        // Allocate a scaled amount of the number of bytes + the size of the block
        size_t allocatedBytes = (aligned * SCALE_FACTOR) + BLOCK_SIZE;
        
        // Increase available heap memory
        byte* startingAddress = sbrk(allocatedBytes);

        // If no more memory could be allocated, return null
        if (startingAddress == UNSUCCESSFUL) {
            return NULL;
        }
        // Create a block just beyond the last page break
        block_t* block = (block_t*)(startingAddress);
        block->size = allocatedBytes - BLOCK_SIZE;

        // Insert the new block into the free list
        insert_with_ordering(block);

        // Try the allocation again with the addition space
        return mem_alloc(aligned);
    }
}

void mem_copy(void* src, void* destination, size_t bytes)
{
    byte* a = (byte*)src;
    byte* b = (byte*)destination;
    for (int i = 0; i < bytes; i++)
    {
        *(b++) = *(a++);
    }
}

void mem_free(void* ptr)
{
    if (ptr != NULL)
    {
        // Find the block containing the memory segment pointed to
        block_t* header = BLOCK(ptr);

        // Add the block to the list of free blocks
        insert_with_ordering(header);

        // Coalesce any contiguous free-blocks
        coalesce_free_blocks();
    }
}

void* mem_calloc(size_t n, size_t size) 
{
    // Returns a contiguous block of n x size bytes
    return mem_alloc(n * size);        
}