#include "mem_alloc.h"
#include <unistd.h>

void init_mem_alloc()
{
    head = (block_t*)(sbrk(PAGE_SIZE));
    head->size = PAGE_SIZE - BLOCK_SIZE;
    head->next = NULL;
    head->prev = NULL;
}

void push_front(block_t* block)
{
    if (!head)
    {
        head = block;
        head->prev = NULL;
        head->next = NULL;
    }
    else
    {
        block->next = head;
        block->prev = NULL;
        head->prev = block;
        head = block;
    }
}

void insert_with_ordering(block_t* block)
{
    /**
     * If the head is null or the address of the block is less than that of the head,
     * simply add the block to the beginning of the list
     */
    if (!head || block < head)
    {
        push_front(block);
    }
    else
    {
        block_t* current = head, * next = NULL;
        while (current->next)
        {
            /**
             * If the address of the block is less than the next block in the list,
             * this block belongs between the current block and the next block
             */
            if (block < current->next)
            {
                next = current->next;

                // Insert node between current and next node
                current->next = block;
                next->prev = block;
                block->next = next;
                block->prev = current;
                return;
            }

            /**
             * Keep iterating until the block is placed or the tail is reached
             */
            current = current->next;
        }

        /**
         * Reached tail of the list. At this point, current is the tail of the list.
         * Add the block to the end of the tail.
         */
        current->next = block;
        block->next = NULL;
        block->prev = current;
    }
}

void remove_from_list(block_t* block)
{
    block_t* prev = block->prev, * next = block->next;

    if (block == head)
    {
        head = next;
        if (next) { next->prev = head; }
    }
    else
    {
        prev->next = next;
        if (next) { next->prev = prev; }
    }
}

void dump_free_list()
{
    block_t* current = head;
    printf("Contents of Free List:\n");
    while (current)
    {
        printf("Address: 0x%p, Size: %lu\n", current, current->size);
        current = current->next;
    }
}

block_t* split_block(block_t* block, size_t bytes)
{
    /*
     * Casting the address of block to a byte pointer is necessary because
     * otherwise the amount added is scaled by the amount of bytes in a block_t
     * struct
     */
    block_t* newBlock = (block_t*)((byte*)block + bytes + BLOCK_SIZE);

    newBlock->size = block->size - bytes - BLOCK_SIZE;
    block->size = bytes;
    return newBlock;
}

block_t* find_first_fit(size_t bytes)
{
    block_t* current = head;

    // Iterate over all blocks in the list to find the first one large enough
    while (current)
    {
        if (current->size >= bytes + BLOCK_SIZE)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void* mem_alloc(size_t bytes)
{
    // Handling spurious allocation requests
    if (bytes <= 0) { return NULL; }

    // If the free list hasn't been created, do so
    if (!head) { init_mem_alloc(); }

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

void merge(block_t* block, block_t* neighbor)
{
    /**
     * Consume the size of the neighboring block and remove it from the list
     */
    block->size = block->size + neighbor->size;
    remove_from_list(neighbor);
}

void coalesce_free_blocks()
{
    block_t* current = head;
    
    // Traverse the free-list
    while (current)
    {
        // If the neighboring block in memory is the next free block 
        if (NEIGHBOR(current) == current->next)
        {
            // Merge the current block and the next block into one
            merge(current, current->next);
        }
        current = current->next;
    }
}

void mem_free(void* ptr)
{
    // Find the block containing the memory segment pointed to
    block_t* header = BLOCK(ptr);

    // Add the block to the list of free blocks
    insert_with_ordering(header);

    // Coalesce any contiguous free-blocks
    coalesce_free_blocks();
}

void* mem_calloc(size_t n, size_t size) 
{
    // Returns a contiguous block of n x size bytes
    return mem_alloc(n * size);        
}

int main()
{
    // Allocate a full page
    int* a = mem_alloc(sizeof(int) * 1024);

    // Write to allocated memory
    for (int i = 0; i < 1024; i++)
    {
        *(a + i) = i;
    }

    // Allocate half a page
    int *b = mem_alloc(sizeof(int) * 512);

    // Free the memory allocated by a
    mem_free(a);

    // Write to allocated memory
    for (int i = 0; i < 512; i++)
    {
        *(b + i) = i;
    }

    // Display the free list
    dump_free_list();
}