#include <stdio.h>
typedef unsigned char byte;
typedef struct block_t block_t;

struct block_t
{
    block_t* next, * prev;
    size_t size;
};

#define MEGABYTE 1024 * 1024
#define MAX_BYTES 16 * MEGABYTE
#define BLOCK_SIZE ((sizeof(block_t) | 15) + 1)
#define ALIGN(bytes) (bytes % 8 == 0 ? bytes : ((bytes + 7) & (-8)))
#define BLOCK(ptr) ((block_t*)((unsigned long)ptr - BLOCK_SIZE))
#define MEM(ptr) ((void*)((unsigned long)ptr + BLOCK_SIZE))

static byte mallocBuffer[MAX_BYTES];
static block_t* head = NULL;

void init_mem_alloc()
{
    head = (block_t*)(mallocBuffer);
    head->size = MAX_BYTES - BLOCK_SIZE;
    head->next = NULL;
    head->prev = NULL;
}

void add_to_list(block_t* block)
{
	//TODO: Add block after a block with a lower address
	
    if (!head)
    {
        head = block;
        head->prev = NULL;
        head->next = NULL;
    }
    else
    {
		block_t *current = head;
	while (current->next)
	{
		printf("DIFF %d\n", (unsigned long)block - (unsigned long)current);
		current = current->next;
	}
        block->next = head;
        block->prev = NULL;
        head->prev = block;
        head = block;
    }
}
/**
 * 
 * 
 */
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
    printf("Dumping Free List...\n");

    while (current)
    {
    	printf("Address: 0x%p, Size: %d\n", current, current->size);
        current = current->next;
    }

    printf("Finished Dump of Free List\n\n");
}

block_t* split_block(block_t* block, size_t bytes)
{
    /*
     * Casting the address of block to an unsigned long is necessary because 
     * otherwise the amount added is scaled by the amount of bytes in a block_t
     * struct
     */ 
    block_t* newBlock = (block_t*)((unsigned long)block + bytes + BLOCK_SIZE);

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
        if (current->size >= (unsigned long)bytes + BLOCK_SIZE)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void mem_free(void* ptr)
{
    // Find the block containing the memory segment pointed to
    block_t* header = BLOCK(ptr);

    //Add the block to the list of free blocks
    add_to_list(header);
}

void coalesce_list()
{

}

void* mem_alloc(size_t bytes)
{
    // Handling spurious allocation requests
    if (bytes <= 0) {return NULL;}

    // If the free list hasn't been created, do so
    if (!head) {init_mem_alloc();}

    // Align the data to be a multiple of 16
    size_t aligned = ALIGN(bytes);
    
    // Find the first block that has enough bytes available
    block_t* allocatedBlock = find_first_fit(bytes);
    
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
        add_to_list(freeBlock);

        // Return the memory address of the allocated block's available memory
        return MEM(allocatedBlock);
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

int main()
{
    int* a = mem_alloc(128);

    
    
    dump_free_list();

	block_t *maybe = BLOCK(a);
	printf("%d\n", (unsigned long)head - (unsigned long)maybe - 128 - 16);
}
